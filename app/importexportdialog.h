#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QDialog>
#include <QGroupBox>
#include "util.h"
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

protected:
    void init();
    QGroupBox *getOptionsGroupBox();

    enum Mode { Import, Export };
    virtual Mode getMode() = 0;

    virtual FileType getFileType() = 0;


private slots:
    void browse();

private:
    Ui::ImportExportDialog *ui;

    FileDialog *m_fileDialog;
};

#endif // IMPORTEXPORTDIALOG_H
