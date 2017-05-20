#ifndef EXPORTIMAGEDIALOG_H
#define EXPORTIMAGEDIALOG_H

#include "importexportdialog.h"

namespace Ui {
class ExportImageOptions;
}

class ExportImageDialog : public ImportExportDialog
{
    Q_OBJECT

public:
    explicit ExportImageDialog(QWidget* parent = 0, bool seq = false);
    ~ExportImageDialog();

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
    Ui::ExportImageOptions* ui;

    FileType m_fileType;
};

#endif // EXPORTIMAGEDIALOG_H
