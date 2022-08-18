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
#include "layer.h"

#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QPainter>
#include <QDomElement>
#include "keyframe.h"
#include "object.h"
#include "timelinecells.h"

// Used to sort the selected frames list
bool sortAsc(int left, int right)
{
    return left < right;
}

Layer::Layer(Object* object, LAYER_TYPE eType)
{
    Q_ASSERT(eType != UNDEFINED);

    mObject = object;
    meType = eType;
    mName = QString(tr("Undefined Layer"));

    mId = object->getUniqueLayerID();
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

void Layer::setObject(Object* obj)
{
    Q_ASSERT(obj);
    mObject = obj;
    mId = mObject->getUniqueLayerID();
}

void Layer::foreachKeyFrame(std::function<void(KeyFrame*)> action) const
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

    markFrameAsDirty(position);

    return true;
}

bool Layer::insertExposureAt(int position)
{
    if(position < 1 || position > getMaxKeyFramePosition() || !getKeyFrameAt(position))
    {
        return false;
    }

    newSelectionOfConnectedFrames(position + 1);
    moveSelectedFrames(1);
    deselectAll();

    return true;
}

bool Layer::removeKeyFrame(int position)
{
    auto frame = getKeyFrameWhichCovers(position);
    if (frame)
    {
        if (frame->isSelected()) {
            removeFromSelectionList(frame->pos());
        }
        mKeyFrames.erase(frame->pos());
        markFrameAsDirty(position);
        delete frame;
    }
    return true;
}

void Layer::removeFromSelectionList(int position)
{
    mSelectedFrames_byLast.removeAll(position);
    mSelectedFrames_byPosition.removeAll(position);
}

bool Layer::moveKeyFrame(int position, int offset)
{
    int newPos = position + offset;
    if (newPos < 1) { return false; }

    auto listOfFramesLast = mSelectedFrames_byLast;
    auto listOfFramesPos = mSelectedFrames_byPosition;
    bool frameSelected = isFrameSelected(position);

    if (swapKeyFrames(position, newPos)) {

        auto oldPosIndex = mSelectedFrames_byPosition.indexOf(position);
        auto newPosIndex = mSelectedFrames_byPosition.indexOf(newPos);

        auto oldLastIndex = mSelectedFrames_byLast.indexOf(position);
        auto newLastndex = mSelectedFrames_byLast.indexOf(newPos);


        // Old position is selected
        if (oldPosIndex != -1) {
            mSelectedFrames_byPosition[oldPosIndex] = newPos;
            mSelectedFrames_byLast[oldLastIndex] = newPos;
        }

        // Old position is selected
        if (newPosIndex != -1) {
            mSelectedFrames_byPosition[newPosIndex] = position;
            mSelectedFrames_byLast[newLastndex] = position;
        }
        return true;
    }

    mSelectedFrames_byLast.clear();
    mSelectedFrames_byPosition.clear();

    setFrameSelected(position, true);

    // If the move fails, assume we can't move at all and revert to old selection
    if (!moveSelectedFrames(offset)) {
        mSelectedFrames_byLast = listOfFramesLast;
        mSelectedFrames_byPosition = listOfFramesPos;
        return false;
    }

    // Remove old position from selection list
    listOfFramesLast.removeOne(position);
    listOfFramesPos.removeOne(position);

    mSelectedFrames_byLast = listOfFramesLast;
    mSelectedFrames_byPosition = listOfFramesPos;

    // If the frame was selected prior to moving, make sure it's still selected.
    setFrameSelected(newPos, frameSelected);

    return true;
}

