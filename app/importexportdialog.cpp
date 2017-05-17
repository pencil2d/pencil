#include "importexportdialog.h"
#include "ui_importexportdialog.h"
#include <QFileInfo>

ImportExportDialog::ImportExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportExportDialog),
    m_fileDialog(new FileDialog(this)),
    m_filePaths(QStringList())
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
    return m_filePaths.isEmpty() ? QString() : m_filePaths.first();
}

QStringList ImportExportDialog::getFilePaths()
{
    return m_filePaths;
}

void ImportExportDialog::init()
{
    switch (getMode())
    {
        case Import:
            m_filePaths = QStringList(m_fileDialog->getLastOpenPath(getFileType()));
            break;
        case Export:
            m_filePaths = QStringList(m_fileDialog->getLastSavePath(getFileType()));
            break;
        default:
            Q_ASSERT(false);
    }
    ui->fileEdit->setText("\"" + m_filePaths.first() + "\"");
}

QGroupBox *ImportExportDialog::getOptionsGroupBox()
{
    return ui->optionsGroupBox;
}

void ImportExportDialog::setFileExtension(QString extension)
{
    for (int i = 0; i < m_filePaths.size(); i++)
    {
        QFileInfo info(m_filePaths.at(i));
        m_filePaths.replace(i, info.path() + "/" + info.baseName() + "." + extension);
    }
    ui->fileEdit->setText("\"" + m_filePaths.join("\" \"") + "\"");
}

void ImportExportDialog::browse()
{
    switch (getMode())
    {
        case Import:
            if (getFileType() == FileType::IMAGE_SEQUENCE)
            {
                m_filePaths = m_fileDialog->openFiles( FileType::IMAGE_SEQUENCE );
                break;
            }

            m_filePaths = QStringList(m_fileDialog->openFile(getFileType()));
            break;
        case Export:
            m_filePaths = QStringList(m_fileDialog->saveFile(getFileType()));
            break;
        default:
            Q_ASSERT(false);
    }

    ui->fileEdit->setText("\"" + m_filePaths.join("\" \"") + "\"");
}
