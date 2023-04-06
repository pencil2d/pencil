/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "importimageseqdialog.h"
#include "ui_importimageseqoptions.h"
#include "ui_importimageseqpreview.h"
#include "util.h"
#include "app_util.h"

#include "editor.h"
#include "errordialog.h"
#include "predefinedsetmodel.h"
#include "layermanager.h"
#include "viewmanager.h"

#include <QProgressDialog>
#include <QMessageBox>
#include <QDir>
#include <QtDebug>
#include <QDialogButtonBox>
#include <QPushButton>

ImportImageSeqDialog::ImportImageSeqDialog(QWidget* parent, Mode mode, FileType fileType, ImportCriteria importCriteria) :
    ImportExportDialog(parent, mode, fileType), mParent(parent), mImportCriteria(importCriteria), mFileType(fileType)
{

    uiOptionsBox = new Ui::ImportImageSeqOptions;
    uiOptionsBox->setupUi(getOptionsGroupBox());

    uiGroupBoxPreview = new Ui::ImportImageSeqPreviewGroupBox;
    uiGroupBoxPreview->setupUi(getPreviewGroupBox());

    if (importCriteria == ImportCriteria::PredefinedSet) {
        setupPredefinedLayout();
    } else {
        setupLayout();
    }

    getDialogButtonBox()->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);
}

void ImportImageSeqDialog::setupLayout()
{

    hideInstructionsLabel(true);

    if (mFileType == FileType::GIF) {
        setWindowTitle(tr("Import Animated GIF"));
    } else {
        setWindowTitle(tr("Import image sequence"));
    }

    connect(uiOptionsBox->spaceSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ImportImageSeqDialog::setSpace);
    connect(this, &ImportImageSeqDialog::filePathsChanged, this, &ImportImageSeqDialog::validateFiles);
}

void ImportImageSeqDialog::setupPredefinedLayout()
{
    setWindowTitle(tr("Import predefined keyframe set"));
    setInstructionsLabel(tr("Select an image that matches the criteria: MyFile000.png, eg. Joe001.png \n"
                         "The importer will search and find images matching the same criteria. You can see the result in the preview box below."));
    hideOptionsGroupBox(true);
    hidePreviewGroupBox(false);

    connect(this, &ImportImageSeqDialog::filePathsChanged, this, &ImportImageSeqDialog::updatePreviewList);
}

ImportImageSeqDialog::~ImportImageSeqDialog()
{
    if (uiOptionsBox) {
        delete uiOptionsBox;
    }
    if (uiGroupBoxPreview) {
        delete uiGroupBoxPreview;
    }
}

int ImportImageSeqDialog::getSpace()
{
    return uiOptionsBox->spaceSpinBox->value();
}

void ImportImageSeqDialog::updatePreviewList(const QStringList& list)
{
    Q_UNUSED(list)
    if (mImportCriteria == ImportCriteria::PredefinedSet)
    {
        const PredefinedKeySet& keySet = generatePredefinedKeySet();

        Status status = Status::OK;
        status = validateKeySet(keySet, list);

        QPushButton* okButton = getDialogButtonBox()->button(QDialogButtonBox::StandardButton::Ok);
        if (status == Status::FAIL)
        {
            QMessageBox::warning(mParent,
                                 status.title(),
                                 status.description(),
                                 QMessageBox::Ok,
                                 QMessageBox::Ok);
            okButton->setEnabled(false);
        } else {
            okButton->setEnabled(true);
        }
        setPreviewModel(keySet);
    }
}

const PredefinedKeySet ImportImageSeqDialog::generatePredefinedKeySet() const
{
    PredefinedKeySet keySet;
    const PredefinedKeySetParams& setParams = predefinedKeySetParams();

    const QStringList& filenames = setParams.filenames;
    const int& digits = setParams.digits;
    const QString& folderPath = setParams.folderPath;

    for (int i = 0; i < filenames.size(); i++)
    {
        const int& frameIndex = filenames[i].mid(setParams.dot - digits, digits).toInt();
        const QString& absolutePath = folderPath + filenames[i];

        keySet.insert(frameIndex, absolutePath);
    }
    keySet.setLayerName(setParams.prefix);
    return keySet;
}

void ImportImageSeqDialog::setPreviewModel(const PredefinedKeySet& keySet)
{
    PredefinedSetModel* previewModel = new PredefinedSetModel(nullptr, keySet);
    uiGroupBoxPreview->tableView->setModel(previewModel);
    uiGroupBoxPreview->tableView->setColumnWidth(0, 500);
    uiGroupBoxPreview->tableView->setColumnWidth(1, 100);
}

ImportExportDialog::Mode ImportImageSeqDialog::getMode()
{
    return ImportExportDialog::Import;
}

FileType ImportImageSeqDialog::getFileType()
{
    return mFileType;
}

void ImportImageSeqDialog::setSpace(int number)
{
    QSignalBlocker b1(uiOptionsBox->spaceSpinBox);
    uiOptionsBox->spaceSpinBox->setValue(number);
}

void ImportImageSeqDialog::importArbitrarySequence()
{
    QStringList files = getFilePaths();
    int number = getSpace();

    // Show a progress dialog, as this can take a while if you have lots of images.
    QProgressDialog progress(tr("Importing image sequence..."), tr("Abort"), 0, 100, mParent);
    hideQuestionMark(progress);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    int totalImagesToImport = files.count();
    int imagesImportedSoFar = 0;
    progress.setMaximum(totalImagesToImport);

    for (const QString& strImgFile : files)
    {
        QString strImgFileLower = strImgFile.toLower();

        Status st = mEditor->importImage(strImgFile);
        if (!st.ok())
        {
            ErrorDialog errorDialog(st.title(), st.description(), st.details().html());
            errorDialog.exec();
            break;
        }

        imagesImportedSoFar++;
        progress.setValue(imagesImportedSoFar);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);  // Required to make progress bar update

        if (progress.wasCanceled())
        {
            break;
        }

        for (int i = 1; i < number; i++)
        {
            mEditor->scrubForward();
        }
    }


    emit notifyAnimationLengthChanged();
    progress.close();
}

