#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QDialog>
#include <QGroupBox>
#include "filedialogex.h"

namespace Ui {
class ImportExportDialog;
}

class ImportExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportExportDialog(QWidget *parent = 0);
    ~ImportExportDialog();

    QString getFilePath();
    QStringList getFilePaths();

protected:
    void init();
    QGroupBox *getOptionsGroupBox();
    void setFileExtension(QString extension);

    enum Mode { Import, Export };
    virtual Mode getMode() = 0;

    virtual FileType getFileType() = 0;


private slots:
    void browse();

private:
    Ui::ImportExportDialog *ui;

    FileDialog *m_fileDialog;
    QStringList m_filePaths;
};

#endif // IMPORTEXPORTDIALOG_H
