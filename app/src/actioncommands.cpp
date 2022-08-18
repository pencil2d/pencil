/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

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
#include <QStandardPaths>
#include <QFileDialog>

#include "pencildef.h"
#include "editor.h"
#include "object.h"
#include "viewmanager.h"
#include "layermanager.h"
#include "scribblearea.h"
#include "soundmanager.h"
#include "playbackmanager.h"
#include "colormanager.h"
#include "preferencemanager.h"
#include "selectionmanager.h"
#include "util.h"
#include "app_util.h"

#include "layercamera.h"
#include "layersound.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "bitmapimage.h"
#include "vectorimage.h"
#include "soundclip.h"
#include "camera.h"

#include "movieimporter.h"
#include "movieexporter.h"
#include "filedialog.h"
#include "exportmoviedialog.h"
#include "exportimagedialog.h"
#include "aboutdialog.h"
#include "doubleprogressdialog.h"
#include "checkupdatesdialog.h"
#include "layeropacitydialog.h"
#include "errordialog.h"


ActionCommands::ActionCommands(QWidget* parent) : QObject(parent)
{
    mParent = parent;
}

ActionCommands::~ActionCommands() {}

Status ActionCommands::importMovieVideo()
{
    QString filePath = FileDialog::getOpenFileName(mParent, FileType::MOVIE);
    if (filePath.isEmpty())
    {
        return Status::FAIL;
    }

    // Show a progress dialog, as this can take a while if you have lots of images.
    QProgressDialog progressDialog(tr("Importing movie..."), tr("Abort"), 0, 100, mParent);
    hideQuestionMark(progressDialog);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setMinimumWidth(250);
    progressDialog.show();

    QMessageBox information(mParent);
    information.setIcon(QMessageBox::Warning);
    information.setText(tr("You are importing a lot of frames, beware this could take some time. Are you sure you want to proceed?"));
    information.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    information.setDefaultButton(QMessageBox::Yes);

    MovieImporter importer(this);
    importer.setCore(mEditor);

    connect(&progressDialog, &QProgressDialog::canceled, &importer, &MovieImporter::cancel);

    Status st = importer.run(filePath, mEditor->playback()->fps(), FileType::MOVIE, [&progressDialog](int prog) {
        progressDialog.setValue(prog);
        QApplication::processEvents();
    }, [&progressDialog](QString progMessage) {
        progressDialog.setLabelText(progMessage);
    }, [&information]() {

        int ret = information.exec();
        return ret == QMessageBox::Yes;
    });

    if (!st.ok() && st != Status::CANCELED)
    {
        ErrorDialog errorDialog(st.title(), st.description(), st.details().html(), mParent);
        errorDialog.exec();
    }

    mEditor->layers()->notifyAnimationLengthChanged();
    emit mEditor->framesModified();

    progressDialog.setValue(100);
    progressDialog.close();

    return Status::OK;
}

Status ActionCommands::importSound(FileType type)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr)
    {
        Q_ASSERT(layer);
        return Status::FAIL;
    }

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
                                                     mEditor->layers()->nameSuggestLayer(tr("Sound Layer", "Default name on creating a sound layer")), &ok);
        if (ok && !strLayerName.isEmpty())
        {
            Layer* newLayer = mEditor->layers()->createSoundLayer(strLayerName);
            mEditor->layers()->setCurrentLayer(newLayer);
        }
        else
        {
            return Status::SAFE;
        }
    }

    layer = mEditor->layers()->currentLayer();
    Q_ASSERT(layer->type() == Layer::SOUND);

    // Adding key before getting file name just to make sure the keyframe can be insterted
    SoundClip* key = static_cast<SoundClip*>(mEditor->addNewKey());

    if (key == nullptr)
    {
        // Probably tried to modify a hidden layer or something like that
        // Let Editor handle the warnings
        return Status::SAFE;
    }

    QString strSoundFile = FileDialog::getOpenFileName(mParent, type);

    Status st = Status::FAIL;

    if (strSoundFile.isEmpty())
    {
        st = Status::CANCELED;
    }
    else if (strSoundFile.endsWith(".wav"))
    {
        st = mEditor->sound()->loadSound(key, strSoundFile);
    }
    else
    {
        st = convertSoundToWav(strSoundFile);
    }

    if (!st.ok())
    {
        mEditor->removeKey();
        emit mEditor->layers()->currentLayerChanged(mEditor->layers()->currentLayerIndex()); // trigger timeline repaint.
    } else {
        showSoundClipWarningIfNeeded();
    }

    return st;
}

