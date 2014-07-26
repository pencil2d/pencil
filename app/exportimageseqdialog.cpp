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
