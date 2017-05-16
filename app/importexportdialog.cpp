#include "importexportdialog.h"
#include "ui_importexportdialog.h"

ImportExportDialog::ImportExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportExportDialog),
    m_fileDialog(new FileDialog(this))
{
    ui->setupUi(this);

    connect(ui->browseButton, &QPushButton::clicked, this, &ImportExportDialog::browse);

    Qt::WindowFlags eFlags = Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint;
    setWindowFlags(eFlags);
}

ImportExportDialog::~ImportExportDialog()
{
    delete ui;
}

QString ImportExportDialog::getFilePath()
{
    return ui->fileEdit->text();
}

void ImportExportDialog::init()
{
    switch (getMode())
    {
        case Import:
            ui->fileEdit->setText(m_fileDialog->getLastOpenPath(getFileType()));
            break;
        case Export:
            ui->fileEdit->setText(m_fileDialog->getLastSavePath(getFileType()));
            break;
        default:
            Q_ASSERT(false);
    }
}

QGroupBox *ImportExportDialog::getOptionsGroupBox()
{
    return ui->optionsGroupBox;
}

void ImportExportDialog::browse()
{
    QString path;
    switch (getMode())
    {
        case Import:
            path = m_fileDialog->openFile(getFileType());
            break;
        case Export:
            path = m_fileDialog->saveFile(getFileType());
            break;
        default:
            Q_ASSERT(false);
    }

    ui->fileEdit->setText(path);
}
