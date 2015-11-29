#include "exportimageseqdialog.h"
#include "ui_exportimageseqdialog.h"

ExportImageSeqDialog::ExportImageSeqDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportImageSeqDialog)
{
    ui->setupUi(this);

    connect( ui->buttonBox, &QDialogButtonBox::accepted, [=]
    {

    });
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
