#include "importimageseqdialog.h"
#include "ui_importimageseqoptions.h"
#include "util.h"

ImportImageSeqDialog::ImportImageSeqDialog(QWidget* parent) :
    ImportExportDialog(parent),
    ui(new Ui::ImportImageSeqOptions)
{
    ui->setupUi(getOptionsGroupBox());
    init();
    setWindowTitle(tr("Import image sequence"));
    connect(ui->spaceSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ImportImageSeqDialog::setSpace);
}

ImportImageSeqDialog::~ImportImageSeqDialog()
{
    delete ui;
}

int ImportImageSeqDialog::getSpace()
{
    return ui->spaceSpinBox->value();
}

ImportExportDialog::Mode ImportImageSeqDialog::getMode()
{
    return ImportExportDialog::Import;
}

FileType ImportImageSeqDialog::getFileType()
{
    return FileType::IMAGE_SEQUENCE;
}

void ImportImageSeqDialog::setSpace(int number)
{
    SignalBlocker b1(ui->spaceSpinBox);
    ui->spaceSpinBox->setValue(number);
}
