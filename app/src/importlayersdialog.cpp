#include "importlayersdialog.h"
#include "ui_importlayersdialog.h"

#include <QFileDialog>
#include <QProgressDialog>

#include "app_util.h"
#include "filemanager.h"
#include "filedialogex.h"
#include "layermanager.h"
#include "layer.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"
#include "layersound.h"

#include <QDebug>

ImportLayersDialog::ImportLayersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportLayersDialog)
{
    ui->setupUi(this);
    connect(ui->btnSelectFile, &QPushButton::clicked, this, &ImportLayersDialog::getFileName);
    connect(ui->btnImportLayers, &QPushButton::clicked, this, &ImportLayersDialog::importLayers);
    connect(ui->lwLayers, &QListWidget::itemSelectionChanged, this, &ImportLayersDialog::listWidgetChanged);
    connect(ui->btnCancel, &QPushButton::clicked, this, &ImportLayersDialog::cancel);
    ui->lwLayers->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->btnImportLayers->setEnabled(false);
}

ImportLayersDialog::~ImportLayersDialog()
{
    delete ui;
}

void ImportLayersDialog::setCore(Editor *editor)
{
    mEditor = editor;
}

void ImportLayersDialog::getFileName()
{
    FileDialog fd(this);
    mFileName = QFileDialog::getOpenFileName(this, tr("Choose file"),
                                             fd.getLastOpenPath(FileType::ANIMATION) , tr("Project files (*.pclx))"));
    getLayers();
    for (int i = 0; i < mObject->getLayerCount(); i++)
        ui->lwLayers->addItem(mObject->getLayer(i)->name());
}

void ImportLayersDialog::listWidgetChanged()
{
    if (ui->lwLayers->count() > 0)
        ui->btnImportLayers->setEnabled(true);
    else
        ui->btnImportLayers->setEnabled(false);
}

void ImportLayersDialog::importLayers()
{
//    if (mLayerList.isEmpty()) { return; }

    for (int i = 0; i < mObject->getLayerCount(); i++ )
    {
        if (ui->lwLayers->item(i)->isSelected())
        {
            Layer *tmpLayer = mObject->findLayerByName(ui->lwLayers->item(i)->text());
            Layer *newLayer = nullptr;
            switch (tmpLayer->type()) {
            case Layer::BITMAP:
                newLayer = static_cast<LayerBitmap*>(mEditor->layers()->createBitmapLayer(tmpLayer->name()));
                break;
            case Layer::VECTOR:
                newLayer = static_cast<LayerVector*>(mEditor->layers()->createVectorLayer(tmpLayer->name()));
                break;
            case Layer::CAMERA:
                newLayer = static_cast<LayerCamera*>(mEditor->layers()->createCameraLayer(tmpLayer->name()));
                break;
            case Layer::SOUND:
                newLayer = static_cast<LayerSound*>(mEditor->layers()->createSoundLayer(tmpLayer->name()));
                break;
            default:
                newLayer = nullptr;
            }
            qDebug() << "Layer: " << tmpLayer->name();
            for (int j = 1; j <= tmpLayer->getMaxKeyFramePosition(); j++)
            {
                if (tmpLayer->keyExists(j))
                {
                    qDebug() << "Key at: " << j;
                }
            }
        }
    }
    close();
}

void ImportLayersDialog::cancel()
{
    close();
}

void ImportLayersDialog::getLayers()
{
    QProgressDialog progress(tr("Opening document..."), tr("Abort"), 0, 100, this);

    // Don't show progress bar if running without a GUI (aka. when rendering from command line)
    if (isVisible())
    {
        hideQuestionMark(progress);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
    }

    FileManager fm;
    connect(&fm, &FileManager::progressChanged, [&progress](int p)
    {
        progress.setValue(p);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    });
    connect(&fm, &FileManager::progressRangeChanged, [&progress](int max)
    {
        progress.setRange(0, max + 3);
    });
    mObject = fm.load(mFileName);
}