const PredefinedKeySetParams ImportImageSeqDialog::predefinedKeySetParams() const
{
    QString strFilePath = getFilePath();
    PredefinedKeySetParams setParams;

    // local vars for testing file validity
    int dot = strFilePath.lastIndexOf(".");
    int slash = strFilePath.lastIndexOf("/");
    QString fName = strFilePath.mid(slash + 1);
    QString path = strFilePath.left(slash + 1);
    QString digit = strFilePath.mid(slash + 1, dot - slash - 1);

    // Find number of digits (min: 1, max: digit.length - 1)
    int digits = 0;
    for (int i = digit.length() - 1; i > 0; i--)
    {
        if (digit.at(i).isDigit())
        {
            digits++;
        }
        else
        {
            break;
        }
    }

    if (digits < 1)
    {
        return setParams;
    }

    digit = strFilePath.mid(dot - digits, digits);
    QString prefix = strFilePath.mid(slash + 1, dot - slash - digits - 1);
    QString suffix = strFilePath.mid(dot, strFilePath.length() - 1);

    QDir dir = strFilePath.left(strFilePath.lastIndexOf("/"));
    QStringList sList = dir.entryList(QDir::Files, QDir::Name);
    if (sList.isEmpty()) { return setParams; }

    // List of files is not empty. Let's go find the relevant files
    QStringList finalList;
    int validLength = prefix.length() + digit.length() + suffix.length();
    for (int i = 0; i < sList.size(); i++)
    {
        if (sList[i].startsWith(prefix) &&
                sList[i].length() == validLength &&
                sList[i].mid(sList[i].lastIndexOf(".") - digits, digits).toInt() > 0 &&
                sList[i].endsWith(suffix))
        {
            finalList.append(sList[i]);
        }
    }
    if (finalList.isEmpty()) { return setParams; }

    // List of relevant files is not empty. Let's validate them
    dot = finalList[0].lastIndexOf(".");

    QStringList absolutePaths;
    for (QString fileName : finalList) {
        absolutePaths << path + fileName;
    }

    setParams.dot = dot;
    setParams.digits = digits;
    setParams.filenames = finalList;
    setParams.folderPath = path;
    setParams.absolutePaths = absolutePaths;
    setParams.prefix = prefix;
    return setParams;
}

void ImportImageSeqDialog::importPredefinedSet()
{
    PredefinedKeySet keySet = generatePredefinedKeySet();

    // Show a progress dialog, as this can take a while if you have lots of images.
    QProgressDialog progress(tr("Importing images..."), tr("Abort"), 0, 100, mParent);
    hideQuestionMark(progress);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    int totalImagesToImport = keySet.size();
    int imagesImportedSoFar = 0;
    progress.setMaximum(totalImagesToImport);

    mEditor->layers()->createBitmapLayer(keySet.layerName());

    for (int i = 0; i < keySet.size(); i++)
    {
        const int& frameIndex = keySet.keyFrameIndexAt(i);
        const QString& filePath = keySet.filePathAt(i);

        mEditor->scrubTo(frameIndex);
        Status st = mEditor->importImage(filePath);
        if (!st.ok())
        {
            ErrorDialog errorDialog(st.title(), st.description(), st.details().html());
            errorDialog.exec();
            break;
        }
        imagesImportedSoFar++;

        progress.setValue(imagesImportedSoFar);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);  // Required to make progress bar update

        if (progress.wasCanceled())
        {
            break;
        }
    }

    emit notifyAnimationLengthChanged();
}

QStringList ImportImageSeqDialog::getFilePaths()
{
    return ImportExportDialog::getFilePaths();
}

Status ImportImageSeqDialog::validateKeySet(const PredefinedKeySet& keySet, const QStringList& filepaths)
{
    QString msg = "";
    QString failedPathsString;

    Status status = Status::OK;

    if (filepaths.isEmpty()) { status = Status::FAIL; }

    if (keySet.isEmpty())
    {
        status = Status::FAIL;
        failedPathsString = QLocale().createSeparatedList(filepaths);
    }

    if (status == Status::FAIL)
    {
        status.setTitle(tr("Invalid path"));
        status.setDescription(QString(tr("The following file did not meet the criteria: \n%1 \n\nRead the instructions and try again")).arg(failedPathsString));
    }

    return status;
}

Status ImportImageSeqDialog::validateFiles(const QStringList &filepaths)
{
    QString failedPathsString = "";

    Status status = Status::OK;

    if (filepaths.isEmpty()) { status = Status::FAIL; }

    for (int i = 0; i < filepaths.count(); i++)
    {
        QFileInfo file = filepaths.at(i);
        if (!file.exists())
            failedPathsString += filepaths.at(i) + "\n";
    }

    if (!failedPathsString.isEmpty())
    {
        status = Status::FAIL;
        status.setTitle(tr("Invalid path"));
        status.setDescription(QString(tr("The following file(-s) did not meet the criteria: \n%1")).arg(failedPathsString));
    }

    if (status == Status::OK)
    {
        getDialogButtonBox()->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);
    }
    return status;
}
