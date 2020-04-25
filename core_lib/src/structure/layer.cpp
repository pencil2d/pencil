/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "layer.h"

#include <QDebug>
#include <QSettings>
#include "keyframe.h"
#include "object.h"
#include "timelinecells.h"

#include "insertbehaviour.h"


// Used to sort the selected frames list
bool sortAsc(int left, int right)
{
    return left < right;
}

Layer::Layer(Object* pObject, LAYER_TYPE eType) : QObject(pObject)
{
    Q_ASSERT(eType != UNDEFINED);

    mObject = pObject;
    meType = eType;
    mName = QString(tr("Undefined Layer"));

    mId = pObject->getUniqueLayerID();
}

Layer::~Layer()
{
    for (auto it : mKeyFrames)
    {
        KeyFrame* pKeyFrame = it.second;
        delete pKeyFrame;
    }
    mKeyFrames.clear();
}

void Layer::foreachKeyFrame(std::function<void(KeyFrame*)> action)
{
    for (auto pair : mKeyFrames)
    {
        action(pair.second);
    }
}

bool Layer::keyExists(int position) const
{
    return (mKeyFrames.find(position) != mKeyFrames.end());
}

KeyFrame* Layer::getKeyFrameAt(int position) const
{
    auto it = mKeyFrames.find(position);
    if (it == mKeyFrames.end())
    {
        return nullptr;
    }
    return it->second;
}

KeyFrame* Layer::getLastKeyFrameAtPosition(int position) const
{
    if (position < 1)
    {
        position = 1;
    }
    auto it = mKeyFrames.lower_bound(position);
    if (it == mKeyFrames.end())
    {
        return nullptr;
    }
    return it->second;
}

int Layer::getPreviousKeyFramePosition(int position) const
{
    auto it = mKeyFrames.upper_bound(position);
    if (it == mKeyFrames.end())
    {
        return firstKeyFramePosition();
    }
    return it->first;
}

int Layer::getNextKeyFramePosition(int position) const
{
    // workaround: bug with lower_bound?
    // when position is before the first frame it == mKeyFrames.end() for some reason
    if (position < firstKeyFramePosition())
    {
       return firstKeyFramePosition();
    }

    auto it = mKeyFrames.lower_bound(position);
    if (it == mKeyFrames.end())
    {
        return getMaxKeyFramePosition();
    }

    if (it != mKeyFrames.begin())
    {
        --it;
    }
    return it->first;
}

int Layer::getPreviousFrameNumber(int position, bool isAbsolute) const
{
    int prevNumber;

    if (isAbsolute)
        prevNumber = getPreviousKeyFramePosition(position);
    else
        prevNumber = position - 1;

    if (prevNumber >= position)
    {
        return -1; // There is no previous keyframe
    }
    return prevNumber;
}

int Layer::getNextFrameNumber(int position, bool isAbsolute) const
{
    int nextNumber;

    if (isAbsolute)
        nextNumber = getNextKeyFramePosition(position);
    else
        nextNumber = position + 1;

    if (nextNumber <= position)
        return -1; // There is no next keyframe

    return nextNumber;
}

int Layer::firstKeyFramePosition() const
{
    if (!mKeyFrames.empty())
    {
        return mKeyFrames.rbegin()->first; // rbegin is the lowest key frame position
    }
    return 0;
}

int Layer::getMaxKeyFramePosition() const
{
    if (!mKeyFrames.empty())
    {
        return mKeyFrames.begin()->first; // begin is the highest key frame position
    }
    return 0;
}

bool Layer::addNewKeyFrameAt(int position)
{
    if (position <= 0) return false;

    KeyFrame* key = createKeyFrame(position, mObject);
    return addKeyFrame(position, key);
}

bool Layer::addKeyFrame(int position, KeyFrame* pKeyFrame)
{
    Q_ASSERT(position > 0);
    auto it = mKeyFrames.find(position);
    if (it != mKeyFrames.end())
    {
        return false;
    }

    pKeyFrame->setPos(position);
    mKeyFrames.insert(std::make_pair(position, pKeyFrame));

    return true;
}

