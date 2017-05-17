#include "exportimagedialog.h"
#include "ui_exportimageoptions.h"

ExportImageDialog::ExportImageDialog(QWidget *parent, bool seq) :
    ImportExportDialog(parent),
    ui(new Ui::ExportImageOptions),
    m_fileType(seq ? FileType::IMAGE_SEQUENCE : FileType::IMAGE)
{
    ui->setupUi( getOptionsGroupBox() );
    init();
    if (seq)
    {
        setWindowTitle( tr( "Export image sequence" ) );
    }
    else
    {
        setWindowTitle( tr( "Export image" ) );
    }

    connect( ui->formatComboBox, &QComboBox::currentTextChanged, this, &ExportImageDialog::formatChanged );
    formatChanged( getExportFormat() ); // Make sure file extension matches format combobox
}

ExportImageDialog::~ExportImageDialog()
{
    delete ui;
}

void ExportImageDialog::setExportSize(QSize size)
{
    ui->imgWidthSpinBox->setValue( size.width() );
    ui->imgHeightSpinBox->setValue( size.height() );
}

QSize ExportImageDialog::getExportSize()
{
    return QSize( ui->imgWidthSpinBox->value(), ui->imgHeightSpinBox->value() );
}

bool ExportImageDialog::getTransparency()
{
    return ui->cbTransparency->checkState() == Qt::Checked;
}

QString ExportImageDialog::getExportFormat()
{
    return ui->formatComboBox->currentText();
}

ImportExportDialog::Mode ExportImageDialog::getMode()
{
    return ImportExportDialog::Export;
}

FileType ExportImageDialog::getFileType()
{
    return m_fileType;
}

void ExportImageDialog::formatChanged(QString format)
{
    setFileExtension( format.toLower() );
}
