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
    setMouseTracking(true);

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

void TimeLineCells::setHold(int frame)
{
    LayerCamera* curLayer = static_cast<LayerCamera*>(mEditor->layers()->currentLayer());
    QList<int> frames = curLayer->getSelectedFramesByPos();
    if (!frames.empty())
    {
        for (int pos:frames)
        {
            Camera* cam = curLayer->getCameraAtFrame(pos);
            Camera* next = curLayer->getCameraAtFrame(curLayer->getNextKeyFramePosition(pos));
            next->assign(*cam);
            cam->setEasingType(CameraEasingType::LINEAR);
        }
    }
    else
    {
        Camera* cam = curLayer->getCameraAtFrame(frame);
        Camera* next = curLayer->getCameraAtFrame(curLayer->getNextKeyFramePosition(frame));
        next->assign(*cam);
        cam->setEasingType(CameraEasingType::LINEAR);
    }
    updateContent();
}

void TimeLineCells::setCameraEasing(CameraEasingType type, int frame)
{
    LayerCamera* layer = static_cast<LayerCamera*>(mEditor->layers()->currentLayer());
    QList<int> frames = layer->getSelectedFramesByPos();
    if (!frames.empty())
    {
        for (int pos:frames)
        {
            Camera* cam = layer->getCameraAtFrame(pos);
            cam->setEasingType(type);
        }
    }
    else
    {
        Camera* cam = layer->getCameraAtFrame(frame);
        cam->setEasingType(type);
    }
}

int TimeLineCells::getFrameNumber(int x) const
{
    return mFrameOffset + 1 + (x - mOffsetX) / mFrameSize;
}

int TimeLineCells::getFrameX(int frameNumber) const
{
    return mOffsetX + (frameNumber - mFrameOffset) * mFrameSize;
}

void TimeLineCells::setFrameSize(int size)
{
    mFrameSize = size;
    mPrefs->set(SETTING::FRAME_SIZE, mFrameSize);
    updateContent();
}

int TimeLineCells::getLayerNumber(int y) const
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

int TimeLineCells::getInbetweenLayerNumber(int y) const {
    int layerNumber = getLayerNumber(y);
    // Round the layer number towards the drag start
    if(layerNumber != mFromLayer) {
        if(mMouseMoveY > 0 && y < getLayerY(layerNumber) + mLayerHeight / 2) {
            layerNumber++;
        }
        else if(mMouseMoveY < 0 && y > getLayerY(layerNumber) + mLayerHeight / 2) {
            layerNumber--;
        }
    }
    return layerNumber;
}

int TimeLineCells::getLayerY(int layerNumber) const
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
    return abs(mMouseMoveY) > mLayerDetachThreshold;
}

