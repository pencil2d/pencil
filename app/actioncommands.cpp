/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "actioncommands.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QApplication>
#include <QDesktopServices>

#include "pencildef.h"
#include "editor.h"
#include "object.h"
#include "viewmanager.h"
#include "layermanager.h"
#include "soundmanager.h"
#include "playbackmanager.h"
#include "preferencemanager.h"
#include "util.h"
#include "app_util.h"

#include "layercamera.h"
#include "layersound.h"
#include "bitmapimage.h"
#include "vectorimage.h"
#include "soundclip.h"
#include "camera.h"

#include "movieexporter.h"
#include "filedialogex.h"
#include "exportmoviedialog.h"
#include "exportimagedialog.h"
#include "aboutdialog.h"


ActionCommands::ActionCommands(QWidget* parent) : QObject(parent)
{
    mParent = parent;
}

ActionCommands::~ActionCommands() {}

Status ActionCommands::importSound()
{
    Layer* layer = mEditor->layers()->currentLayer();
    Q_ASSERT(layer);
    NULLReturn(layer, Status::FAIL);

    if (layer->type() != Layer::SOUND)
    {
        QMessageBox msg;
        msg.setText(tr("No sound layer exists as a destination for your import. Create a new sound layer?"));
        msg.addButton(tr("Create sound layer"), QMessageBox::AcceptRole);
        msg.addButton(tr("Don't create layer"), QMessageBox::RejectRole);

        int buttonClicked = msg.exec();
        if (buttonClicked != QMessageBox::AcceptRole)
        {
            return Status::SAFE;
        }

        // Create new sound layer.
        bool ok = false;
        QString strLayerName = QInputDialog::getText(mParent, tr("Layer Properties", "Dialog title on creating a sound layer"),
                                                     tr("Layer name:"), QLineEdit::Normal,
                                                     tr("Sound Layer", "Default name on creating a sound layer"), &ok);
        if (ok && !strLayerName.isEmpty())
        {
            Layer* newLayer = mEditor->layers()->createSoundLayer(strLayerName);
            mEditor->layers()->setCurrentLayer(newLayer);
        }
        else
        {
            Q_ASSERT(false);
            return Status::FAIL;
        }
    }

    layer = mEditor->layers()->currentLayer();
    Q_ASSERT(layer->type() == Layer::SOUND);


    int currentFrame = mEditor->currentFrame();
    SoundClip* key = nullptr;

    if (layer->keyExists(currentFrame))
    {
        key = static_cast<SoundClip*>(layer->getKeyFrameAt(currentFrame));
        if (!key->fileName().isEmpty())
        {
            QMessageBox::warning(nullptr, "",
                                 tr("A sound clip already exists on this frame! Please select another frame or layer."));
            return Status::SAFE;
        }
    }
    else
    {
        key = new SoundClip;
        layer->addKeyFrame(currentFrame, key);
    }

    FileDialog fileDialog(mParent);
    QString strSoundFile = fileDialog.openFile(FileType::SOUND);

    if (strSoundFile.isEmpty())
    {
        return Status::SAFE;
    }

    Status st = mEditor->sound()->loadSound(key, strSoundFile);

    if (!st.ok())
    {
        layer->removeKeyFrame(currentFrame);
    }

    return st;
}