Status ActionCommands::convertSoundToWav(const QString& filePath)
{
    QProgressDialog progressDialog(tr("Importing sound..."), tr("Abort"), 0, 100, mParent);
    hideQuestionMark(progressDialog);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.show();

    MovieImporter importer(this);
    importer.setCore(mEditor);

    Status st = importer.run(filePath, mEditor->playback()->fps(), FileType::SOUND, [&progressDialog](int prog) {
        progressDialog.setValue(prog);
        QApplication::processEvents();
    }, [](QString progressMessage) {
        Q_UNUSED(progressMessage)
        // Not needed
    }, []() {
        return true;
    });

    connect(&progressDialog, &QProgressDialog::canceled, &importer, &MovieImporter::cancel);

    if (!st.ok() && st != Status::CANCELED)
    {
        ErrorDialog errorDialog(st.title(), st.description(), st.details().html(), mParent);
        errorDialog.exec();
    }
    return st;
}

Status ActionCommands::exportGif()
{
    // exporting gif
    return exportMovie(true);
}

Status ActionCommands::exportMovie(bool isGif)
{
    FileType fileType = (isGif) ? FileType::GIF : FileType::MOVIE;

    int clipCount = mEditor->sound()->soundClipCount();
    if (fileType == FileType::MOVIE && clipCount >= MovieExporter::MAX_SOUND_FRAMES)
    {
        ErrorDialog errorDialog(tr("Something went wrong"), tr("You currently have a total of %1 sound clips. Due to current limitations, you will be unable to export any animation exceeding %2 sound clips. We recommend splitting up larger projects into multiple smaller project to stay within this limit.").arg(clipCount).arg(MovieExporter::MAX_SOUND_FRAMES), QString(), mParent);
        errorDialog.exec();
        return Status::FAIL;
    }

    ExportMovieDialog* dialog = new ExportMovieDialog(mParent, ImportExportDialog::Export, fileType);
    OnScopeExit(dialog->deleteLater());

    dialog->init();

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

    int lengthWithSounds = mEditor->layers()->animationLength(true);
    int length = mEditor->layers()->animationLength(false);

    dialog->setDefaultRange(1, length, lengthWithSounds);
    dialog->exec();

    if (dialog->result() == QDialog::Rejected)
    {
        return Status::SAFE;
    }
    QString strMoviePath = dialog->getFilePath();

    ExportMovieDesc desc;
    desc.strFileName = strMoviePath;
    desc.startFrame = dialog->getStartFrame();
    desc.endFrame = dialog->getEndFrame();
    desc.fps = mEditor->playback()->fps();
    desc.exportSize = dialog->getExportSize();
    desc.strCameraName = dialog->getSelectedCameraName();
    desc.loop = dialog->getLoop();
    desc.alpha = dialog->getTransparency();

    DoubleProgressDialog progressDlg;
    progressDlg.setWindowModality(Qt::WindowModal);
    progressDlg.setWindowTitle(tr("Exporting movie"));
    Qt::WindowFlags eFlags = Qt::Dialog | Qt::WindowTitleHint;
    progressDlg.setWindowFlags(eFlags);
    progressDlg.show();

    MovieExporter ex;

    connect(&progressDlg, &DoubleProgressDialog::canceled, [&ex]
    {
        ex.cancel();
    });

    // The start points and length for the current minor operation segment on the major progress bar
    float minorStart, minorLength;

    Status st = ex.run(mEditor->object(), desc,
        [&progressDlg, &minorStart, &minorLength](float f, float final)
        {
            progressDlg.major->setValue(f);

            minorStart = f;
            minorLength = qMax(0.f, final - minorStart);

            QApplication::processEvents();
        },
        [&progressDlg, &minorStart, &minorLength](float f) {
            progressDlg.minor->setValue(f);

            progressDlg.major->setValue(minorStart + f * minorLength);

            QApplication::processEvents();
        },
        [&progressDlg](QString s) {
            progressDlg.setStatus(s);
            QApplication::processEvents();
        }
    );

    if (st.ok())
    {
        if (QFile::exists(strMoviePath))
        {
            if (isGif) {
                auto btn = QMessageBox::question(mParent, "Pencil2D",
                                                 tr("Finished. Open file location?"));

                if (btn == QMessageBox::Yes)
                {
                    QString path = dialog->getAbsolutePath();
                    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
                }
                return Status::OK;
            }
            auto btn = QMessageBox::question(mParent, "Pencil2D",
                                             tr("Finished. Open movie now?", "When movie export done."));
            if (btn == QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl::fromLocalFile(strMoviePath));
            }
        }
        else
        {
            ErrorDialog errorDialog(tr("Unknown export error"), tr("The export did not produce any errors, however we can't find the output file. Your export may not have completed successfully."), QString(), mParent);
            errorDialog.exec();
        }
    }
    else if(st != Status::CANCELED)
    {
        ErrorDialog errorDialog(st.title(), st.description(), st.details().html(), mParent);
        errorDialog.exec();
    }

    return st;
}