void TimeLineCells::showCameraMenu(QPoint pos)
{
    int frameNumber = getFrameNumber(pos.x());
    pos = this->mapToGlobal(pos);

    Layer* curLayer = mEditor->layers()->currentLayer();
    Q_ASSERT(curLayer);
    // only show menu if on camera layer
    if (curLayer->type() != Layer::CAMERA)
    {
        return;
    }

    int nextFrame = curLayer->getNextKeyFramePosition(frameNumber);

    const QString interpolateFrom = tr("Interpolate frames from: %1 to %2");
    const QString clearMovementDesc = tr("Clear interpolation on: %1");

    if (mEasingMenu == nullptr)
    {
        mEasingMenu = new QMenu(this);

        mInterpolationMenu = mEasingMenu->addMenu(interpolateFrom.arg(frameNumber).arg(nextFrame));

        QMenu* subSine  = mInterpolationMenu->addMenu(tr("Slow"));
        QMenu* subQuad  = mInterpolationMenu->addMenu(tr("Normal"));
        QMenu* subCubic = mInterpolationMenu->addMenu(tr("Quick"));
        QMenu* subQuart = mInterpolationMenu->addMenu(tr("Fast"));
        QMenu* subQuint = mInterpolationMenu->addMenu(tr("Faster"));
        QMenu* subExpo  = mInterpolationMenu->addMenu(tr("Fastest"));

        mInterpolationMenu->addSeparator();
        QMenu* subCirc  = mInterpolationMenu->addMenu(tr("Circle-based"));
        QMenu* subOther = mInterpolationMenu->addMenu(tr("Other"));

        subSine->addAction(tr("Slow Ease-in"), [=] { this->setCameraEasing(CameraEasingType::INSINE, frameNumber); });
        subSine->addAction(tr("Slow Ease-out"), [=] { this->setCameraEasing(CameraEasingType::OUTSINE, frameNumber); });
        subSine->addAction(tr("Slow Ease-in - Ease-out"), [=] { this->setCameraEasing(CameraEasingType::INOUTSINE, frameNumber); });
        subSine->addAction(tr("Slow Ease-out - Ease-in"), [=] { this->setCameraEasing(CameraEasingType::OUTINSINE, frameNumber); });
        subQuad->addAction(tr("Normal Ease-in"), [=] { this->setCameraEasing(CameraEasingType::INQUAD, frameNumber); });
        subQuad->addAction(tr("Normal Ease-out"), [=] { this->setCameraEasing(CameraEasingType::OUTQUAD, frameNumber); });
        subQuad->addAction(tr("Normal Ease-in - Ease-out"), [=] { this->setCameraEasing(CameraEasingType::INOUTQUAD, frameNumber); });
        subQuad->addAction(tr("Normal Ease-out - Ease-in"), [=] { this->setCameraEasing(CameraEasingType::OUTINQUAD, frameNumber); });
        subCubic->addAction(tr("Quick Ease-in"), [=] { this->setCameraEasing(CameraEasingType::INCUBIC, frameNumber); });
        subCubic->addAction(tr("Quick Ease-out"), [=] { this->setCameraEasing(CameraEasingType::OUTCUBIC, frameNumber); });
        subCubic->addAction(tr("Quick Ease-in - Ease-out"), [=] { this->setCameraEasing(CameraEasingType::INOUTCUBIC, frameNumber); });
        subCubic->addAction(tr("Quick Ease-out - Ease-in"), [=] { this->setCameraEasing(CameraEasingType::OUTINCUBIC, frameNumber); });
        subQuart->addAction(tr("Fast Ease-in"), [=] { this->setCameraEasing(CameraEasingType::INQUART, frameNumber); });
        subQuart->addAction(tr("Fast Ease-out"), [=] { this->setCameraEasing(CameraEasingType::OUTQUART, frameNumber); });
        subQuart->addAction(tr("Fast Ease-in - Ease-out"), [=] { this->setCameraEasing(CameraEasingType::INOUTQUART, frameNumber); });
        subQuart->addAction(tr("Fast Ease-out - Ease-in"), [=] { this->setCameraEasing(CameraEasingType::OUTINQUART, frameNumber); });
        subQuint->addAction(tr("Faster Ease-in"), [=] { this->setCameraEasing(CameraEasingType::INQUINT, frameNumber); });
        subQuint->addAction(tr("Faster Ease-out"), [=] { this->setCameraEasing(CameraEasingType::OUTQUINT, frameNumber); });
        subQuint->addAction(tr("Faster Ease-in - Ease-out"), [=] { this->setCameraEasing(CameraEasingType::INOUTQUINT, frameNumber); });
        subQuint->addAction(tr("Faster Ease-out - Ease-in"), [=] { this->setCameraEasing(CameraEasingType::OUTINQUINT, frameNumber); });
        subExpo->addAction(tr("Fastest Ease-in"), [=] { this->setCameraEasing(CameraEasingType::INEXPO, frameNumber); });
        subExpo->addAction(tr("Fastest Ease-out"), [=] { this->setCameraEasing(CameraEasingType::OUTEXPO, frameNumber); });
        subExpo->addAction(tr("Fastest Ease-in - Ease-out"), [=] { this->setCameraEasing(CameraEasingType::INOUTEXPO, frameNumber); });
        subExpo->addAction(tr("Fastest Ease-out - Ease-in"), [=] { this->setCameraEasing(CameraEasingType::OUTINEXPO, frameNumber); });
        subCirc->addAction(tr("Circle-based Ease-in"), [=] { this->setCameraEasing(CameraEasingType::INCIRC, frameNumber); });
        subCirc->addAction(tr("Circle-based Ease-out"), [=] { this->setCameraEasing(CameraEasingType::OUTCIRC, frameNumber); });
        subCirc->addAction(tr("Circle-based Ease-in - Ease-out"), [=] { this->setCameraEasing(CameraEasingType::INOUTCIRC, frameNumber); });
        subCirc->addAction(tr("Circle-based Ease-out - Ease-in"), [=] { this->setCameraEasing(CameraEasingType::OUTINCIRC, frameNumber); });
        mHoldAction = subOther->addAction(tr("Hold to frame %1").arg(nextFrame), [=] { this->setHold(frameNumber); });
        subOther->addAction(tr("Linear interpolation"), [=] { this->setCameraEasing(CameraEasingType::LINEAR, frameNumber); });
    }

    if (curLayer->getSelectedFramesByPos().empty() && !curLayer->keyExists(frameNumber)) {
        return;
    }

    if (curLayer->getSelectedFramesByPos().size() > 1)
    {
        QList<int> frameList = curLayer->getSelectedFramesByPos();
        QString keyNumbers = "";
        for (int pos:frameList)
        {
            keyNumbers += " " + QString::number(pos) + ",";
        }
        // Remove last comma
        keyNumbers.chop(1);
        mInterpolationMenu->setTitle(tr("Interpolate frames at: %1").arg(keyNumbers));
        mHoldAction->setText(clearMovementDesc.arg(keyNumbers));
    }
    else if(curLayer->keyExists(frameNumber))
    {
        QString keyPosString = QString::number(nextFrame);
        if (frameNumber == nextFrame) {
            keyPosString = "-";
        }
        mInterpolationMenu->setTitle(interpolateFrom.arg(frameNumber).arg(keyPosString));
        mHoldAction->setText(clearMovementDesc.arg(nextFrame));
    }

    mEasingMenu->popup(pos);
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

    QPainter painter(mCache);

    const Object* object = mEditor->object();

    Q_ASSERT(object != nullptr);

    const Layer* currentLayer = mEditor->layers()->currentLayer();
    if (currentLayer == nullptr) return;

    // grey background of the view
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette.color(QPalette::Base));
    painter.drawRect(QRect(0, 0, width(), height()));

    const int widgetWidth = width();
    const int layerHeight = mLayerHeight;

    // --- draw layers of the current object
    for (int i = 0; i < object->getLayerCount(); i++)
    {
        if (i == mEditor->layers()->currentLayerIndex())
        {
            continue;
        }
        const Layer* layeri = object->getLayer(i);

        if (layeri != nullptr)
        {
            const int layerY = getLayerY(i);
            switch (mType)
            {
            case TIMELINE_CELL_TYPE::Tracks:
                paintTrack(painter, layeri, mOffsetX,
                           layerY, widgetWidth - mOffsetX,
                           layerHeight, false, mFrameSize);
                break;

            case TIMELINE_CELL_TYPE::Layers:
                paintLabel(painter, layeri, 0,
                           layerY, widgetWidth - 1,
                           layerHeight, false, mEditor->layerVisibility());
                break;
            }
        }
    }

    if (didDetachLayer())
    {
        int layerYMouseMove = getLayerY(mEditor->layers()->currentLayerIndex()) + mMouseMoveY;
        if (mType == TIMELINE_CELL_TYPE::Tracks)
        {
            paintTrack(painter, currentLayer,
                       mOffsetX, layerYMouseMove,
                       widgetWidth - mOffsetX, layerHeight,
                       true, mFrameSize);
        }
        else if (mType == TIMELINE_CELL_TYPE::Layers)
        {
            paintLabel(painter, currentLayer,
                       0, layerYMouseMove,
                       widgetWidth - 1, layerHeight, true, mEditor->layerVisibility());

            paintLayerGutter(painter);
        }
    }
    else
    {
        if (mType == TIMELINE_CELL_TYPE::Tracks)
        {
            paintTrack(painter,
                       currentLayer,
                       mOffsetX,
                       getLayerY(mEditor->layers()->currentLayerIndex()),
                       widgetWidth - mOffsetX,
                       layerHeight,
                       true,
                       mFrameSize);
        }
        else if (mType == TIMELINE_CELL_TYPE::Layers)
        {
            paintLabel(painter,
                       currentLayer,
                       0,
                       getLayerY(mEditor->layers()->currentLayerIndex()),
                       widgetWidth - 1,
                       layerHeight,
                       true,
                       mEditor->layerVisibility());
        }
    }

    // --- draw track bar background
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette.color(QPalette::Base));
    painter.drawRect(QRect(0, 0, width() - 1, mOffsetY - 1));

    // --- draw bottom line splitter for track bar
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
        paintTicks(painter, palette);

        const auto object = mEditor->object();
        for (int i = 0; i < object->getLayerCount(); i++) {
            paintSelectedFrames(painter, object->getLayer(i), i);
        }
    }
}

