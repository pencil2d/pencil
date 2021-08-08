/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "timelinecells.h"

#include <QApplication>
#include <QResizeEvent>
#include <QInputDialog>
#include <QPainter>
#include <QSettings>
#include <QMenu>
#include <QDebug>

#include "camerapropertiesdialog.h"
#include "editor.h"
#include "keyframe.h"
#include "camera.h"
#include "cameraeasingtype.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "object.h"
#include "playbackmanager.h"
#include "preferencemanager.h"
#include "timeline.h"
#include "toolmanager.h"

TimeLineCells::TimeLineCells(TimeLine* parent, Editor* editor, TIMELINE_CELL_TYPE type) : QWidget(parent)
{
    mTimeLine = parent;
    mEditor = editor;
    mPrefs = editor->preference();
    mType = type;

    mFrameLength = mPrefs->getInt(SETTING::TIMELINE_SIZE);
    mFontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);
    mFrameSize = mPrefs->getInt(SETTING::FRAME_SIZE);
    mbShortScrub = mPrefs->isOn(SETTING::SHORT_SCRUB);
    mDrawFrameNumber = mPrefs->isOn(SETTING::DRAW_LABEL);

    setMinimumSize(500, 4 * mLayerHeight);
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    setAttribute(Qt::WA_OpaquePaintEvent, false);

    connect(mPrefs, &PreferenceManager::optionChanged, this, &TimeLineCells::loadSetting);
}

TimeLineCells::~TimeLineCells()
{
    delete mCache;
}

void TimeLineCells::loadSetting(SETTING setting)
{
    switch (setting)
    {
    case SETTING::TIMELINE_SIZE:
        mFrameLength = mPrefs->getInt(SETTING::TIMELINE_SIZE);
        mTimeLine->updateLength();
        break;
    case SETTING::LABEL_FONT_SIZE:
        mFontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);
        break;
    case SETTING::FRAME_SIZE:
        mFrameSize = mPrefs->getInt(SETTING::FRAME_SIZE);
        mTimeLine->updateLength();
        break;
    case SETTING::SHORT_SCRUB:
        mbShortScrub = mPrefs->isOn(SETTING::SHORT_SCRUB);
        break;
    case SETTING::DRAW_LABEL:
        mDrawFrameNumber = mPrefs->isOn(SETTING::DRAW_LABEL);
        break;
    default:
        break;
    }

    updateContent();
}

int TimeLineCells::getFrameNumber(int x) const
{
    int frameNumber = mFrameOffset + 1 + (x - mOffsetX) / mFrameSize;
    return frameNumber;
}

int TimeLineCells::getFrameX(int frameNumber) const
{
    int x = mOffsetX + (frameNumber - mFrameOffset) * mFrameSize;
    return x;
}

void TimeLineCells::setFrameSize(int size)
{
    mFrameSize = size;
    mPrefs->set(SETTING::FRAME_SIZE, mFrameSize);
    updateContent();
}

int TimeLineCells::getLayerNumber(int y)
{
    int layerNumber = mLayerOffset + (y - mOffsetY) / mLayerHeight;

    int totalLayerCount = mEditor->object()->getLayerCount();

    // Layers numbers are displayed in descending order
    // The last row is layer 0
    if (layerNumber <= totalLayerCount)
        layerNumber = (totalLayerCount - 1) - layerNumber;
    else
        layerNumber = 0;

    if (y < mOffsetY)
    {
        layerNumber = -1;
    }

    if (layerNumber >= totalLayerCount)
    {
        layerNumber = totalLayerCount;
    }

    //If the mouse release event if fired with mouse off the frame of the application
    // mEditor->object()->getLayerCount() doesn't return the correct value.
    if (layerNumber < -1)
    {
        layerNumber = -1;
    }
    return layerNumber;
}

int TimeLineCells::getInbetweenLayerNumber(int y) {
    int layerNumber = getLayerNumber(y);
    // Round the layer number towards the drag start
    if(layerNumber != mFromLayer) {
        if(getMouseMoveY() > 0 && y < getLayerY(layerNumber) + getLayerHeight() / 2) {
            layerNumber++;
        }
        else if(getMouseMoveY() < 0 && y > getLayerY(layerNumber) + getLayerHeight() / 2) {
            layerNumber--;
        }
    }
    return layerNumber;
}

int TimeLineCells::getLayerY(int layerNumber)
{
    return mOffsetY + (mEditor->object()->getLayerCount() - 1 - layerNumber - mLayerOffset)*mLayerHeight;
}

void TimeLineCells::updateFrame(int frameNumber)
{
    int x = getFrameX(frameNumber);
    update(x - mFrameSize, 0, mFrameSize + 1, height());
}

void TimeLineCells::updateContent()
{
    drawContent();
    update();
}

bool TimeLineCells::didDetachLayer() const {
    return abs(getMouseMoveY()) > mLayerDetachThreshold;
}

