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
    explicit ExportImageSeqDialog(QWidget* parent = 0);
    ~ExportImageSeqDialog();

    void  setExportSize( QSize size );
    QSize getExportSize();

    bool getTransparency();

    QString getExportFormat();

private:
    Ui::ExportImageSeqDialog* ui;
};

#endif // EXPORTIMAGESEQDIALOG_H
