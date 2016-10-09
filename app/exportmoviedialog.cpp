#include "exportmoviedialog.h"
#include "ui_exportmoviedialog.h"
#include "util.h"

ExportMovieDialog::ExportMovieDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportMovieDialog)
{
    ui->setupUi(this);
	ui->rangeGroupBox->hide();

	Qt::WindowFlags eFlags = Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint;
	setWindowFlags( eFlags );
}

ExportMovieDialog::~ExportMovieDialog()
{
    delete ui;
}

void ExportMovieDialog::setCamerasInfo( std::vector< std::pair< QString, QSize > > camerasInfo )
{
    if ( ui->cameraCombo->count() > 0 )
    {
        ui->cameraCombo->clear();
    }

    for ( std::pair< QString, QSize >& camera : camerasInfo )
    {
        ui->cameraCombo->addItem( camera.first, camera.second );
    }

    auto indexChanged = static_cast< void(QComboBox::*)( int i ) >( &QComboBox::currentIndexChanged );
    connect( ui->cameraCombo, indexChanged, this, &ExportMovieDialog::updateResolutionCombo );

    updateResolutionCombo( 0 );
}

void ExportMovieDialog::updateResolutionCombo( int index )
{
    QSize camSize = ui->cameraCombo->itemData( index ).toSize();
    
    SignalBlocker b1( ui->widthSpinBox );
    SignalBlocker b2( ui->heightSpinBox );

    ui->widthSpinBox->setValue( camSize.width() );
    ui->heightSpinBox->setValue( camSize.height() );
}

void ExportMovieDialog::setDefaultRange( int startFrame, int endFrame )
{
	ui->startSpinbox->setValue( startFrame );
	ui->endSpinBox->setValue( endFrame );
}

QString ExportMovieDialog::getSelectedCameraName()
{
	return ui->cameraCombo->currentText();
}

QSize ExportMovieDialog::getExportSize()
{
	return QSize( ui->widthSpinBox->value(), ui->heightSpinBox->value() );
}

int ExportMovieDialog::getStartFrame()
{
	return ui->startSpinbox->value();
}

int ExportMovieDialog::getEndFrame()
{
	return ui->endSpinBox->value();
}