bool Layer::addKeyFrameAfter(int position, KeyFrame* pKeyFrame)
{
    Q_ASSERT(position > 0);
//    auto posIt = mKeyFrames.find(position);

//    std::map<int, KeyFrame*, std::greater<int>>::iterator it = mKeyFrames.begin();
//    for (it=mKeyFrames.begin(); it!=mKeyFrames.end(); ++it)

    for (auto it = mKeyFrames.begin(); mKeyFrames.end()!=it; ++it) {

        qDebug() << it->first;
        qDebug() << it->second;
        if (it->first < position) {
            break;
        }


        KeyFrame* key = it->second;
        int newPos = key->pos()+1;
        key->setPos(newPos);

        if (mKeyFrames.find(newPos) == mKeyFrames.end()) {
//            KeyFrame* key2 = createKeyFrame(newPos, mObject);
            auto newIt = mKeyFrames.insert(std::make_pair(newPos, std::move(key)));

            qDebug() << newIt.second;
        } else {
            mKeyFrames[newPos] = std::move(key);
        }
    }

    pKeyFrame->setPos(position);
    mKeyFrames[position] = std::move(pKeyFrame);

    return true;
}

bool Layer::removeKeyFrame(int position)
{
    auto frame = getKeyFrameWhichCovers(position);
    if (frame)
    {
        mKeyFrames.erase(frame->pos());
        delete frame;
    }
    return true;
}

bool Layer::moveKeyFrameForward(int position)
{
    return swapKeyFrames(position, position + 1);
}

bool Layer::moveKeyFrameBackward(int position)
{
    if (position != 1)
    {
        return swapKeyFrames(position, position - 1);
    }
    return true;
}

bool Layer::swapKeyFrames(int position1, int position2) //Current behaviour, need to refresh the swapped cels
{
    bool keyPosition1 = false;
    bool keyPosition2 = false;
    KeyFrame* pFirstFrame = nullptr;
    KeyFrame* pSecondFrame = nullptr;

    if (keyExists(position1))
    {
        auto firstFrame = mKeyFrames.find(position1);
        pFirstFrame = firstFrame->second;

        mKeyFrames.erase(position1);

        keyPosition1 = true;
    }

    if (keyExists(position2))
    {
        auto secondFrame = mKeyFrames.find(position2);
        pSecondFrame = secondFrame->second;

        mKeyFrames.erase(position2);

        keyPosition2 = true;
    }

    if (keyPosition2)
    {
        pSecondFrame->setPos(position1);
        mKeyFrames.insert(std::make_pair(position1, pSecondFrame));
    }
    else if (position1 == 1)
    {
        addNewKeyFrameAt(position1);
    }

    if (keyPosition1)
    {
        pFirstFrame->setPos(position2);
        mKeyFrames.insert(std::make_pair(position2, pFirstFrame));
    }
    else if (position2 == 1)
    {
        addNewKeyFrameAt(position2);
    }

    if (pFirstFrame)
        pFirstFrame->modification();

    if (pSecondFrame)
        pSecondFrame->modification();

    return true;
}

bool Layer::loadKey(KeyFrame* pKey)
{
    auto it = mKeyFrames.find(pKey->pos());
    if (it != mKeyFrames.end())
    {
        delete it->second;
        mKeyFrames.erase(it);
    }
    mKeyFrames.emplace(pKey->pos(), pKey);
    return true;
}

Status Layer::save(const QString& sDataFolder, QStringList& attachedFiles, ProgressCallback progressStep)
{
    DebugDetails dd;
    dd << __FUNCTION__;

    bool ok = true;

    for (auto pair : mKeyFrames)
    {
        KeyFrame* keyFrame = pair.second;
        Status st = saveKeyFrameFile(keyFrame, sDataFolder);
        if (st.ok())
        {
            //qDebug() << "Layer [" << name() << "] FN=" << keyFrame->fileName();
            if (!keyFrame->fileName().isEmpty())
                attachedFiles.append(keyFrame->fileName());
        }
        else
        {
            ok = false;
            dd.collect(st.details());
            dd << QString("- Keyframe[%1] failed to save").arg(keyFrame->pos());
        }
        progressStep();
    }
    if (!ok)
    {
        return Status(Status::FAIL, dd);
    }
    return Status::OK;
}

