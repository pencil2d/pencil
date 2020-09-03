#include "importlayersdialog.h"
#include "ui_importlayersdialog.h"

#include <QFileDialog>
#include <QProgressDialog>

#include "app_util.h"
#include "filemanager.h"
#include "filedialogex.h"
#include "fileformat.h"
#include "layermanager.h"
#include "soundmanager.h"
#include "layer.h"
#include "layersound.h"
#include "soundclip.h"


ImportLayersDialog::ImportLayersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportLayersDialog)
{
    ui->setupUi(this);
    connect(ui->btnSelectFile, &QPushButton::clicked, this, &ImportLayersDialog::getFileName);
    connect(ui->btnImportLayers, &QPushButton::clicked, this, &ImportLayersDialog::importLayers);
    connect(ui->lwLayers, &QListWidget::itemSelectionChanged, this, &ImportLayersDialog::listWidgetChanged);
    connect(ui->btnClose, &QPushButton::clicked, this, &ImportLayersDialog::cancel);
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
    mObject = mEditor->object();
}

void ImportLayersDialog::getFileName()
{
    mFileName.clear();
    ui->lwLayers->clear();
    FileDialog fd(this);
    mFileName = QFileDialog::getOpenFileName(this, tr("Choose file"),
                                             fd.getLastOpenPath(FileType::ANIMATION),
                                             PFF_PROJECT_EXT_FILTER);
    if (mFileName.isEmpty()) { return; }
    getLayers();
}

void ImportLayersDialog::listWidgetChanged()
{
    mItemsSelected.clear();
    for (int i = 0; i < ui->lwLayers->count(); i++)
        if (ui->lwLayers->item(i)->isSelected())
            mItemsSelected.append(i);

    if (!mItemsSelected.isEmpty())
        ui->btnImportLayers->setEnabled(true);
    else
        ui->btnImportLayers->setEnabled(false);
}

void ImportLayersDialog::importLayers()
{
    int currentFrame = mEditor->currentFrame();
    for (int i = 0; i < mImportObject->getLayerCount(); i++ )
    {
        if (ui->lwLayers->item(i)->isSelected())
        {
            mImportLayer = mImportObject->findLayerByName(ui->lwLayers->item(i)->text());
            mImportLayer->setName(mEditor->layers()->nameSuggestLayer(ui->lwLayers->item(i)->text()));
            if (mImportLayer->type() == Layer::SOUND)
            {
                LayerSound* layerSound = static_cast<LayerSound*>(mImportLayer);
                int count = 0;
                while (count < layerSound->getNextKeyFramePosition(count))
                {
                    int newKeyPos = layerSound->getNextKeyFramePosition(count);
                    SoundClip* clip = new SoundClip;
                    clip = layerSound->getSoundClipWhichCovers(newKeyPos);
                    Status st = mEditor->sound()->loadSound(clip, clip->fileName());
                    count = newKeyPos;
                }
                mObject->addLayer(layerSound);
            }
            else
            {
                mObject->addLayer(mImportLayer);
            }
            mEditor->object()->modification();
        }
    }
    mImportObject = nullptr;
    getLayers();
    mEditor->scrubTo(currentFrame);
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
    mImportObject = fm.load(mFileName);

    ui->lwLayers->clear();
    for (int i = 0; i < mImportObject->getLayerCount(); i++)
        ui->lwLayers->addItem(mImportObject->getLayer(i)->name());

}