Status ActionCommands::exportImageSequence()
{
    auto dialog = new ExportImageDialog(mParent, FileType::IMAGE_SEQUENCE);
    OnScopeExit(dialog->deleteLater());

    dialog->init();

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

    int lengthWithSounds = mEditor->layers()->animationLength(true);
    int length = mEditor->layers()->animationLength(false);

    dialog->setDefaultRange(1, length, lengthWithSounds);

    dialog->exec();

    if (dialog->result() == QDialog::Rejected)
    {
        return Status::SAFE;
    }

    QString strFilePath = dialog->getFilePath();
    QSize exportSize = dialog->getExportSize();
    QString exportFormat = dialog->getExportFormat();
    bool exportKeyframesOnly = dialog->getExportKeyframesOnly();
    bool useTranparency = dialog->getTransparency();
    int startFrame = dialog->getStartFrame();
    int endFrame  = dialog->getEndFrame();

    QString sCameraLayerName = dialog->getCameraLayerName();
    LayerCamera* cameraLayer = static_cast<LayerCamera*>(mEditor->layers()->findLayerByName(sCameraLayerName, Layer::CAMERA));

    // Show a progress dialog, as this can take a while if you have lots of frames.
    QProgressDialog progress(tr("Exporting image sequence..."), tr("Abort"), 0, 100, mParent);
    hideQuestionMark(progress);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    mEditor->object()->exportFrames(startFrame, endFrame,
                                    cameraLayer,
                                    exportSize,
                                    strFilePath,
                                    exportFormat,
                                    useTranparency,
                                    exportKeyframesOnly,
                                    mEditor->layers()->currentLayer()->name(),
                                    true,
                                    &progress,
                                    100);

    progress.close();

    return Status::OK;
}

Status ActionCommands::exportImage()
{
    // Options
    auto dialog = new ExportImageDialog(mParent, FileType::IMAGE);
    OnScopeExit(dialog->deleteLater())

    dialog->init();

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
    LayerCamera* cameraLayer = static_cast<LayerCamera*>(mEditor->layers()->findLayerByName(sCameraLayerName, Layer::CAMERA));

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

void ActionCommands::selectAll()
{
    mEditor->selectAll();
}

void ActionCommands::deselectAll()
{
    mEditor->deselectAll();
}

void ActionCommands::ZoomIn()
{
    mEditor->view()->scaleUp();
}

void ActionCommands::ZoomOut()
{
    mEditor->view()->scaleDown();
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
    // Sound keyframes should not be empty, so we try to import a sound instead
    if (mEditor->layers()->currentLayer()->type() == Layer::SOUND)
    {
        return importSound(FileType::SOUND);
    }

    KeyFrame* key = mEditor->addNewKey();
    Camera* cam = dynamic_cast<Camera*>(key);
    if (cam)
    {
        mEditor->view()->forceUpdateViewTransform();
    }

    return Status::OK;
}

void ActionCommands::exposeSelectedFrames(int offset)
{
    Layer* currentLayer = mEditor->layers()->currentLayer();

    bool hasSelectedFrames = currentLayer->hasAnySelectedFrames();

    // Functionality to be able to expose the current frame without selecting
    // A:
    KeyFrame* key = currentLayer->getLastKeyFrameAtPosition(mEditor->currentFrame());
    if (!hasSelectedFrames) {

        if (key == nullptr) { return; }
        currentLayer->setFrameSelected(key->pos(), true);
    }

    currentLayer->setExposureForSelectedFrames(offset);
    emit mEditor->updateTimeLine();
    emit mEditor->framesModified();

    // Remember to deselect frame again so we don't show it being visually selected.
    // B:
    if (!hasSelectedFrames) {
        currentLayer->setFrameSelected(key->pos(), false);
    }
}

void ActionCommands::addExposureToSelectedFrames()
{
    exposeSelectedFrames(1);
}

void ActionCommands::subtractExposureFromSelectedFrames()
{
    exposeSelectedFrames(-1);
}

Status ActionCommands::insertKeyFrameAtCurrentPosition()
{
    Layer* currentLayer = mEditor->layers()->currentLayer();
    int currentPosition = mEditor->currentFrame();

    currentLayer->insertExposureAt(currentPosition);
    return addNewKey();
}

void ActionCommands::removeSelectedFrames()
{
    Layer* currentLayer = mEditor->layers()->currentLayer();

    if (!currentLayer->hasAnySelectedFrames()) { return; }

    int ret = QMessageBox::warning(mParent,
                                   tr("Remove selected frames", "Windows title of remove selected frames pop-up."),
                                   tr("Are you sure you want to remove the selected frames? This action is irreversible currently!"),
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Ok);

    if (ret != QMessageBox::Ok)
    {
        return;
    }

    for (int pos : currentLayer->selectedKeyFramesPositions()) {
        currentLayer->removeKeyFrame(pos);
    }
    mEditor->layers()->notifyLayerChanged(currentLayer);
}

void ActionCommands::reverseSelectedFrames()
{
    Layer* currentLayer = mEditor->layers()->currentLayer();

    if (!currentLayer->reverseOrderOfSelection()) {
        return;
    }

    if (currentLayer->type() == Layer::CAMERA) {
        mEditor->view()->forceUpdateViewTransform();
    }
    emit mEditor->framesModified();
};

void ActionCommands::removeKey()
{
    mEditor->removeKey();

    // Add a new keyframe at the beginning if there are none, unless it is a sound layer which can't have empty keyframes but can be an empty layer
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->keyFrameCount() == 0 && layer->type() != Layer::SOUND)
    {
        layer->addNewKeyFrameAt(1);
    }
}