void Layer::paintTrack(QPainter& painter, TimeLineCells* cells,
                       int x, int y, int width, int height,
                       bool selected, int frameSize)
{
    if (mVisible)
    {
        QColor col;
        if (type() == BITMAP) col = QColor(51, 155, 252);
        if (type() == VECTOR) col = QColor(70, 205, 123);
        if (type() == SOUND) col = QColor(255, 141, 112);
        if (type() == CAMERA) col = QColor(253, 202, 92);

        painter.save();
        painter.setBrush(col);
        painter.setPen(QPen(QBrush(QColor(100, 100, 100)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawRect(x, y - 1, width, height);

        // changes the apparence if selected
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

        paintFrames(painter, col, cells, y, height, selected, frameSize);

        painter.restore();
    }
    else
    {
        painter.setBrush(Qt::gray);
        painter.setPen(QPen(QBrush(QColor(100, 100, 100)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawRect(x, y - 1, width, height); // empty rectangle  by default
    }
}

void Layer::paintFrames(QPainter& painter, QColor trackCol, TimeLineCells* cells, int y, int height, bool selected, int frameSize)
{
    painter.setPen(QPen(QBrush(QColor(40, 40, 40)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    QList<KeyFrame*> movingFrames;
    for (auto pair : mKeyFrames)
    {
        int framePos = pair.first;

        int recLeft = cells->getFrameX(framePos) - frameSize + 2;
        int recTop = y + 1;
        int recWidth = frameSize - 2;
        int recHeight = height - 4;

        KeyFrame* key = pair.second;
        qDebug() << "mkeyFrames-key: " << pair.first;
        if (key->length() > 1)
        {
            // This is especially for sound clip.
            // Sound clip is the only type of KeyFrame that has variant frame length.
            recWidth = frameSize * key->length() - 2;
        }

        if (selected && key->pos() == cells->getCurrentFrame()) {
            painter.setPen(Qt::white);
        } else {
            painter.setPen(QPen(QBrush(QColor(40, 40, 40)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        }

        if (pair.second->isSelected())
        {
            painter.setBrush(QColor(60, 60, 60));
        }
        else if (selected)
        {            
            painter.setBrush(QColor(trackCol.red(), trackCol.green(), trackCol.blue(), 150));
        }

        if (key->isSelected() && cells->movingFrames() && cells->mouseButtonDown()) {
            movingFrames << key;
        } else {
            painter.drawRect(recLeft, recTop, recWidth, recHeight);
        }
    }


    cells->clearMovedFrames();

    for (KeyFrame* key : movingFrames) {

        int framePos = key->pos();
        int direction = 0;
        if (cells->getLastMouseX() > cells->getMouseXPos())
        {
//            qDebug() << "moving right";
             direction = 1;
        }
        else
        {
//            qDebug() <<" moving left";
            direction = -1;
        }

        int recTop = y + 1;
        int recWidth = frameSize - 2;
        int recHeight = height - 4;

        int translatedFramePos = cells->getMouseXPos()-cells->getFrameX(framePos);
        int posUnderCursor = cells->getFrameNumber(cells->mousePressPosX());
        int offset = framePos - posUnderCursor;

        qDebug() << "offset: " << offset;

        int newFrameX = cells->getFrameX(framePos)+(offset*frameSize)+translatedFramePos;
//        int newFramePos = cells->getFrameNumber(newFrameX);

        int movingFromStart = cells->getFrameNumber(cells->getMouseXPos()) - framePos;

//        qDebug() << "moving from: " << key->pos();
//        qDebug() << "moved to: " << newFramePos;

        qDebug() << "offset from start + offset: " << movingFromStart + offset;
        qDebug() << "offset from start: " << movingFromStart;

        if (key->length() > 1)
        {
            // This is especially for sound clip.
            // Sound clip is the only type of KeyFrame that has variant frame length.
            recWidth = frameSize * key->length() - 2;
        }

        if (key->isSelected())
        {
            painter.setBrush(QColor(60, 60, 60));
        }
        painter.drawRect(newFrameX, recTop, recWidth, recHeight);

        cells->addFramesToBeMoved(key->pos(), movingFromStart+offset);
    }
}

void Layer::paintLabel(QPainter& painter, TimeLineCells* cells,
                       int x, int y, int width, int height,
                       bool selected, LayerVisibility layerVisibility)
{
    Q_UNUSED(cells)
    painter.setBrush(Qt::lightGray);
    painter.setPen(QPen(QBrush(QColor(100, 100, 100)), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawRect(x, y - 1, width, height); // empty rectangle  by default


    if (selected)
    {
        paintSelection(painter, x, y, width, height);
    } else {
        painter.save();
        QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
        linearGrad.setColorAt(0, QColor(255,255,255,150));
        linearGrad.setColorAt(1, QColor(0,0,0,0));
        painter.setCompositionMode(QPainter::CompositionMode_Overlay);
        painter.setBrush(linearGrad);
        painter.drawRect(x, y - 1, width, height);
        painter.restore();
    }

    if (mVisible)
    {
        if ((layerVisibility == LayerVisibility::ALL) || selected) painter.setBrush(Qt::black);
        else if (layerVisibility == LayerVisibility::CURRENTONLY) painter.setBrush(Qt::NoBrush);
        else if (layerVisibility == LayerVisibility::RELATED) painter.setBrush(Qt::darkGray);
    }
    else
    {
        painter.setBrush(Qt::NoBrush);
    }
    painter.setPen(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(x + 6, y + 4, 9, 9);
    painter.setRenderHint(QPainter::Antialiasing, false);

    if (type() == BITMAP) painter.drawPixmap(QPoint(20, y + 2), QPixmap(":/icons/layer-bitmap.png"));
    if (type() == VECTOR) painter.drawPixmap(QPoint(20, y + 2), QPixmap(":/icons/layer-vector.png"));
    if (type() == SOUND) painter.drawPixmap(QPoint(21, y + 2), QPixmap(":/icons/layer-sound.png"));
    if (type() == CAMERA) painter.drawPixmap(QPoint(21, y + 2), QPixmap(":/icons/layer-camera.png"));

    painter.setPen(Qt::black);
    painter.drawText(QPoint(45, y + (2 * height) / 3), mName);
}

void Layer::paintSelection(QPainter& painter, int x, int y, int width, int height)
{
    QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
    QSettings settings(PENCIL2D, PENCIL2D);
    QString style = settings.value("style").toString();
    linearGrad.setColorAt(0, QColor(0, 0, 0, 255));
    linearGrad.setColorAt(1, QColor(255, 255, 255, 0));
    painter.save();
    painter.setCompositionMode(QPainter::CompositionMode_Overlay);
    painter.setBrush(linearGrad);
    painter.setPen(Qt::NoPen);
    painter.drawRect(x, y, width, height - 1);
    painter.restore();
}

void Layer::mouseDoubleClick(QMouseEvent* event, int frameNumber)
{
    Q_UNUSED(event)
    Q_UNUSED(frameNumber)
}

void Layer::editProperties()
{
}

void Layer::setModified(int position, bool modified)
{
    KeyFrame* key = getKeyFrameAt(position);
    if (key)
    {
        key->setModified(modified);
    }
}

bool Layer::isFrameSelected(int position) const
{
    KeyFrame* keyFrame = getKeyFrameWhichCovers(position);
    if (keyFrame)
    {
        return mSelectedFrames_byLast.contains(keyFrame->pos());
    }
    return false;
}

void Layer::setFrameSelected(int position, bool isSelected)
{
    KeyFrame* keyFrame = getKeyFrameWhichCovers(position);
    if (keyFrame != nullptr)
    {
        int startPosition = keyFrame->pos();

        if (isSelected && !mSelectedFrames_byLast.contains(startPosition))
        {
            // Add the selected frame to the lists
            mSelectedFrames_byLast.insert(0, startPosition);
            mSelectedFrames_byPosition.append(startPosition);

            // We need to keep the list of selected frames sorted
            // in order to easily handle their movement
            std::sort(mSelectedFrames_byPosition.begin(), mSelectedFrames_byPosition.end(), sortAsc);
        }
        else if (!isSelected)
        {
            // Remove the selected frame from the lists
            int iLast = mSelectedFrames_byLast.indexOf(startPosition);
            mSelectedFrames_byLast.removeAt(iLast);

            int iPos = mSelectedFrames_byPosition.indexOf(startPosition);
            mSelectedFrames_byPosition.removeAt(iPos);
        }
        keyFrame->setSelected(isSelected);
    }
}

void Layer::toggleFrameSelected(int position, bool allowMultiple)
{
    bool wasSelected = isFrameSelected(position);

    if (!allowMultiple)
    {
        deselectAll();
    }

    setFrameSelected(position, !wasSelected);
}

void Layer::extendSelectionTo(int position)
{
    if (mSelectedFrames_byLast.count() > 0)
    {
        int lastSelected = mSelectedFrames_byLast[0];
        int startPos;
        int endPos;

        if (lastSelected < position)
        {
            startPos = lastSelected;
            endPos = position;
        }
        else
        {
            startPos = position;
            endPos = lastSelected;
        }

        int i = startPos;
        while (i <= endPos)
        {
            setFrameSelected(i, true);
            i++;
        }
    }
}

void Layer::selectAllFramesAfter(int position)
{
    int startPosition = position;
    int endPosition = getMaxKeyFramePosition();

    if (!keyExists(startPosition))
    {
        startPosition = getNextKeyFramePosition(startPosition);
    }

    if (startPosition > 0 && startPosition <= endPosition)
    {
        deselectAll();
        setFrameSelected(startPosition, true);
        extendSelectionTo(endPosition);
    }
}

void Layer::deselectAll()
{
    mSelectedFrames_byLast.clear();
    mSelectedFrames_byPosition.clear();

    for (auto pair : mKeyFrames)
    {
        pair.second->setSelected(false);
    }
}

bool Layer::moveFrame(int oldPos, int newPos, InsertBehaviour behaviour)
{
    KeyFrame* copiedFrame = getKeyFrameAt(oldPos);

    qDebug() << "new potential pos:" << newPos;
    qDebug() << "old pos:" << oldPos;

    bool offsetDir = 0;
//    if (newPos > oldPos) {
//        offsetDir = 1;
//    } else {
//        offsetDir = -1;
//    }

    if (oldPos == newPos) {
        return false;
    }

    if (copiedFrame != nullptr) {
        copiedFrame->clone();

        if (!keyExists(newPos)) {
            // TODO: should only be able to move if all selected frames can be moved... otherwise don't move
            mKeyFrames.erase(oldPos);
            copiedFrame->setPos(newPos);

            qDebug() << "new pos is: " << newPos;
            mKeyFrames.emplace(newPos+offsetDir, copiedFrame);

            updateSelectionLists(oldPos, newPos);
        } else {
            mKeyFrames.erase(oldPos);

//            if (behaviour == InsertBehaviour::INBETWEEN) {

//                if (keyExists(newPos)) {
//                   newPos++;
//                }
                copiedFrame->setPos(newPos+offsetDir);

//                std::map<int, KeyFrame*, std::greater<int>>::iterator it = mKeyFrames.begin();
                qDebug() << "new pos is: " << newPos+offsetDir;
                addKeyFrameAfter(newPos+offsetDir, copiedFrame);
                updateSelectionLists(oldPos, newPos);
//            }
//            while (keyExists(newPos)) {
//                qDebug() << "would overwrite frame: " << newPos;
//                newPos++;
//            }
        }
    }
    return true;
}

void Layer::updateSelectionLists(int oldPos, int newPos)
{
    // Update selection lists
    for (int i = 0; i < mSelectedFrames_byPosition.count(); i++)
    {
        if (mSelectedFrames_byPosition[i] == oldPos) {
            mSelectedFrames_byPosition[i] = newPos;
            break;
        }
    }
    for (int i = 0; i < mSelectedFrames_byLast.count(); i++)
    {
        if (mSelectedFrames_byLast[i] == oldPos) {
            mSelectedFrames_byLast[i] = newPos;
            break;
        }
    }
}

bool Layer::canMoveSelectedFramesToOffset(int offset) const
{
    QList<int> newByPositions = mSelectedFrames_byPosition;

    std::map<int, KeyFrame*, std::greater<int>>::const_iterator it;
    for (int i = 0; i < newByPositions.count(); i++)
    {
        newByPositions[i] = newByPositions[i] + offset;
    }

    for (it = mKeyFrames.cbegin(); it != mKeyFrames.cend(); ++it) {

        if (newByPositions.contains(it->first)) {
            return false;
        }
    }
    return true;
}

bool Layer::moveSelectedFrames(int offset)
{
    if (offset == 0 || mSelectedFrames_byPosition.count() <= 0) {
        return false;
    }

    // If we are moving to the right we start moving selected frames from the highest (right) to the lowest (left)
    int indexInSelection = mSelectedFrames_byPosition.count() - 1;
    int step = -1;

    if (offset < 0)
    {
        // If we are moving to the left we start moving selected frames from the lowest (left) to the highest (right)
        indexInSelection = 0;
        step = 1;

        // Check if we are not moving out of the timeline
        if (mSelectedFrames_byPosition[0] + offset < 1) return false;
    }

    if (!canMoveSelectedFramesToOffset(offset)) { return false; }

    while (indexInSelection > -1 && indexInSelection < mSelectedFrames_byPosition.count())
    {
        int fromPos = mSelectedFrames_byPosition[indexInSelection];
        int toPos = fromPos + offset;

        // Get the frame to move
        KeyFrame* selectedFrame = getKeyFrameAt(fromPos);

        if (selectedFrame != nullptr && !keyExists(toPos))
        {
            mKeyFrames.erase(fromPos);

            // Update the position of the selected frame
            selectedFrame->setPos(toPos);
            mKeyFrames.insert(std::make_pair(toPos, selectedFrame));
        }
        indexInSelection = indexInSelection + step;
    }

    // Update selection lists
    for (int i = 0; i < mSelectedFrames_byPosition.count(); i++)
    {
        mSelectedFrames_byPosition[i] = mSelectedFrames_byPosition[i] + offset;
    }
    for (int i = 0; i < mSelectedFrames_byLast.count(); i++)
    {
        mSelectedFrames_byLast[i] = mSelectedFrames_byLast[i] + offset;
    }
    return true;
}

bool Layer::isPaintable() const
{
    return (type() == BITMAP || type() == VECTOR);
}

bool Layer::keyExistsWhichCovers(int frameNumber)
{
    return getKeyFrameWhichCovers(frameNumber) != nullptr;
}

KeyFrame* Layer::getKeyFrameWhichCovers(int frameNumber) const
{
    auto keyFrame = getLastKeyFrameAtPosition(frameNumber);
    if (keyFrame != nullptr)
    {
        if (keyFrame->pos() + keyFrame->length() > frameNumber)
        {
            return keyFrame;
        }
    }
    return nullptr;
}

QDomElement Layer::createBaseDomElement(QDomDocument& doc)
{
    QDomElement layerTag = doc.createElement("layer");
    layerTag.setAttribute("id", id());
    layerTag.setAttribute("name", name());
    layerTag.setAttribute("visibility", visible());
    layerTag.setAttribute("type", type());
    return layerTag;
}

void Layer::loadBaseDomElement(QDomElement& elem)
{
    if (!elem.attribute("id").isNull())
    {
        int id = elem.attribute("id").toInt();
        setId(id);
    }
    setName(elem.attribute("name", "untitled"));
    setVisible(elem.attribute("visibility", "1").toInt());
}