// Current behaviour, need to refresh the swapped cels
bool Layer::swapKeyFrames(int position1, int position2)
{
    KeyFrame* pFirstFrame = nullptr;
    KeyFrame* pSecondFrame = nullptr;

    if (mKeyFrames.count(position1) != 1 || mKeyFrames.count(position2) != 1)
    {
        return false;
    }

    // Both keys exist
    pFirstFrame = mKeyFrames[position1];
    pSecondFrame = mKeyFrames[position2];

    mKeyFrames[position1] = pSecondFrame;
    mKeyFrames[position2] = pFirstFrame;

    pFirstFrame->setPos(position2);
    pSecondFrame->setPos(position1);

    pFirstFrame->modification();
    pSecondFrame->modification();

    markFrameAsDirty(position1);
    markFrameAsDirty(position2);

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

void Layer::setModified(int position, bool modified) const
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
    if (keyFrame == nullptr) { return false; }

    int frameFound = mSelectedFrames_byLast.contains(keyFrame->pos());
    Q_ASSERT(!frameFound || keyFrame->isSelected());
    return frameFound;
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
            mSelectedFrames_byLast.removeOne(startPosition);
            mSelectedFrames_byPosition.removeOne(startPosition);
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

bool Layer::newSelectionOfConnectedFrames(int position)
{
    // Deselect all before extending to make sure we don't get already
    // selected frames
    deselectAll();

    if (!keyExists(position)) { return false; }

    setFrameSelected(position, true);

    // Find keyframes that are connected and make sure we're below max.
    while (position < getMaxKeyFramePosition() && getKeyFrameWhichCovers(position) != nullptr) {
        position++;
    }

    extendSelectionTo(position);

    return true;
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

bool Layer::canMoveSelectedFramesToOffset(int offset) const
{
    QList<int> newByPositions = mSelectedFrames_byPosition;

    for (int pos : newByPositions)
    {
        pos += offset;
        if (keyExists(pos) && !newByPositions.contains(pos)) {
            return false;
        }
    }

    return true;
}

void Layer::setExposureForSelectedFrames(int offset)
{
    auto selectedFramesByLast = mSelectedFrames_byLast;
    auto selectedFramesByPos = mSelectedFrames_byPosition;

    int addSpaceBetweenFrames = offset;

    if (selectedFramesByLast.isEmpty()) { return; }

    const int max = selectedFramesByPos.count()-1;
    const int posForIndex = selectedFramesByPos[max];
    const int nextPos = getNextKeyFramePosition(selectedFramesByPos[max]);

    // When exposing the frame in front of the right most element in the selection.
    if (posForIndex != nextPos) {
        selectedFramesByPos.append(nextPos);
    }

    auto initialLastList = selectedFramesByLast;
    auto initialPosList = selectedFramesByPos;

    auto offsetList = QList<int>();

    // Create an offset list to have reference of how many frames should be moved
    for (int offset = 0; offset < selectedFramesByPos.count(); offset++)
    {
        int pos = selectedFramesByPos[offset];
        int nextKeyPos = getNextKeyFramePosition(pos);

        if (pos >= getMaxKeyFramePosition()) {
            offsetList << (pos - 1) - getPreviousKeyFramePosition(pos) + addSpaceBetweenFrames;
        } else { // first frame doesn't move so only the space that's required
            offsetList << nextKeyPos - (pos + 1) + addSpaceBetweenFrames;
        }
    }

    // Either positive or negative
    int offsetDirection = offset > 0 ? 1 : -1;

    for (int i = 0; i < selectedFramesByPos.count(); i++) {
        const int itPos = selectedFramesByPos[i];
        const int nextIndex = i + 1;
        const int positionInFront = itPos + 1;

        // Index safety
        if (nextIndex < 0 || nextIndex >= selectedFramesByPos.count()) {
            continue;
        }

        // Offset above 0 will move frames forward
        // Offset below 0 will move a frame backwards
        while ((offset > 0 && getNextKeyFramePosition(itPos) - positionInFront < offsetList[i]) ||
               (getNextKeyFramePosition(itPos) - positionInFront > offsetList[i] && getNextKeyFramePosition(itPos) - positionInFront > 0)) {

           selectAllFramesAfter(getNextKeyFramePosition(itPos));

           for (int selIndex = 0; selIndex < mSelectedFrames_byPosition.count(); selIndex++) {

               if (nextIndex+selIndex >= selectedFramesByPos.count()) { break; }

               int pos = selectedFramesByPos[nextIndex+selIndex];

               if (!mSelectedFrames_byPosition.contains(pos)) { continue; }

               selectedFramesByPos[nextIndex+selIndex] = pos + offsetDirection;

               // To make the sure we get the correct index for last selection list
               // use the initial list where values doesn't affect the index.
               int initialPos = initialPosList[nextIndex+selIndex];
               int indexOfLast = initialLastList.indexOf(initialPos);
               if (indexOfLast == -1 || nextIndex+selIndex >= selectedFramesByLast.count()) {
                   continue;
               }
               selectedFramesByLast[indexOfLast] = selectedFramesByLast[indexOfLast] + offsetDirection;
           }

           moveSelectedFrames(offsetDirection);
        }
    }

    deselectAll();

    // Reselect frames again based on last selection list to ensure selection behaviour
    // works correctly
    for (int pos : selectedFramesByLast) {
        Q_UNUSED(pos)
        setFrameSelected(selectedFramesByLast.takeLast(), true);
    }
}

bool Layer::reverseOrderOfSelection()
{
    QList<int> selectedIndexes = mSelectedFrames_byPosition;

    if (selectedIndexes.isEmpty()) { return false; }

    for (int swapBegin = 0, swapEnd = selectedIndexes.count()-1; swapBegin < swapEnd; swapBegin++, swapEnd--) {
        int oldPos = selectedIndexes[swapBegin];
        int newPos = selectedIndexes[swapEnd];
        bool canSwap = swapKeyFrames(oldPos, newPos);
        Q_ASSERT(canSwap);
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

    for (; indexInSelection > -1 && indexInSelection < mSelectedFrames_byPosition.count(); indexInSelection += step)
    {
        int fromPos = mSelectedFrames_byPosition[indexInSelection];
        int toPos = fromPos + offset;

        // Get the frame to move
        KeyFrame* selectedFrame = getKeyFrameAt(fromPos);

        Q_ASSERT(!keyExists(toPos));

        mKeyFrames.erase(fromPos);
        markFrameAsDirty(fromPos);

        // Update the position of the selected frame
        selectedFrame->setPos(toPos);
        mKeyFrames.insert(std::make_pair(toPos, selectedFrame));
        markFrameAsDirty(toPos);
    }

    // Update selection lists
    for (int& pos : mSelectedFrames_byPosition)
    {
        pos += offset;
    }
    for (int& pos : mSelectedFrames_byLast)
    {
        pos += offset;
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

QDomElement Layer::createBaseDomElement(QDomDocument& doc) const
{
    QDomElement layerTag = doc.createElement("layer");
    layerTag.setAttribute("id", id());
    layerTag.setAttribute("name", name());
    layerTag.setAttribute("visibility", visible());
    layerTag.setAttribute("type", type());
    return layerTag;
}

void Layer::loadBaseDomElement(const QDomElement& elem)
{
    if (!elem.attribute("id").isNull())
    {
        int id = elem.attribute("id").toInt();
        setId(id);
    }
    setName(elem.attribute("name", "untitled"));
    setVisible(elem.attribute("visibility", "1").toInt());
}
