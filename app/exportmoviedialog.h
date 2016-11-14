#ifndef EXPORTMOVIEDIALOG_H
#define EXPORTMOVIEDIALOG_H

#include <QDialog>

namespace Ui {
class ExportMovieDialog;
}

class ExportMovieDialog : public QDialog
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

private:
    Ui::ExportMovieDialog* ui = nullptr;
};

#endif // EXPORTMOVIEDIALOG_H