void ActionCommands::duplicateLayer()
{
    LayerManager* layerMgr = mEditor->layers();
    Layer* fromLayer = layerMgr->currentLayer();
    int currFrame = mEditor->currentFrame();

    Layer* toLayer = layerMgr->createLayer(fromLayer->type(), tr("%1 (copy)", "Default duplicate layer name").arg(fromLayer->name()));
    toLayer->removeKeyFrame(1);
    fromLayer->foreachKeyFrame([&] (KeyFrame* key) {
        key = key->clone();
        toLayer->addKeyFrame(key->pos(), key);
        if (toLayer->type() == Layer::SOUND)
        {
            mEditor->sound()->processSound(static_cast<SoundClip*>(key));
        }
        else
        {
            key->modification();
        }
    });
    mEditor->scrubTo(currFrame);
}

void ActionCommands::duplicateKey()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) return;
    if (!layer->visible())
    {
        mEditor->getScribbleArea()->showLayerNotVisibleWarning();
        return;
    }

    KeyFrame* key = layer->getKeyFrameAt(mEditor->currentFrame());
    if (key == nullptr) return;

    KeyFrame* dupKey = key->clone();

    int nextEmptyFrame = mEditor->currentFrame() + 1;
    while (layer->keyExistsWhichCovers(nextEmptyFrame))
    {
        nextEmptyFrame += 1;
    }

    layer->addKeyFrame(nextEmptyFrame, dupKey);
    mEditor->scrubTo(nextEmptyFrame);

    if (layer->type() == Layer::SOUND)
    {
        mEditor->sound()->processSound(dynamic_cast<SoundClip*>(dupKey));
        showSoundClipWarningIfNeeded();
    }
    else
    {
        dupKey->setFileName(""); // don't share filename
        dupKey->modification();
    }

    mEditor->layers()->notifyAnimationLengthChanged();
}

void ActionCommands::moveFrameForward()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer)
    {
        if (layer->moveKeyFrame(mEditor->currentFrame(), 1))
        {
            mEditor->scrubForward();
        }
    }
    mEditor->layers()->notifyAnimationLengthChanged();
    emit mEditor->framesModified();
}

void ActionCommands::moveFrameBackward()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer)
    {
        if (layer->moveKeyFrame(mEditor->currentFrame(), -1))
        {
            mEditor->scrubBackward();
        }
    }
    emit mEditor->framesModified();
}

Status ActionCommands::addNewBitmapLayer()
{
    bool ok;
    QString text = QInputDialog::getText(nullptr, tr("Layer Properties"),
                                         tr("Layer name:"), QLineEdit::Normal,
                                         mEditor->layers()->nameSuggestLayer(tr("Bitmap Layer")), &ok);
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
                                         mEditor->layers()->nameSuggestLayer(tr("Vector Layer")), &ok);
    if (ok && !text.isEmpty())
    {
        mEditor->layers()->createVectorLayer(text);
    }
    return Status::OK;
}

