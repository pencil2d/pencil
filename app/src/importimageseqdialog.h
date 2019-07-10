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

#ifndef IMPORTIMAGESEQDIALOG_H
#define IMPORTIMAGESEQDIALOG_H

#include "importexportdialog.h"
#include "pencilerror.h"
#include "predefinedsetmodel.h"

class Editor;

namespace Ui {
class ImportImageSeqOptions;
class ImportImageSeqPreviewGroupBox;
}

struct PredefinedKeySetParams {
    int dot;
    int digits;
    QStringList filenames;
    QStringList absolutePaths;
    QString folderPath;
    QString prefix;

    Status pathsValid() const;

    int count() const { return filenames.count();}
};

enum ImportCriteria { Arbitrary, PredefinedSet };

class ImportImageSeqDialog : public ImportExportDialog
{
    Q_OBJECT

public:
    explicit ImportImageSeqDialog(QWidget *parent = nullptr,
                                  Mode mode = ImportExportDialog::Import,
                                  FileType fileType = FileType::IMAGE_SEQUENCE,
                                  ImportCriteria importCriteria = ImportCriteria::Arbitrary);
    ~ImportImageSeqDialog() override;

    void importArbitrarySequence();
    void importPredefinedSet();
    int getSpace();

    void setCore(Editor* editor) { mEditor = editor; }

signals:
    void notifyAnimationLengthChanged();

protected:
    Mode getMode();
    FileType getFileType();

private slots:
    void setSpace(int number);
    void updatePreviewList(const QStringList& list);

    const PredefinedKeySetParams predefinedKeySetParams() const;

private:
    int keyFramePosFromFilePath(const QString& path);

private:

    const PredefinedKeySet generatePredefinedKeySet() const;
    void setPreviewModel(const PredefinedKeySet& predefinedKeySet);
    void setupLayout();
    void setupPredefinedLayout();
    Status validateKeySet(const PredefinedKeySet& keySet, const QStringList& filepaths);

    Ui::ImportImageSeqOptions *uiOptionsBox;
    Ui::ImportImageSeqPreviewGroupBox *uiGroupBoxPreview;

    QStringList getFilePaths();

    Editor* mEditor = nullptr;
    QWidget* mParent = nullptr;
    ImportCriteria mImportCriteria = ImportCriteria::Arbitrary;
    FileType mFileType = FileType::IMAGE_SEQUENCE;
};

#endif // IMPORTIMAGESEQDIALOG_H
