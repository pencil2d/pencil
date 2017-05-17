#include "exportimageseqdialog.h"
#include "ui_exportimageseqoptions.h"

ExportImageSeqDialog::ExportImageSeqDialog(QWidget *parent) :
    ImportExportDialog(parent),
    ui(new Ui::ExportImageSeqOptions)
{
    ui->setupUi( getOptionsGroupBox() );
    init();
    setWindowTitle( tr( "Export image sequence" ) );

    connect( ui->formatComboBox, &QComboBox::currentTextChanged, this, &ExportImageSeqDialog::formatChanged );
    formatChanged( getExportFormat() ); // Make sure file extension matches format combobox
}

ExportImageSeqDialog::~ExportImageSeqDialog()
{
    delete ui;
}

void ExportImageSeqDialog::setExportSize(QSize size)
{
    ui->imgWidthSpinBox->setValue( size.width() );
    ui->imgHeightSpinBox->setValue( size.height() );
}

QSize ExportImageSeqDialog::getExportSize()
{
    return QSize( ui->imgWidthSpinBox->value(), ui->imgHeightSpinBox->value() );
}

bool ExportImageSeqDialog::getTransparency()
{
    return ui->cbTransparency->checkState() == Qt::Checked;
}

QString ExportImageSeqDialog::getExportFormat()
{
    return ui->formatComboBox->currentText();
}

ImportExportDialog::Mode ExportImageSeqDialog::getMode()
{
    return ImportExportDialog::Export;
}

FileType ExportImageSeqDialog::getFileType()
{
    return FileType::IMAGE_SEQUENCE;
}

void ExportImageSeqDialog::formatChanged(QString format)
{
    setFileExtension( format.toLower() );
}
