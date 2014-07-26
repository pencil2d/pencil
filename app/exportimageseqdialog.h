#ifndef EXPORTIMAGESEQDIALOG_H
#define EXPORTIMAGESEQDIALOG_H

#include <QDialog>

namespace Ui {
class ExportImageSeqDialog;
}

class ExportImageSeqDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportImageSeqDialog(QWidget *parent = 0);
    ~ExportImageSeqDialog();

private:
    Ui::ExportImageSeqDialog *ui;
};

#endif // EXPORTIMAGESEQDIALOG_H
