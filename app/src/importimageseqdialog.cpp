/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

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

#include "editor.h"

#include <QProgressDialog>
#include <QMessageBox>
#include <QDir>
#include <QtDebug>

ImportImageSeqDialog::ImportImageSeqDialog(QWidget* parent, Mode mode, FileType fileType, ImportCriteria importCriteria) :
    ImportExportDialog(parent, mode, fileType), mParent(parent), mImportCriteria(importCriteria), mFileType(fileType)
{
    uiOptionsBox = new Ui::ImportImageSeqOptions;
    uiOptionsBox->setupUi(getOptionsGroupBox());

    if (importCriteria == ImportCriteria::Numbered) {
        setInstructionsLabel(tr("Select an image that matches the criteria: Name000.png, eg. Joe001.png \n"
                             "The preview box below will show the rest of the images matching the same criteria.\n\n"
                             "A new layer will be created and the imports will be added to that layer \n"
                             "Keyframes will be added based on the image index, eg. Name002.png, will create a keyframe on pos 2 \non the timeline."));
        hideOptionsGroupBox(true);
    } else {
        hideInstructionsLabel(true);
    }

    uiGroupBoxPreview = new Ui::ImportImageSeqPreviewGroupBox;
    uiGroupBoxPreview->setupUi(getPreviewGroupBox());

    if (fileType == FileType::GIF) {
        setWindowTitle(tr("Import Animated GIF"));
    } else {
        setWindowTitle(tr("Import image sequence"));
    }

    connect(uiOptionsBox->spaceSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ImportImageSeqDialog::setSpace);
    connect(this, &ImportImageSeqDialog::filePathsChanged, this, &ImportImageSeqDialog::updatePreviewList);
}

ImportImageSeqDialog::~ImportImageSeqDialog()
{
    delete uiOptionsBox;
    delete uiGroupBoxPreview;
}

int ImportImageSeqDialog::getSpace()
{
    return uiOptionsBox->spaceSpinBox->value();
}

void ImportImageSeqDialog::updatePreviewList(const QStringList& list)
{

    if (mImportCriteria == ImportCriteria::Numbered)
    {
        uiGroupBoxPreview->listWidget->addItems(getFilePaths());
    } else {
        uiGroupBoxPreview->listWidget->addItems(list);
    }
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
    SignalBlocker b1(uiOptionsBox->spaceSpinBox);
    uiOptionsBox->spaceSpinBox->setValue(number);
}

void ImportImageSeqDialog::importArbitrarySequence()
{
    QStringList files = getFilePaths();
    int number = getSpace();

    // Show a progress dialog, as this can take a while if you have lots of images.
    QProgressDialog progress(tr("Importing image sequence..."), tr("Abort"), 0, 100, mParent);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    int totalImagesToImport = files.count();
    int imagesImportedSoFar = 0;
    progress.setMaximum(totalImagesToImport);

    QString failedFiles;
    bool failedImport = false;
    for (const QString& strImgFile : files)
    {
        QString strImgFileLower = strImgFile.toLower();

        if (strImgFileLower.endsWith(".png") ||
            strImgFileLower.endsWith(".jpg") ||
            strImgFileLower.endsWith(".jpeg") ||
            strImgFileLower.endsWith(".bmp") ||
            strImgFileLower.endsWith(".tif") ||
            strImgFileLower.endsWith(".tiff"))
        {
            mEditor->importImage(strImgFile);

            imagesImportedSoFar++;
            progress.setValue(imagesImportedSoFar);
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);  // Required to make progress bar update

            if (progress.wasCanceled())
            {
                break;
            }
        }
        else
        {
            failedFiles += strImgFile + "\n";
            if (!failedImport)
            {
                failedImport = true;
            }
        }

        for (int i = 1; i < number; i++)
        {
            mEditor->scrubForward();
        }
    }

    if (failedImport)
    {
        QMessageBox::warning(mParent,
                             tr("Warning"),
                             tr("was unable to import") + failedFiles,
                             QMessageBox::Ok,
                             QMessageBox::Ok);
    }

    emit notifyAnimationLengthChanged();
    progress.close();
}

NumberedFiles ImportImageSeqDialog::numberedFiles()
{
    QString strFilePath = getFilePath();
    NumberedFiles numberedFiles;

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
        return numberedFiles;
    }

    digit = strFilePath.mid(dot - digits, digits);
    QString prefix = strFilePath.mid(slash + 1, dot - slash - digits - 1);
    QString suffix = strFilePath.mid(dot, strFilePath.length() - 1);

    QDir dir = strFilePath.left(strFilePath.lastIndexOf("/"));
    QStringList sList = dir.entryList(QDir::Files, QDir::Name);
    if (sList.isEmpty()) { return numberedFiles; }

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
    if (finalList.isEmpty()) { return numberedFiles; }

    // List of relevant files is not empty. Let's validate them
    dot = finalList[0].lastIndexOf(".");

    QStringList absolutePaths;
    for (QString fileName : finalList) {
        absolutePaths << path + fileName;
    }

    numberedFiles.dot = dot;
    numberedFiles.digits = digits;
    numberedFiles.filenames = finalList;
    numberedFiles.folderPath = path;
    numberedFiles.absolutePaths = absolutePaths;
    numberedFiles.prefix = prefix;
    return numberedFiles;
}

void ImportImageSeqDialog::importNumberedSequence()
{
    NumberedFiles numbFiles = numberedFiles();

    const QStringList& list = numbFiles.filenames;
    const int& dot = numbFiles.dot;
    const int& digits = numbFiles.digits;
    const QString& folderPath = numbFiles.folderPath;

    mEditor->createNewBitmapLayer(numbFiles.prefix);

    for (int i = 0; i < list.size(); i++)
    {
        const int& frameIndex = list[i].mid(dot - digits, digits).toInt();
        const QString& absolutePath = folderPath + list[i];

        mEditor->scrubTo(frameIndex);
        bool ok = mEditor->importImage(absolutePath);
        if (!ok) { return;}
    }

    emit notifyAnimationLengthChanged();
}

QStringList ImportImageSeqDialog::getFilePaths()
{
    if (mImportCriteria == ImportCriteria::Numbered)
    {
        NumberedFiles numFiles = numberedFiles();

        Status status = numFiles.pathsValid();
        if (status == Status::OK) {
            return numFiles.absolutePaths;
        } else {
            QMessageBox::warning(mParent,
                                 status.title(),
                                 status.description(),
                                 QMessageBox::Ok,
                                 QMessageBox::Ok);
        }
        return QStringList();
    }
    return ImportExportDialog::getFilePaths();
}

Status NumberedFiles::pathsValid() const
{
    QString msg = "";
    QString failedPathsString;

    Status status = Status::OK;

    if (filenames.isEmpty()) { status = Status::FAIL; }

    for (int i = 0; i < filenames.size(); i++)
    {
        const int& frameIndex = filenames[i].mid(dot - digits, digits).toInt();
        if (!(frameIndex && (frameIndex > 0)))
        {
            status = Status::FAIL;
            failedPathsString += filenames[i] +  ", ";
        }
    }

    if (filenames.size() > 0 && status == Status::FAIL)
    {
        status.setTitle("Invalid paths");
        status.setDescription(QString("The following files had one or more invalid names: ").arg(failedPathsString));
    } else if (status == Status::FAIL) {
        status.setTitle("Input file");
        status.setDescription("The input file did not match the criteria for importing. Read the instructions and try again");
    }

    return status;
}
