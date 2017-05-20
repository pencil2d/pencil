#ifndef EXPORTMOVIEDIALOG_H
#define EXPORTMOVIEDIALOG_H

#include "importexportdialog.h"

namespace Ui {
class ExportMovieOptions;
}

class ExportMovieDialog : public ImportExportDialog
{
    Q_OBJECT

public:
    explicit ExportMovieDialog(QWidget* parent = 0);
    ~ExportMovieDialog();

    void setCamerasInfo( std::vector< std::pair< QString, QSize > > );
    void updateResolutionCombo( int index );

    void setDefaultRange( int startFrame, int endFrame );

    QString getSelectedCameraName();
    QSize getExportSize();

    int getStartFrame();
    int getEndFrame();

protected:
    Mode getMode();
    FileType getFileType();

private:
    Ui::ExportMovieOptions* ui = nullptr;
};

#endif // EXPORTMOVIEDIALOG_H