Status ActionCommands::addNewCameraLayer()
{
    bool ok;
    QString text = QInputDialog::getText(nullptr, tr("Layer Properties", "A popup when creating a new layer"),
                                         tr("Layer name:"), QLineEdit::Normal,
                                         mEditor->layers()->nameSuggestLayer(tr("Camera Layer")), &ok);
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
                                                 mEditor->layers()->nameSuggestLayer(tr("Sound Layer")), &ok);
    if (ok && !strLayerName.isEmpty())
    {
        Layer* layer = mEditor->layers()->createSoundLayer(strLayerName);
        mEditor->layers()->setCurrentLayer(layer);
   }
    return Status::OK;
}

Status ActionCommands::deleteCurrentLayer()
{
    LayerManager* layerMgr = mEditor->layers();
    QString strLayerName = layerMgr->currentLayer()->name();

    int ret = QMessageBox::warning(mParent,
                                   tr("Delete Layer", "Windows title of Delete current layer pop-up."),
                                   tr("Are you sure you want to delete layer: %1? This cannot be undone.").arg(strLayerName),
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

void ActionCommands::setLayerVisibilityIndex(int index)
{
    mEditor->setLayerVisibility(static_cast<LayerVisibility>(index));
}

void ActionCommands::changeKeyframeLineColor()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP &&
            mEditor->layers()->currentLayer()->keyExists(mEditor->currentFrame()))
    {
        QRgb color = mEditor->color()->frontColor().rgb();
        LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
        layer->getBitmapImageAtFrame(mEditor->currentFrame())->fillNonAlphaPixels(color);
        mEditor->updateFrame(mEditor->currentFrame());
    }
}

void ActionCommands::changeallKeyframeLineColor()
{
    if (mEditor->layers()->currentLayer()->type() == Layer::BITMAP)
    {
        QRgb color = mEditor->color()->frontColor().rgb();
        LayerBitmap* layer = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
        for (int i = layer->firstKeyFramePosition(); i <= layer->getMaxKeyFramePosition(); i++)
        {
            if (layer->keyExists(i))
                layer->getBitmapImageAtFrame(i)->fillNonAlphaPixels(color);
        }
        mEditor->updateFrame(mEditor->currentFrame());
    }
}

void ActionCommands::help()
{
    QString url = "http://www.pencil2d.org/doc/";
    QDesktopServices::openUrl(QUrl(url));
}

void ActionCommands::quickGuide()
{
    QString sDocPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString sCopyDest = QDir(sDocPath).filePath("pencil2d_quick_guide.pdf");

    QFile quickGuideFile(":/app/pencil2d_quick_guide.pdf");
    quickGuideFile.copy(sCopyDest);

    QDesktopServices::openUrl(QUrl::fromLocalFile(sCopyDest));
}

void ActionCommands::website()
{
    QString url = "https://www.pencil2d.org/";
    QDesktopServices::openUrl(QUrl(url));
}

void ActionCommands::forum()
{
    QString url = "https://discuss.pencil2d.org/";
    QDesktopServices::openUrl(QUrl(url));
}

void ActionCommands::discord()
{
    QString url = "https://discord.gg/8FxdV2g";
    QDesktopServices::openUrl(QUrl(url));
}

void ActionCommands::reportbug()
{
    QString url = "https://github.com/pencil2d/pencil/issues";
    QDesktopServices::openUrl(QUrl(url));
}

void ActionCommands::checkForUpdates()
{
    CheckUpdatesDialog dialog;
    dialog.startChecking();
    dialog.exec();
}

// This action is a temporary measure until we have an automated recover mechanism in place
void ActionCommands::openTemporaryDirectory()
{
    int ret = QMessageBox::warning(mParent, tr("Warning"), tr("The temporary directory is meant to be used only by Pencil2D. Do not modify it unless you know what you are doing."), QMessageBox::Cancel, QMessageBox::Ok);
    if (ret == QMessageBox::Ok)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::temp().filePath("Pencil2D")));
    }
}

void ActionCommands::about()
{
    AboutDialog* aboutBox = new AboutDialog(mParent);
    aboutBox->setAttribute(Qt::WA_DeleteOnClose);
    aboutBox->init();
    aboutBox->exec();
}

void ActionCommands::showSoundClipWarningIfNeeded()
{
    int clipCount = mEditor->sound()->soundClipCount();
    if (clipCount >= MovieExporter::MAX_SOUND_FRAMES && !mSuppressSoundWarning) {
        QMessageBox::warning(mParent, tr("Warning"), tr("You currently have a total of %1 sound clips. Due to current limitations, you will be unable to export any animation exceeding %2 sound clips. We recommend splitting up larger projects into multiple smaller project to stay within this limit.").arg(clipCount).arg(MovieExporter::MAX_SOUND_FRAMES));
        mSuppressSoundWarning = true;
    } else {
        mSuppressSoundWarning = false;
    }
}