Status ActionCommands::exportMovie()
{
    mExportMovieDialog = new ExportMovieDialog(mParent);

    std::vector< std::pair<QString, QSize> > camerasInfo;
    auto cameraLayers = mEditor->object()->getLayersByType< LayerCamera >();
    for (LayerCamera* i : cameraLayers)
    {
        camerasInfo.push_back(std::make_pair(i->name(), i->getViewSize()));
    }

    auto currLayer = mEditor->layers()->currentLayer();
    if (currLayer->type() == Layer::CAMERA)
    {
        QString strName = currLayer->name();
        auto it = std::find_if(camerasInfo.begin(), camerasInfo.end(),
            [strName](std::pair<QString, QSize> p)
        {
            return p.first == strName;
        });

        Q_ASSERT(it != camerasInfo.end());

        std::swap(camerasInfo[0], *it);
    }

    mExportMovieDialog->setCamerasInfo(camerasInfo);

    int projectLenWithSounds = mEditor->layers()->projectLength(true);
    int projectLen = mEditor->layers()->projectLength(false);

    mExportMovieDialog->setDefaultRange(1, projectLen, projectLenWithSounds);

    if (!mExportMovieDialog->isVisible())
    {
        mExportMovieDialog->exec();
    }
    if (mExportMovieDialog->result() == QDialog::Rejected)
    {
        return Status::SAFE;
    }
    QString strMoviePath = mExportMovieDialog->getFilePath();

    ExportMovieDesc desc;
    desc.strFileName = strMoviePath;
    desc.startFrame = mExportMovieDialog->getStartFrame();
    desc.endFrame = mExportMovieDialog->getEndFrame();
    desc.fps = mEditor->playback()->fps();
    desc.exportSize = mExportMovieDialog->getExportSize();
    desc.strCameraName = mExportMovieDialog->getSelectedCameraName();

    QProgressDialog progressDlg;
    progressDlg.setWindowModality(Qt::WindowModal);
    progressDlg.setLabelText(tr("Exporting movie..."));
    Qt::WindowFlags eFlags = Qt::Dialog | Qt::WindowTitleHint;
    progressDlg.setWindowFlags(eFlags);
    progressDlg.show();

    MovieExporter ex;

    connect(&progressDlg, &QProgressDialog::canceled, [&ex]
    {
        ex.cancel();
    });

    Status st = ex.run(mEditor->object(), desc, [&progressDlg](float f)
    {
        progressDlg.setValue((int)(f * 100.f));
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    });

    if (st.ok() && QFile::exists(strMoviePath))
    {
        auto btn = QMessageBox::question(mParent,
            "Pencil2D",
            tr("Finished. Open movie now?"));
        if (btn == QMessageBox::Yes)
        {
            QDesktopServices::openUrl(QUrl::fromLocalFile(strMoviePath));
        }
    }
    delete mExportMovieDialog;
    return Status::OK;
}

Status ActionCommands::exportImageSequence()
{
    // Options
    auto dialog = new ExportImageDialog(mParent, true);
    OnScopeExit(dialog->deleteLater());

    std::vector< std::pair<QString, QSize> > camerasInfo;
    auto cameraLayers = mEditor->object()->getLayersByType< LayerCamera >();
    for (LayerCamera* i : cameraLayers)
    {
        camerasInfo.push_back(std::make_pair(i->name(), i->getViewSize()));
    }

    auto currLayer = mEditor->layers()->currentLayer();
    if (currLayer->type() == Layer::CAMERA)
    {
        QString strName = currLayer->name();
        auto it = std::find_if(camerasInfo.begin(), camerasInfo.end(),
            [strName](std::pair<QString, QSize> p)
        {
            return p.first == strName;
        });

        Q_ASSERT(it != camerasInfo.end());
        std::swap(camerasInfo[0], *it);
    }
    dialog->setCamerasInfo(camerasInfo);

    dialog->exec();

    if (dialog->result() == QDialog::Rejected)
    {
        return Status::SAFE;
    }

    QString strFilePath = dialog->getFilePath();
    QSize exportSize = dialog->getExportSize();
    QString exportFormat = dialog->getExportFormat();
    bool useTranparency = dialog->getTransparency();

    int projectLength = mEditor->layers()->projectLength();

    QString sCameraLayerName = dialog->getCameraLayerName();
    LayerCamera* cameraLayer = (LayerCamera*)mEditor->layers()->getLayerByName(sCameraLayerName);

    // Show a progress dialog, as this can take a while if you have lots of frames.
    QProgressDialog progress(tr("Exporting image sequence..."), tr("Abort"), 0, 100, mParent);
    hideQuestionMark(progress);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    mEditor->object()->exportFrames(1, projectLength,
        cameraLayer,
        exportSize,
        strFilePath,
        exportFormat,
        useTranparency,
        true,
        &progress,
        100);

    progress.close();

    return Status::OK;
}