void TimeLineCells::paintTicks(QPainter& painter, const QPalette& palette) const
{
    painter.setPen(palette.color(QPalette::Text));
    painter.setBrush(palette.brush(QPalette::Text));
    int fps = mEditor->playback()->fps();
    for (int i = mFrameOffset; i < mFrameOffset + (width() - mOffsetX) / mFrameSize; i++)
    {
        // line x pos + some offset
        const int lineX = getFrameX(i) + 1;
        if (i + 1 >= mTimeLine->getRangeLower() && i < mTimeLine->getRangeUpper())
        {
            painter.setPen(Qt::NoPen);
            painter.setBrush(palette.color(QPalette::Highlight));

            painter.drawRect(lineX, 1, mFrameSize + 1, 2);

            painter.setPen(palette.color(QPalette::Text));
            painter.setBrush(palette.brush(QPalette::Text));
        }

        // Draw large tick at fps mark
        if (i % fps == 0 || i % fps == fps / 2)
        {
            painter.drawLine(lineX, 1, lineX, 5);
        }
        else // draw small tick
        {
            painter.drawLine(lineX, 1, lineX, 3);
        }
        if (i == 0 || i % fps == fps - 1)
        {
            int incr = (i < 9) ? 4 : 0; // poor man’s text centering
            painter.drawText(QPoint(lineX + incr, 15), QString::number(i + 1));
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

    paintFrames(painter, col, layer, y, height, selected, frameSize);

    painter.restore();
}

void TimeLineCells::paintFrames(QPainter& painter, QColor trackCol, const Layer* layer, int y, int height, bool selected, int frameSize) const
{
    painter.setPen(QPen(QBrush(QColor(40, 40, 40)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    int recTop = y + 1;
    int standardWidth = frameSize - 2;

    int recHeight = height - 4;
    layer->foreachKeyFrame([&](KeyFrame* key)
    {
        int framePos = key->pos();
        int recWidth = standardWidth;
        int recLeft = getFrameX(framePos) - recWidth;

        if (key->length() > 1)
        {
            // This is especially for sound clips.
            // Sound clips are the only type of KeyFrame with variable frame length.
            recWidth = frameSize * key->length();
        }

        // Paint the frame border
        if (selected && framePos == mEditor->currentFrame()) {
            painter.setPen(Qt::white);
        } else {
            painter.setPen(QPen(QBrush(QColor(40, 40, 40)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        }

        // Paint the frame contents
        if (selected)
        {
            if (key->isSelected()) {
                painter.setBrush(QColor(60, 60, 60));
            }
            else
            {
                painter.setBrush(QColor(trackCol.red(), trackCol.green(), trackCol.blue(), 150));
            }
        }

        if (!key->isSelected()) {
            painter.drawRect(recLeft, recTop, recWidth, recHeight);
        }
    });

    if (!mMovingFrames && selected && mLayerPosMoveY != -1 && mLayerPosMoveY == mEditor->currentLayerIndex()) {
        paintFrameCursorOnCurrentLayer(painter, recTop, standardWidth, recHeight);
    }
}

void TimeLineCells::paintFrameCursorOnCurrentLayer(QPainter &painter, int recTop, int recWidth, int recHeight) const
{
    int recLeft = getFrameX(mFramePosMoveX) - recWidth;

    painter.save();
    const QPalette palette = QApplication::palette();
    // Don't fill
    painter.setBrush(Qt::NoBrush);
    // paint border
    QColor penColor = palette.color(QPalette::WindowText);
    penColor.setAlpha(127);
    painter.setPen(penColor);
    painter.drawRect(recLeft, recTop, recWidth, recHeight);
    painter.restore();
}

void TimeLineCells::paintSelectedFrames(QPainter& painter, const Layer* layer, const int layerIndex) const
{
    int mouseX = mMouseMoveX;
    int posUnderCursor = getFrameNumber(mMousePressX);
    int standardWidth = mFrameSize - 2;
    int recWidth = standardWidth;
    int recHeight = mLayerHeight - 4;
    int recTop = getLayerY(layerIndex) + 1;

    painter.save();
    for (int framePos : layer->getSelectedFramesByPos()) {

        KeyFrame* key = layer->getKeyFrameAt(framePos);
        if (key->length() > 1)
        {
            // This is a special case for sound clip.
            // Sound clip is the only type of KeyFrame that has variable frame length.
            recWidth = mFrameSize * key->length();
        }

        painter.setBrush(QColor(60, 60, 60));
        painter.setPen(QPen(QBrush(QColor(40, 40, 40)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        int frameX = getFrameX(framePos);
        if (mMovingFrames) {
            int offset = (framePos - posUnderCursor) + mFrameOffset;
            int newFrameX = getFrameX(getFrameNumber(getFrameX(offset)+mouseX)) - standardWidth;
            // Paint as frames are hovering
            painter.drawRect(newFrameX, recTop-4, recWidth, recHeight);

        } else {
            int currentFrameX = frameX - standardWidth;
            painter.drawRect(currentFrameX, recTop, recWidth, recHeight);
        }
    }
    painter.restore();
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

void TimeLineCells::paintLayerGutter(QPainter& painter) const
{
    painter.setPen(QApplication::palette().color(QPalette::Mid));
    if (mMouseMoveY > mLayerDetachThreshold)
    {
        painter.drawRect(0, getLayerY(getInbetweenLayerNumber(mEndY))+mLayerHeight, width(), 2);
    }
    else
    {
        painter.drawRect(0, getLayerY(getInbetweenLayerNumber(mEndY)), width(), 2);
    }
}

void TimeLineCells::paintOnionSkin(QPainter& painter) const
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

        if (mPrevFrame != mEditor->currentFrame()  || mEditor->playback()->isPlaying())
        {
            mPrevFrame = mEditor->currentFrame();
            trackScrubber();
        }

        if (!isPlaying) {
            paintOnionSkin(painter);
        }

        // --- draw the position of the current frame
        if (mEditor->currentFrame() > mFrameOffset)
        {
            QColor scrubColor = palette.color(QPalette::Highlight);
            scrubColor.setAlpha(160);
            painter.setBrush(scrubColor);
            painter.setPen(Qt::NoPen);

            int currentFrameStartX = getFrameX(mEditor->currentFrame() - 1) + 1;
            int currentFrameEndX = getFrameX(mEditor->currentFrame());
            QRect scrubRect;
            scrubRect.setTopLeft(QPoint(currentFrameStartX, 0));
            scrubRect.setBottomRight(QPoint(currentFrameEndX, height()));
            if (mbShortScrub)
            {
                scrubRect.setBottomRight(QPoint(currentFrameEndX, 19));
            }
            painter.save();

            bool mouseUnderScrubber = mEditor->currentFrame() == mFramePosMoveX;
            if (mouseUnderScrubber) {
                QRect smallScrub = QRect(QPoint(currentFrameStartX, 0), QPoint(currentFrameEndX,19));
                QPen pen = scrubColor;
                pen.setWidth(2);
                painter.setPen(pen);
                painter.drawRect(smallScrub);
                painter.setBrush(Qt::NoBrush);
            }
            painter.drawRect(scrubRect);
            painter.restore();

            painter.setPen(palette.color(QPalette::HighlightedText));
            int incr = (mEditor->currentFrame() < 10) ? 4 : 0;
            painter.drawText(QPoint(currentFrameStartX + incr, 15),
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

bool TimeLineCells::event(QEvent* event)
{
    if (event->type() == QEvent::Leave) {
        onDidLeaveWidget();
    }

    return QWidget::event(event);
}

void TimeLineCells::mousePressEvent(QMouseEvent* event)
{
    int frameNumber = getFrameNumber(event->pos().x());
    int layerNumber = getLayerNumber(event->pos().y());
    mCurrentLayerNumber = layerNumber;

    mMousePressX = event->pos().x();
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
            else if (mEditor->currentLayerIndex() != layerNumber)
            {
                mEditor->layers()->setCurrentLayer(layerNumber);
                mEditor->layers()->currentLayer()->deselectAll();
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
            if (frameNumber == mEditor->currentFrame() && mStartY < 20)
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
                        // If it is the case, we select it
                        mCanBoxSelect = true;
                        mClickSelecting = true;
                        if (event->button() == Qt::RightButton)
                        {
                            showCameraMenu(event->pos());
                        }

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
                        else
                        {
                            currentLayer->toggleFrameSelected(frameNumber, false);
                            mEditor->selectedFramesChanged();
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

                    if (currentLayer->hasAnySelectedFrames()) {
                        emit selectionChanged();
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
    mMouseMoveX = event->pos().x();
    mFramePosMoveX = getFrameNumber(mMouseMoveX);
    mLayerPosMoveY = getLayerNumber(event->pos().y());

    if (mType == TIMELINE_CELL_TYPE::Layers)
    {
        if (event->buttons() & Qt::LeftButton ) {
            mEndY = event->pos().y();
            emit mouseMovedY(mEndY - mStartY);
        }
    }
    else if (mType == TIMELINE_CELL_TYPE::Tracks)
    {
        if (primaryButton == Qt::MidButton)
        {
            mFrameOffset = qMin(qMax(0, mFrameLength - width() / getFrameSize()), qMax(0, mFrameOffset + mLastFrameNumber - mFramePosMoveX));
            update();
            emit offsetChanged(mFrameOffset);
        }
        else
        {
            if (mTimeLine->scrubbing)
            {
                if (mEditor->playback()->getSoundScrubActive() && mLastScrubFrame != mFramePosMoveX)
                {
                    mEditor->playback()->playScrub(mFramePosMoveX);
                    mLastScrubFrame = mFramePosMoveX;
                }
                mEditor->scrubTo(mFramePosMoveX);
            }
            else
            {
                if (event->buttons() & Qt::LeftButton) {
                    if (mStartLayerNumber != -1 && mStartLayerNumber < mEditor->object()->getLayerCount())
                    {
                        Layer *currentLayer = mEditor->object()->getLayer(mStartLayerNumber);

                        // Check if the frame we clicked was selected
                        if (mCanMoveFrame) {

                            // If it is the case, we move the selected frames in the layer
                            mMovingFrames = true;
                        }
                        else if (mCanBoxSelect)
                        {
                            // Otherwise, we do a box select
                            mBoxSelecting = true;

                            currentLayer->deselectAll();
                            currentLayer->setFrameSelected(mStartFrameNumber, true);
                            currentLayer->extendSelectionTo(mFramePosMoveX);
                        }
                        mLastFrameNumber = mFramePosMoveX;
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
    if (frameNumber < 1) frameNumber = 1;
    int layerNumber = getLayerNumber(event->pos().y());

    if (mCurrentLayerNumber != -1 && mType == TIMELINE_CELL_TYPE::Tracks && primaryButton != Qt::MidButton)
    {
        // We should affect the current layer based on what's selected, not where the mouse currently is.
        Layer* currentLayer = mEditor->layers()->getLayer(mCurrentLayerNumber);

        Q_ASSERT(currentLayer);
        if (mMovingFrames)
        {
            int posUnderCursor = getFrameNumber(mMousePressX);
            int offset = frameNumber - posUnderCursor;

            currentLayer->moveSelectedFrames(offset);

            mEditor->layers()->notifyAnimationLengthChanged();
            mEditor->framesModified();
        }
        else if (!mTimeLine->scrubbing && !mMovingFrames && !mClickSelecting && !mBoxSelecting)
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

    mMovingFrames = false;
}

void TimeLineCells::mouseDoubleClickEvent(QMouseEvent* event)
{
    int frameNumber = getFrameNumber(event->pos().x());
    int layerNumber = getLayerNumber(event->pos().y());

    // -- short scrub --
    if (event->pos().y() < 20 && (mType != TIMELINE_CELL_TYPE::Layers || event->pos().x() >= 15))
    {
        mPrefs->set(SETTING::SHORT_SCRUB, !mbShortScrub);
    }

    // -- layer --
    Layer* layer = mEditor->object()->getLayer(layerNumber);
    if (layer && event->buttons() & Qt::LeftButton)
    {
        if (mType == TIMELINE_CELL_TYPE::Tracks && (layerNumber != -1) && (frameNumber > 0) && layerNumber < mEditor->object()->getLayerCount())
        {
            mEditor->scrubTo(frameNumber);
            emit insertNewKeyFrame();

            // The release event will toggle the frame on again, so we make sure it gets
            // deselected now instead.
            layer->setFrameSelected(frameNumber, true);
        }
        else if (mType == TIMELINE_CELL_TYPE::Layers && event->pos().x() >= 15)
        {
            editLayerProperties(layer);
        }
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

    CameraPropertiesDialog dialog(cameraLayer->name(), cameraLayer->getViewRect().width(), cameraLayer->getViewRect().height());
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

void TimeLineCells::onDidLeaveWidget()
{
    // Reset last known frame pos to avoid wrong UI states when leaving the widget
    mFramePosMoveX = 0;
    update();
}
