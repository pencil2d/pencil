#ifndef IMPORTIMAGESEQDIALOG_H
#define IMPORTIMAGESEQDIALOG_H

#include "importexportdialog.h"

namespace Ui {
class ImportImageSeqOptions;
}

class ImportImageSeqDialog : public ImportExportDialog
{
    Q_OBJECT

public:
    explicit ImportImageSeqDialog(QWidget *parent = 0);
    ~ImportImageSeqDialog();

    int getSpace();

protected:
    Mode getMode();
    FileType getFileType();

private slots:
    void setSpace(int number);

private:
    Ui::ImportImageSeqOptions *ui;
};

#endif // IMPORTIMAGESEQDIALOG_H