Status ActionCommands::exportImage()
{
    // Options
    auto dialog = new ExportImageDialog(mParent);
    OnScopeExit(dialog->deleteLater());

    std::vector< std::pair<QString, QSize> > camerasInfo;
    auto cameraLayers = mEditor->object()->getLayersByType< LayerCamera >();
    for (LayerCamera* i : cameraLayers)
    {
        camerasInfo.push_back(std::make_pair(i->name(), i->getViewSize()));
    }

    auto currLayer = mEditor->layers()->currentLayer();
    if (currLayer->type() == Layer::CAMERA)
    {
        QString strName = currLayer->name();
        auto it = std::find_if(camerasInfo.begin(), camerasInfo.end(),
            [strName](std::pair<QString, QSize> p)
        {
            return p.first == strName;
        });

        Q_ASSERT(it != camerasInfo.end());
        std::swap(camerasInfo[0], *it);
    }
    dialog->setCamerasInfo(camerasInfo);

    dialog->exec();

    if (dialog->result() == QDialog::Rejected)
    {
        return Status::SAFE;
    }

    QString filePath = dialog->getFilePath();
    QSize exportSize = dialog->getExportSize();
    QString exportFormat = dialog->getExportFormat();
    bool useTranparency = dialog->getTransparency();

    // Export
    QString sCameraLayerName = dialog->getCameraLayerName();
    LayerCamera* cameraLayer = (LayerCamera*)mEditor->layers()->getLayerByName(sCameraLayerName);

    QTransform view = cameraLayer->getViewAtFrame(mEditor->currentFrame());

    bool bOK = mEditor->object()->exportIm(mEditor->currentFrame(),
                                           view,
                                           cameraLayer->getViewSize(),
                                           exportSize,
                                           filePath,
                                           exportFormat,
                                           true,
                                           useTranparency);

    if (!bOK)
    {
        QMessageBox::warning(mParent,
            tr("Warning"),
            tr("Unable to export image."),
            QMessageBox::Ok);
        return Status::FAIL;
    }
    return Status::OK;
}

void ActionCommands::ZoomIn()
{
    mEditor->view()->snapScale(1);
}

void ActionCommands::ZoomOut()
{
    mEditor->view()->snapScale(-1);
}

void ActionCommands::flipSelectionX()
{
    bool flipVertical = false;
    mEditor->flipSelection(flipVertical);
}

void ActionCommands::flipSelectionY()
{
    bool flipVertical = true;
    mEditor->flipSelection(flipVertical);
}

void ActionCommands::rotateClockwise()
{
    float currentRotation = mEditor->view()->rotation();
    mEditor->view()->rotate(currentRotation + 15.f);
}

void ActionCommands::rotateCounterClockwise()
{
    float currentRotation = mEditor->view()->rotation();
    mEditor->view()->rotate(currentRotation - 15.f);
}

void ActionCommands::toggleMirror()
{
    bool flipX = mEditor->view()->isFlipHorizontal();
    mEditor->view()->flipHorizontal(!flipX);
}

void ActionCommands::toggleMirrorV()
{
    bool flipY = mEditor->view()->isFlipVertical();
    mEditor->view()->flipVertical(!flipY);
}

void ActionCommands::showGrid(bool bShow)
{
    auto prefs = mEditor->preference();
    if (bShow)
        prefs->turnOn(SETTING::GRID);
    else
        prefs->turnOff(SETTING::GRID);
}

void ActionCommands::PlayStop()
{
    PlaybackManager* playback = mEditor->playback();
    if (playback->isPlaying())
    {
        playback->stop();
    }
    else
    {
        playback->play();
    }
}

void ActionCommands::GotoNextFrame()
{
    mEditor->scrubForward();
}

void ActionCommands::GotoPrevFrame()
{
    mEditor->scrubBackward();
}

void ActionCommands::GotoNextKeyFrame()
{
    mEditor->scrubNextKeyFrame();
}

void ActionCommands::GotoPrevKeyFrame()
{
    mEditor->scrubPreviousKeyFrame();
}