void TimeLineCells::showCameraMenu(QPoint pos)
{
    int frameNumber = getFrameNumber(pos.x());
    pos = this->mapToGlobal(pos);

    Layer* curLayer = mEditor->layers()->currentLayer();
    Q_ASSERT(curLayer);
    // only show menu if on camera layer and key exists
    if (curLayer->type() != Layer::CAMERA || !curLayer->keyExists(frameNumber))
    {
        return;
    }

    LayerCamera* layer = static_cast<LayerCamera*>(curLayer);
    int nextFrame = curLayer->getNextKeyFramePosition(frameNumber);

    QMenu* cameraMenu = new QMenu();

    QMenu* cameraInterpolationMenu = cameraMenu->addMenu(tr("Camera interpolation frame %1 to %2").arg( QString::number(frameNumber), QString::number(nextFrame)));

    QMenu* subSine  = cameraInterpolationMenu->addMenu(tr("Slow"));
    QMenu* subQuad  = cameraInterpolationMenu->addMenu(tr("Normal"));
    QMenu* subCubic = cameraInterpolationMenu->addMenu(tr("Quick"));
    QMenu* subQuart = cameraInterpolationMenu->addMenu(tr("Fast"));
    QMenu* subQuint = cameraInterpolationMenu->addMenu(tr("Faster"));
    QMenu* subExpo  = cameraInterpolationMenu->addMenu(tr("Fastest"));
    cameraInterpolationMenu->addSeparator();
    QMenu* subCirc  = cameraInterpolationMenu->addMenu(tr("Circle-based"));
    QMenu* subBack  = cameraInterpolationMenu->addMenu(tr("Overshoot"));
    QMenu* subElast  = cameraInterpolationMenu->addMenu(tr("Elastic"));
    QMenu* subBounc  = cameraInterpolationMenu->addMenu(tr("Bounce"));
    cameraInterpolationMenu->addSeparator();
    QMenu* subOther = cameraInterpolationMenu->addMenu(tr("Other"));

    subSine->addAction(tr("Slow Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::INSINE, frameNumber); });
    subSine->addAction(tr("Slow  Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::OUTSINE, frameNumber); });
    subSine->addAction(tr("Slow  Ease-in - Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::INOUTSINE, frameNumber); });
    subSine->addAction(tr("Slow  Ease-out - Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::OUTINSINE, frameNumber); });
    subQuad->addAction(tr("Normal Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::INQUAD, frameNumber); });
    subQuad->addAction(tr("Normal Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::OUTQUAD, frameNumber); });
    subQuad->addAction(tr("Normal Ease-in - Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::INOUTQUAD, frameNumber); });
    subQuad->addAction(tr("Normal Ease-out - Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::OUTINQUAD, frameNumber); });
    subCubic->addAction(tr("Quick Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::INCUBIC, frameNumber); });
    subCubic->addAction(tr("Quick Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::OUTCUBIC, frameNumber); });
    subCubic->addAction(tr("Quick Ease-in - Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::INOUTCUBIC, frameNumber); });
    subCubic->addAction(tr("Quick Ease-out - Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::OUTINCUBIC, frameNumber); });
    subQuart->addAction(tr("Fast Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::INQUART, frameNumber); });
    subQuart->addAction(tr("Fast Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::OUTQUART, frameNumber); });
    subQuart->addAction(tr("Fast Ease-in - Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::INOUTQUART, frameNumber); });
    subQuart->addAction(tr("Fast Ease-out - Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::OUTINQUART, frameNumber); });
    subQuint->addAction(tr("Faster Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::INQUINT, frameNumber); });
    subQuint->addAction(tr("Faster Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::OUTQUINT, frameNumber); });
    subQuint->addAction(tr("Faster Ease-in - Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::INOUTQUINT, frameNumber); });
    subQuint->addAction(tr("Faster Ease-out - Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::OUTINQUINT, frameNumber); });
    subExpo->addAction(tr("Fastest Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::INEXPO, frameNumber); });
    subExpo->addAction(tr("Fastest Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::OUTEXPO, frameNumber); });
    subExpo->addAction(tr("Fastest Ease-in - Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::INOUTEXPO, frameNumber); });
    subExpo->addAction(tr("Fastest Ease-out - Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::OUTINEXPO, frameNumber); });
    subCirc->addAction(tr("Circle-based Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::INCIRC, frameNumber); });
    subCirc->addAction(tr("Circle-based Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::OUTCIRC, frameNumber); });
    subCirc->addAction(tr("Circle-based Ease-in - Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::INOUTCIRC, frameNumber); });
    subCirc->addAction(tr("Circle-based Ease-out - Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::OUTINCIRC, frameNumber); });
    subBack->addAction(tr("Overshoot Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::INBACK, frameNumber); });
    subBack->addAction(tr("Overshoot Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::OUTBACK, frameNumber); });
    subBack->addAction(tr("Overshoot Ease-in - Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::INOUTBACK, frameNumber); });
    subBack->addAction(tr("Overshoot Ease-out - Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::OUTINBACK, frameNumber); });
    subElast->addAction(tr("Elastic Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::INELASTIC, frameNumber); });
    subElast->addAction(tr("Elastic Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::OUTELASTIC, frameNumber); });
    subElast->addAction(tr("Elastic Ease-in - Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::INOUTELASTIC, frameNumber); });
    subElast->addAction(tr("Elastic Ease-out - Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::OUTINELASTIC, frameNumber); });
    subBounc->addAction(tr("Bounce Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::INBOUNCE, frameNumber); });
    subBounc->addAction(tr("Bounce Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::OUTBOUNCE, frameNumber); });
    subBounc->addAction(tr("Bounce Ease-in - Ease-out"), [=] { layer->setCameraEasing(CameraEasingType::INOUTBOUNCE, frameNumber); });
    subBounc->addAction(tr("Bounce Ease-out - Ease-in"), [=] { layer->setCameraEasing(CameraEasingType::OUTINBOUNCE, frameNumber); });
    subOther->addAction(tr("Linear interpolation"), [=] { layer->setCameraEasing(CameraEasingType::LINEAR, frameNumber); });

    QMenu* cameraFieldMenu = cameraMenu->addMenu(tr("Camera field"));
    cameraFieldMenu->addAction(tr("Reset camera field to default"), [=] { layer->setCameraReset(CameraFieldOption::RESET_FIELD, frameNumber); });
    cameraFieldMenu->addSeparator();
    cameraFieldMenu->addAction(tr("Reset camera position to center (0,0)"), [=] { layer->setCameraReset(CameraFieldOption::RESET_TRANSITION, frameNumber); });
    cameraFieldMenu->addAction(tr("Reset camera scaling to 1:1"), [=] { layer->setCameraReset(CameraFieldOption::RESET_SCALING, frameNumber); });
    cameraFieldMenu->addAction(tr("Reset camera rotation to 0"), [=] { layer->setCameraReset(CameraFieldOption::RESET_ROTATION, frameNumber); });
    cameraFieldMenu->addSeparator();
    cameraFieldMenu->addAction(tr("Align keyframe %1 horizontally").arg(QString::number(nextFrame)), [=] { layer->setCameraReset(CameraFieldOption::ALIGN_HORIZONTAL, frameNumber); });
    cameraFieldMenu->addAction(tr("Align keyframe %1 vertically").arg(QString::number(nextFrame)), [=] { layer->setCameraReset(CameraFieldOption::ALIGN_VERTICAL, frameNumber); });
    cameraFieldMenu->addSeparator();
    cameraFieldMenu->addAction(tr("Hold to keyframe %1").arg(QString::number(nextFrame)), [=] { layer->setCameraReset(CameraFieldOption::HOLD_FRAME, frameNumber); });

    cameraMenu->exec(pos);
    mEditor->scrubTo(mEditor->currentFrame());
    updateContent();
}

void TimeLineCells::drawContent()
{
    const QPalette palette = QApplication::palette();

    if (mCache == nullptr)
    {
        mCache = new QPixmap(size());
        if (mCache->isNull())
        {
            // fail to create cache
            return;
        }
    }

    mCurrentFrame = mEditor->currentFrame();

    QPainter painter(mCache);

    const Object* object = mEditor->object();

    Q_ASSERT(object != nullptr);

    const Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) return;

    // grey background of the view
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette.color(QPalette::Base));
    painter.drawRect(QRect(0, 0, width(), height()));

    // --- draw layers of the current object
    for (int i = 0; i < object->getLayerCount(); i++)
    {
        if (i == mEditor->layers()->currentLayerIndex())
        {
            continue;
        }
        Layer* layeri = object->getLayer(i);

        if (layeri != nullptr)
        {
            switch (mType)
            {
            case TIMELINE_CELL_TYPE::Tracks:
                paintTrack(painter, layeri, mOffsetX,
                           getLayerY(i), width() - mOffsetX,
                           getLayerHeight(), false, mFrameSize);
                break;

            case TIMELINE_CELL_TYPE::Layers:
                paintLabel(painter, layeri, 0,
                           getLayerY(i), width() - 1,
                           getLayerHeight(), false, mEditor->layerVisibility());
                break;
            }
        }
    }
    if (didDetachLayer())
    {
        if (mType == TIMELINE_CELL_TYPE::Tracks)
        {
            paintTrack(painter, layer,
                       mOffsetX, getLayerY(mEditor->layers()->currentLayerIndex()) + getMouseMoveY(),
                       width() - mOffsetX, getLayerHeight(),
                       true, mFrameSize);
        }
        else if (mType == TIMELINE_CELL_TYPE::Layers)
        {
            paintLabel(painter, layer,
                       0, getLayerY(mEditor->layers()->currentLayerIndex()) + getMouseMoveY(),
                       width() - 1, getLayerHeight(), true, mEditor->layerVisibility());

            paintLayerGutter(painter);
        }
    }
    else
    {
        if (mType == TIMELINE_CELL_TYPE::Tracks)
        {
            paintTrack(painter,
                       layer,
                       mOffsetX,
                       getLayerY(mEditor->layers()->currentLayerIndex()),
                       width() - mOffsetX,
                       getLayerHeight(),
                       true,
                       mFrameSize);
        }
        else if (mType == TIMELINE_CELL_TYPE::Layers)
        {
            paintLabel(painter,
                       layer,
                       0,
                       getLayerY(mEditor->layers()->currentLayerIndex()),
                       width() - 1,
                       getLayerHeight(),
                       true,
                       mEditor->layerVisibility());
        }
    }

    // --- draw top
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette.color(QPalette::Base));
    painter.drawRect(QRect(0, 0, width() - 1, mOffsetY - 1));
    painter.setPen(palette.color(QPalette::Mid));
    painter.drawLine(0, mOffsetY - 2, width() - 1, mOffsetY - 2);

    if (mType == TIMELINE_CELL_TYPE::Layers)
    {
        // --- draw circle
        painter.setPen(palette.color(QPalette::Text));
        if (mEditor->layerVisibility() == LayerVisibility::CURRENTONLY)
        {
            painter.setBrush(palette.color(QPalette::Base));
        }
        else if (mEditor->layerVisibility() == LayerVisibility::RELATED)
        {
            QColor color = palette.color(QPalette::Text);
            color.setAlpha(128);
            painter.setBrush(color);
        }
        else if (mEditor->layerVisibility() == LayerVisibility::ALL)
        {
            painter.setBrush(palette.brush(QPalette::Text));
        }
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawEllipse(6, 4, 9, 9);
        painter.setRenderHint(QPainter::Antialiasing, false);
    }
    else if (mType == TIMELINE_CELL_TYPE::Tracks)
    {
        // --- draw ticks
        painter.setPen(palette.color(QPalette::Text));
        painter.setBrush(palette.brush(QPalette::Text));
        int fps = mEditor->playback()->fps();
        for (int i = mFrameOffset; i < mFrameOffset + (width() - mOffsetX) / mFrameSize; i++)
        {
            if (i + 1 >= mTimeLine->getRangeLower() && i < mTimeLine->getRangeUpper())
            {
                painter.setPen(Qt::NoPen);
                painter.setBrush(palette.color(QPalette::Highlight));

                painter.drawRect(getFrameX(i), 1, mFrameSize + 1, 2);

                painter.setPen(palette.color(QPalette::Text));
                painter.setBrush(palette.brush(QPalette::Text));
            }

            if (i%fps == 0 || i%fps == fps / 2)
            {
                painter.drawLine(getFrameX(i), 1, getFrameX(i), 5);
            }
            else
            {
                painter.drawLine(getFrameX(i), 1, getFrameX(i), 3);
            }
            if (i == 0 || i % fps == fps - 1)
            {
                int incr = (i < 9) ? 4 : 0; // poor man’s text centering
                painter.drawText(QPoint(getFrameX(i) + incr, 15), QString::number(i + 1));
            }
        }
    }
}

void TimeLineCells::paintTrack(QPainter& painter, const Layer* layer,
                       int x, int y, int width, int height,
                       bool selected, int frameSize) const
{
    const QPalette palette = QApplication::palette();
    QColor col;
    // Color each track according to the layer type
    if (layer->type() == Layer::BITMAP) col = QColor(51, 155, 252);
    if (layer->type() == Layer::VECTOR) col = QColor(70, 205, 123);
    if (layer->type() == Layer::SOUND) col = QColor(255, 141, 112);
    if (layer->type() == Layer::CAMERA) col = QColor(253, 202, 92);
    // Dim invisible layers
    if (!layer->visible()) col.setAlpha(64);

    painter.save();
    painter.setBrush(col);
    painter.setPen(QPen(QBrush(palette.color(QPalette::Mid)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawRect(x, y - 1, width, height);

    if (!layer->visible())
    {
        painter.restore();
        return;
    }

    // Changes the appearance if selected
    if (selected)
    {
        paintSelection(painter, x, y, width, height);
    }
    else
    {
        painter.save();
        QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
        linearGrad.setColorAt(0, QColor(255,255,255,150));
        linearGrad.setColorAt(1, QColor(0,0,0,0));
        painter.setCompositionMode(QPainter::CompositionMode_Overlay);
        painter.setBrush(linearGrad);
        painter.drawRect(x, y - 1, width, height);
        painter.restore();
    }

    paintFrames(painter, layer, col, y, height, selected, frameSize);

    painter.restore();
}

void TimeLineCells::paintFrames(QPainter& painter, const Layer* layer, QColor trackCol, int y, int height, bool selected, int frameSize) const
{
    painter.setPen(QPen(QBrush(QColor(40, 40, 40)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    layer->foreachKeyFrame([&](KeyFrame* key)
    {
        int recLeft = getFrameX(key->pos()) - frameSize + 2;
        int recTop = y + 1;
        int recWidth = frameSize - 2;
        int recHeight = height - 4;

        if (key->length() > 1)
        {
            // This is especially for sound clips.
            // Sound clips are the only type of KeyFrame with variable frame length.
            recWidth = frameSize * key->length() - 2;
        }

        // Paint the frame border
        if (selected && key->pos() == getCurrentFrame()) {
            painter.setPen(Qt::white);
        } else {
            painter.setPen(QPen(QBrush(QColor(40, 40, 40)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        }

        // Paint the frame contents
        if (key->isSelected())
        {
            painter.setBrush(QColor(60, 60, 60));
        }
        else if (selected)
        {
            painter.setBrush(QColor(trackCol.red(), trackCol.green(), trackCol.blue(), 150));
        }
        painter.drawRect(recLeft, recTop, recWidth, recHeight);
    });
}

void TimeLineCells::paintLabel(QPainter& painter, const Layer* layer,
                       int x, int y, int width, int height,
                       bool selected, LayerVisibility layerVisibility) const
{
    const QPalette palette = QApplication::palette();

    if (selected)
    {
        painter.setBrush(palette.color(QPalette::Highlight));
    }
    else
    {
        painter.setBrush(palette.color(QPalette::Base));
    }
    painter.setPen(Qt::NoPen);
    painter.drawRect(x, y - 1, width, height); // empty rectangle by default

    if (!layer->visible())
    {
        painter.setBrush(palette.color(QPalette::Base));
    }
    else
    {
        if ((layerVisibility == LayerVisibility::ALL) || selected)
        {
            painter.setBrush(palette.color(QPalette::Text));
        }
        else if (layerVisibility == LayerVisibility::CURRENTONLY)
        {
            painter.setBrush(palette.color(QPalette::Base));
        }
        else if (layerVisibility == LayerVisibility::RELATED)
        {
            QColor color = palette.color(QPalette::Text);
            color.setAlpha(128);
            painter.setBrush(color);
        }
    }
    if (selected)
    {
        painter.setPen(palette.color(QPalette::HighlightedText));
    }
    else
    {
        painter.setPen(palette.color(QPalette::Text));
    }
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(x + 6, y + 4, 9, 9);
    painter.setRenderHint(QPainter::Antialiasing, false);

    if (layer->type() == Layer::BITMAP) painter.drawPixmap(QPoint(20, y + 2), QPixmap(":/icons/layer-bitmap.png"));
    if (layer->type() == Layer::VECTOR) painter.drawPixmap(QPoint(20, y + 2), QPixmap(":/icons/layer-vector.png"));
    if (layer->type() == Layer::SOUND) painter.drawPixmap(QPoint(21, y + 2), QPixmap(":/icons/layer-sound.png"));
    if (layer->type() == Layer::CAMERA) painter.drawPixmap(QPoint(21, y + 2), QPixmap(":/icons/layer-camera.png"));

    if (selected)
    {
        painter.setPen(palette.color(QPalette::HighlightedText));
    }
    else
    {
        painter.setPen(palette.color(QPalette::Text));
    }
    painter.drawText(QPoint(45, y + (2 * height) / 3), layer->name());
}

void TimeLineCells::paintSelection(QPainter& painter, int x, int y, int width, int height) const
{
    QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
    linearGrad.setColorAt(0, QColor(0, 0, 0, 255));
    linearGrad.setColorAt(1, QColor(255, 255, 255, 0));
    painter.save();
    painter.setCompositionMode(QPainter::CompositionMode_Overlay);
    painter.setBrush(linearGrad);
    painter.setPen(Qt::NoPen);
    painter.drawRect(x, y, width, height - 1);
    painter.restore();
}

void TimeLineCells::paintLayerGutter(QPainter& painter)
{
    painter.setPen(QApplication::palette().color(QPalette::Mid));
    if (getMouseMoveY() > mLayerDetachThreshold)
    {
        painter.drawRect(0, getLayerY(getInbetweenLayerNumber(mEndY))+mLayerHeight, width(), 2);
    }
    else
    {
        painter.drawRect(0, getLayerY(getInbetweenLayerNumber(mEndY)), width(), 2);
    }
}

void TimeLineCells::paintOnionSkin(QPainter& painter)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    int frameNumber = mEditor->currentFrame();

    int prevOnionSkinCount = mEditor->preference()->getInt(SETTING::ONION_PREV_FRAMES_NUM);
    int nextOnionSkinCount = mEditor->preference()->getInt(SETTING::ONION_NEXT_FRAMES_NUM);

    bool isAbsolute = (mEditor->preference()->getString(SETTING::ONION_TYPE) == "absolute");

    if (mEditor->preference()->isOn(SETTING::PREV_ONION) && prevOnionSkinCount > 0)
    {
        int onionFrameNumber = frameNumber;
        if (isAbsolute)
        {
            onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber+1, true);
        }
        onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber, isAbsolute);
        int onionPosition = 0;

        while (onionPosition < prevOnionSkinCount && onionFrameNumber > 0)
        {
            painter.setBrush(QColor(128, 128, 128, 128));
            painter.setPen(Qt::NoPen);
            QRect onionRect;
            onionRect.setTopLeft(QPoint(getFrameX(onionFrameNumber - 1), 0));
            onionRect.setBottomRight(QPoint(getFrameX(onionFrameNumber), height()));
            onionRect.setBottomRight(QPoint(getFrameX(onionFrameNumber), 19));
            painter.drawRect(onionRect);

            onionFrameNumber = layer->getPreviousFrameNumber(onionFrameNumber, isAbsolute);
            onionPosition++;
        }
    }

    if (mEditor->preference()->isOn(SETTING::NEXT_ONION) && nextOnionSkinCount > 0) {

        int onionFrameNumber = layer->getNextFrameNumber(frameNumber, isAbsolute);
        int onionPosition = 0;

        while (onionPosition < nextOnionSkinCount && onionFrameNumber > 0)
        {
            painter.setBrush(QColor(128, 128, 128, 128));
            painter.setPen(Qt::NoPen);
            QRect onionRect;
            onionRect.setTopLeft(QPoint(getFrameX(onionFrameNumber - 1), 0));
            onionRect.setBottomRight(QPoint(getFrameX(onionFrameNumber), height()));
            onionRect.setBottomRight(QPoint(getFrameX(onionFrameNumber), 19));
            painter.drawRect(onionRect);

            onionFrameNumber = layer->getNextFrameNumber(onionFrameNumber, isAbsolute);
            onionPosition++;
        }
    }
}

void TimeLineCells::paintEvent(QPaintEvent*)
{
    Object* object = mEditor->object();
    Layer* layer = mEditor->layers()->currentLayer();

    Q_ASSUME(object != nullptr && layer != nullptr);

    const QPalette palette = QApplication::palette();
    QPainter painter(this);

    bool isPlaying = mEditor->playback()->isPlaying();
    if ((!isPlaying && !mTimeLine->scrubbing) || mCache == nullptr)
    {
        drawContent();
    }
    if (mCache)
    {
        painter.drawPixmap(QPoint(0, 0), *mCache);
    }

    if (mType == TIMELINE_CELL_TYPE::Tracks)
    {
        if (!isPlaying)
        {
            paintOnionSkin(painter);
        }

        if (mPrevFrame != mEditor->currentFrame()  || mEditor->playback()->isPlaying())
        {
            mPrevFrame = mEditor->currentFrame();
            trackScrubber();
        }

        // --- draw the position of the current frame
        if (mEditor->currentFrame() > mFrameOffset)
        {
            QColor scrubColor = palette.color(QPalette::Highlight);
            scrubColor.setAlpha(160);
            painter.setBrush(scrubColor);
            painter.setPen(Qt::NoPen);
            //painter.setCompositionMode(QPainter::CompositionMode_Source); // this causes the message: QPainter::setCompositionMode: PorterDuff modes not supported on device
            QRect scrubRect;
            scrubRect.setTopLeft(QPoint(getFrameX(mEditor->currentFrame() - 1), 0));
            scrubRect.setBottomRight(QPoint(getFrameX(mEditor->currentFrame()), height()));
            if (mbShortScrub)
            {
                scrubRect.setBottomRight(QPoint(getFrameX(mEditor->currentFrame()), 19));
            }
            painter.drawRect(scrubRect);
            painter.setPen(palette.color(QPalette::HighlightedText));
            int incr = (mEditor->currentFrame() < 10) ? 4 : 0;
            painter.drawText(QPoint(getFrameX(mEditor->currentFrame() - 1) + incr, 15),
                             QString::number(mEditor->currentFrame()));
        }
    }
}

void TimeLineCells::resizeEvent(QResizeEvent* event)
{
    clearCache();
    updateContent();
    event->accept();
    emit lengthChanged(getFrameLength());
}

void TimeLineCells::mousePressEvent(QMouseEvent* event)
{
    int frameNumber = getFrameNumber(event->pos().x());
    int layerNumber = getLayerNumber(event->pos().y());
    mFromLayer = mToLayer = layerNumber;

    mStartY = event->pos().y();
    mStartLayerNumber = layerNumber;
    mEndY = event->pos().y();

    mStartFrameNumber = frameNumber;
    mLastFrameNumber = mStartFrameNumber;

    mCanMoveFrame = false;
    mMovingFrames = false;

    mCanBoxSelect = false;
    mBoxSelecting = false;

    mClickSelecting = false;

    primaryButton = event->button();

    bool switchLayer = mEditor->tools()->currentTool()->switchingLayer();
    if (!switchLayer) { return; }

    switch (mType)
    {
    case TIMELINE_CELL_TYPE::Layers:
        if (layerNumber != -1 && layerNumber < mEditor->object()->getLayerCount())
        {
            if (event->pos().x() < 15)
            {
                mEditor->switchVisibilityOfLayer(layerNumber);
            }
            else
            {
                mEditor->layers()->setCurrentLayer(layerNumber);
            }
        }
        if (layerNumber == -1)
        {
            if (event->pos().x() < 15)
            {
                if (event->button() == Qt::LeftButton) {
                    mEditor->increaseLayerVisibilityIndex();
                } else if (event->button() == Qt::RightButton) {
                    mEditor->decreaseLayerVisibilityIndex();
                }
            }
        }
        break;
    case TIMELINE_CELL_TYPE::Tracks:
        if (event->button() == Qt::MidButton)
        {
            mLastFrameNumber = getFrameNumber(event->pos().x());
        }
        else
        {
            if (frameNumber == mEditor->currentFrame() && (!mbShortScrub || (mbShortScrub && mStartY < 20)))
            {
                if (mEditor->playback()->isPlaying())
                {
                    mEditor->playback()->stop();
                }
                mTimeLine->scrubbing = true;
            }
            else
            {
                if ((layerNumber != -1) && layerNumber < mEditor->object()->getLayerCount())
                {
                    int previousLayerNumber = mEditor->layers()->currentLayerIndex();

                    if (previousLayerNumber != layerNumber)
                    {
                        Layer *previousLayer = mEditor->object()->getLayer(previousLayerNumber);
                        previousLayer->deselectAll();
                        mEditor->selectedFramesChanged();
                        mEditor->layers()->setCurrentLayer(layerNumber);
                    }

                    Layer *currentLayer = mEditor->object()->getLayer(layerNumber);

                    // Check if we are using the alt key
                    if (event->modifiers() == Qt::AltModifier)
                    {
                        // If it is the case, we select everything that is after the selected frame
                        mClickSelecting = true;
                        mCanMoveFrame = true;

                        currentLayer->selectAllFramesAfter(frameNumber);
                        mEditor->selectedFramesChanged();
                    }
                    // Check if we are clicking on a non selected frame
                    else if (!currentLayer->isFrameSelected(frameNumber))
                    {
                        // If it is the case, we select it if it is the left button...
                        mCanBoxSelect = true;
                        mClickSelecting = true;
                        if (event->button() == Qt::LeftButton)
                        {

                            if (event->modifiers() == Qt::ControlModifier)
                            {
                                // Add/remove from already selected
                                currentLayer->toggleFrameSelected(frameNumber, true);
                                mEditor->selectedFramesChanged();
                            }
                            else if (event->modifiers() == Qt::ShiftModifier)
                            {
                                // Select a range from the last selected
                                currentLayer->extendSelectionTo(frameNumber);
                                mEditor->selectedFramesChanged();
                            }
                            else if (event->button() == Qt::LeftButton)
                            {
                                // Only select if left button clicked
                                currentLayer->toggleFrameSelected(frameNumber, false);
                            }
                        }

                        // ... or we show the camera context menu, if it is the right button
                        if (event->button() == Qt::RightButton)
                        {
                            showCameraMenu(event->pos());
                        }

                    }
                    else
                    {
                        // If selected they can also be interpolated
                        if (event->button() == Qt::RightButton)
                        {
                            showCameraMenu(event->pos());
                        }
                        // We clicked on a selected frame, we can move it
                        mCanMoveFrame = true;
                    }

                    mTimeLine->updateContent();
                }
                else
                {
                    if (frameNumber > 0)
                    {
                        if (mEditor->playback()->isPlaying())
                        {
                            mEditor->playback()->stop();
                        }
                        if (mEditor->playback()->getSoundScrubActive() && mLastScrubFrame != frameNumber)
                        {
                            mEditor->playback()->playScrub(frameNumber);
                            mLastScrubFrame = frameNumber;
                        }

                        mEditor->scrubTo(frameNumber);

                        mTimeLine->scrubbing = true;
                        qDebug("Scrub to %d frame", frameNumber);
                    }
                }
            }
        }
        break;
    }
}

void TimeLineCells::mouseMoveEvent(QMouseEvent* event)
{
    if (mType == TIMELINE_CELL_TYPE::Layers)
    {
        mEndY = event->pos().y();
        emit mouseMovedY(mEndY - mStartY);
    }
    else if (mType == TIMELINE_CELL_TYPE::Tracks)
    {
        int frameNumber = getFrameNumber(event->pos().x());
        if (primaryButton == Qt::MidButton)
        {
            // qMin( max_frame_offset, qMax ( min_frame_offset, draw_frame_offset ) )
            mFrameOffset = qMin(qMax(0, mFrameLength - width() / getFrameSize()), qMax(0, mFrameOffset + mLastFrameNumber - frameNumber));
            update();
            emit offsetChanged(mFrameOffset);
        }
        else
        {
            if (mTimeLine->scrubbing)
            {
                if (mEditor->playback()->getSoundScrubActive() && mLastScrubFrame != frameNumber)
                {
                    mEditor->playback()->playScrub(frameNumber);
                    mLastScrubFrame = frameNumber;
                }
                mEditor->scrubTo(frameNumber);
            }
            else
            {
                if (mStartLayerNumber != -1 && mStartLayerNumber < mEditor->object()->getLayerCount())
                {
                    Layer *currentLayer = mEditor->object()->getLayer(mStartLayerNumber);

                    // Did we move to another frame ?
                    if (frameNumber != mLastFrameNumber)
                    {
                        // Check if the frame we clicked was selected
                        if (mCanMoveFrame) {

                            // If it is the case, we move the selected frames in the layer
                            mMovingFrames = true;

                            int offset = frameNumber - mLastFrameNumber;
                            currentLayer->moveSelectedFrames(offset);
                            mEditor->layers()->notifyAnimationLengthChanged();
                            mEditor->framesModified();
                        }
                        else if (mCanBoxSelect)
                        {
                            // Otherwise, we do a box select
                            mBoxSelecting = true;

                            currentLayer->deselectAll();
                            currentLayer->setFrameSelected(mStartFrameNumber, true);
                            currentLayer->extendSelectionTo(frameNumber);
                            mEditor->selectedFramesChanged();
                        }
                        mLastFrameNumber = frameNumber;
                    }
                }
            }
        }
    }
    mTimeLine->update();
}

void TimeLineCells::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != primaryButton) return;

    primaryButton = Qt::NoButton;
    mEndY = mStartY;
    mTimeLine->scrubbing = false;
    int frameNumber = getFrameNumber(event->pos().x());
    if (frameNumber < 1) frameNumber = -1;
    int layerNumber = getLayerNumber(event->pos().y());
    if (mType == TIMELINE_CELL_TYPE::Tracks && primaryButton != Qt::MidButton && layerNumber != -1 && layerNumber < mEditor->object()->getLayerCount())
    {
        Layer *currentLayer = mEditor->object()->getLayer(layerNumber);

        if (!mTimeLine->scrubbing && !mMovingFrames && !mClickSelecting && !mBoxSelecting)
        {
            // De-selecting if we didn't move, scrub nor select anything
            bool multipleSelection = (event->modifiers() == Qt::ControlModifier);

            // Add/remove from already selected
            currentLayer->toggleFrameSelected(frameNumber, multipleSelection);
            mEditor->selectedFramesChanged();
        }
    }
    if (mType == TIMELINE_CELL_TYPE::Layers && layerNumber != mStartLayerNumber && mStartLayerNumber != -1 && layerNumber != -1)
    {
        mToLayer = getInbetweenLayerNumber(event->pos().y());
        if (mToLayer != mFromLayer && mToLayer > -1 && mToLayer < mEditor->layers()->count())
        {
            // Bubble the from layer up or down to the to layer
            if (mToLayer < mFromLayer) // bubble up
            {
                for (int i = mFromLayer - 1; i >= mToLayer; i--)
                    mEditor->swapLayers(i, i + 1);
            }
            else // bubble down
            {
                for (int i = mFromLayer + 1; i <= mToLayer; i++)
                    mEditor->swapLayers(i, i - 1);
            }
        }
    }

    emit mouseMovedY(0);
    mTimeLine->updateContent();
}

void TimeLineCells::mouseDoubleClickEvent(QMouseEvent* event)
{
    int layerNumber = getLayerNumber(event->pos().y());

    // -- short scrub --
    if (event->pos().y() < 20 && (mType != TIMELINE_CELL_TYPE::Layers || event->pos().x() >= 15))
    {
        mPrefs->set(SETTING::SHORT_SCRUB, !mbShortScrub);
    }

    // -- layer --
    Layer* layer = mEditor->object()->getLayer(layerNumber);
    if (layer && mType == TIMELINE_CELL_TYPE::Layers && event->pos().x() >= 15)
    {
        editLayerProperties(layer);
    }
    QWidget::mouseDoubleClickEvent(event);
}

void TimeLineCells::editLayerProperties(Layer *layer) const
{
    if (layer->type() != Layer::CAMERA)
    {
        editLayerName(layer);
        return;
    }

    auto cameraLayer = dynamic_cast<LayerCamera*>(layer);
    Q_ASSERT(cameraLayer);
    editLayerProperties(cameraLayer);
}

void TimeLineCells::editLayerProperties(LayerCamera* cameraLayer) const
{
    QRegExp regex("([\\xFFEF-\\xFFFF])+");

    CameraPropertiesDialog dialog(cameraLayer->name(), cameraLayer->getViewRect().width(),
                                  cameraLayer->getViewRect().height());
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }
    QString name = dialog.getName().replace(regex, "");

    if (!name.isEmpty())
    {
        mEditor->layers()->renameLayer(cameraLayer, name);
    }
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_FIELD_W, dialog.getWidth());
    settings.setValue(SETTING_FIELD_H, dialog.getHeight());
    cameraLayer->setViewRect(QRect(-dialog.getWidth() / 2, -dialog.getHeight() / 2, dialog.getWidth(), dialog.getHeight()));
    mEditor->view()->forceUpdateViewTransform();
}

void TimeLineCells::editLayerName(Layer* layer) const
{
    QRegExp regex("([\\xFFEF-\\xFFFF])+");

    bool ok;
    QString name = QInputDialog::getText(nullptr, tr("Layer Properties"),
                                         tr("Layer name:"), QLineEdit::Normal,
                                         layer->name(), &ok);
    name.replace(regex, "");
    if (!ok || name.isEmpty())
    {
        return;
    }

    mEditor->layers()->renameLayer(layer, name);
}

void TimeLineCells::hScrollChange(int x)
{
    mFrameOffset = x;
    update();
}

void TimeLineCells::vScrollChange(int x)
{
    mLayerOffset = x;
    update();
}

void TimeLineCells::setMouseMoveY(int x)
{
    mMouseMoveY = x;
    if (x == 0)
    {
        update();
    }
}

void TimeLineCells::trackScrubber()
{
    if (mEditor->currentFrame() <= mFrameOffset)
    {
        // Move the timeline back if the scrubber is offscreen to the left
        mFrameOffset = mEditor->currentFrame() - 1;
        emit offsetChanged(mFrameOffset);
        mTimeLine->updateContent();
    }
    else if (width() < (mEditor->currentFrame() - mFrameOffset + 1) * mFrameSize)
    {
        // Move timeline forward if the scrubber is offscreen to the right
        if (mEditor->playback()->isPlaying())
            mFrameOffset = mFrameOffset + ((mEditor->currentFrame() - mFrameOffset) / 2);
        else
            mFrameOffset = mEditor->currentFrame() - width() / mFrameSize;
        emit offsetChanged(mFrameOffset);
        mTimeLine->updateContent();
    }
}
