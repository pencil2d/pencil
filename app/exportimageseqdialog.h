#ifndef EXPORTIMAGESEQDIALOG_H
#define EXPORTIMAGESEQDIALOG_H

#include "importexportdialog.h"

namespace Ui {
class ExportImageSeqOptions;
}

class ExportImageSeqDialog : public ImportExportDialog
{
    Q_OBJECT

public:
    explicit ExportImageSeqDialog(QWidget* parent = 0);
    ~ExportImageSeqDialog();

    void  setExportSize( QSize size );
    QSize getExportSize();

    bool getTransparency();

    QString getExportFormat();

protected:
    Mode getMode();
    FileType getFileType();

private slots:
    void formatChanged(QString format);

private:
    Ui::ExportImageSeqOptions* ui;
};

#endif // EXPORTIMAGESEQDIALOG_H