Status ActionCommands::addNewKey()
{
    KeyFrame* key = mEditor->addNewKey();

    SoundClip* clip = dynamic_cast<SoundClip*>(key);
    if (clip)
    {
        FileDialog fileDialog(mParent);
        QString strSoundFile = fileDialog.openFile(FileType::SOUND);

        if (strSoundFile.isEmpty())
        {
            mEditor->layers()->currentLayer()->removeKeyFrame(clip->pos());
            return Status::SAFE;
        }
        Status st = mEditor->sound()->loadSound(clip, strSoundFile);
        if (!st.ok())
        {
            mEditor->layers()->currentLayer()->removeKeyFrame(clip->pos());
            return Status::ERROR_LOAD_SOUND_FILE;
        }
    }

    Camera* cam = dynamic_cast<Camera*>(key);
    if (cam)
    {
        auto camLayer = static_cast<LayerCamera*>(mEditor->layers()->currentLayer());
        Q_ASSERT(camLayer);

        camLayer->LinearInterpolateTransform(cam);
        mEditor->view()->updateViewTransforms();
    }
    return Status::OK;
}

void ActionCommands::removeKey()
{
    mEditor->removeKey();

    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->keyFrameCount() == 0)
    {
        switch (layer->type())
        {
        case Layer::BITMAP:
        case Layer::VECTOR:
        case Layer::SOUND:
        case Layer::CAMERA:
            layer->addNewEmptyKeyAt(1);
            break;
        default:
            break;
        }
    }
}

Status ActionCommands::addNewBitmapLayer()
{
    bool ok;
    QString text = QInputDialog::getText(nullptr, tr("Layer Properties"),
                                         tr("Layer name:"), QLineEdit::Normal,
                                         tr("Bitmap Layer"), &ok);
    if (ok && !text.isEmpty())
    {
        mEditor->layers()->createBitmapLayer(text);
    }
    return Status::OK;
}

Status ActionCommands::addNewVectorLayer()
{
    bool ok;
    QString text = QInputDialog::getText(nullptr, tr("Layer Properties"),
                                         tr("Layer name:"), QLineEdit::Normal,
                                         tr("Vector Layer"), &ok);
    if (ok && !text.isEmpty())
    {
        mEditor->layers()->createVectorLayer(text);
    }

    return Status::OK;
}

Status ActionCommands::addNewCameraLayer()
{
    bool ok;
    QString text = QInputDialog::getText(nullptr, tr("Layer Properties"),
                                         tr("Layer name:"), QLineEdit::Normal,
                                         tr("Camera Layer"), &ok);
    if (ok && !text.isEmpty())
    {
        mEditor->layers()->createCameraLayer(text);
    }

    return Status::OK;
}

Status ActionCommands::addNewSoundLayer()
{
    bool ok = false;
    QString strLayerName = QInputDialog::getText(nullptr, tr("Layer Properties"),
                                                 tr("Layer name:"), QLineEdit::Normal,
                                                 tr("Sound Layer"), &ok);
    if (ok && !strLayerName.isEmpty())
    {
        Layer* layer = mEditor->layers()->createSoundLayer(strLayerName);
        mEditor->layers()->setCurrentLayer(layer);

        return Status::OK;
    }
    return Status::FAIL;
}

Status ActionCommands::deleteCurrentLayer()
{
    LayerManager* layerMgr = mEditor->layers();
    QString strLayerName = layerMgr->currentLayer()->name();

    int ret = QMessageBox::warning(mParent,
                                   tr("Delete Layer", "Windows title of Delete current layer pop-up."),
                                   tr("Are you sure you want to delete layer: ") + strLayerName + " ?",
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok)
    {
        Status st = layerMgr->deleteLayer(mEditor->currentLayerIndex());
        if (st == Status::ERROR_NEED_AT_LEAST_ONE_CAMERA_LAYER)
        {
            QMessageBox::information(mParent, "",
                                     tr("Please keep at least one camera layer in project", "text when failed to delete camera layer"));
        }
    }
    return Status::OK;
}


void ActionCommands::help()
{
    QString url = "http://www.pencil2d.org/documentation/";
    QDesktopServices::openUrl(QUrl(url));
}

void ActionCommands::website()
{
    QString url = "http://pencil2d.github.io/";
    QDesktopServices::openUrl(QUrl(url));
}

void ActionCommands::reportbug()
{
    QString url = "https://github.com/pencil2d/pencil/issues";
    QDesktopServices::openUrl(QUrl(url));
}

void ActionCommands::about()
{
    AboutDialog* aboutBox = new AboutDialog(mParent);
    aboutBox->setAttribute(Qt::WA_DeleteOnClose);
    aboutBox->init();
    aboutBox->exec();
}
