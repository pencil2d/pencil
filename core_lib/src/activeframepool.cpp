/*

Pencil - Traditional Animation Software
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

#include "activeframepool.h"
#include "keyframe.h"
#include "pencildef.h"
#include <QDebug>


ActiveFramePool::ActiveFramePool()
{
    Q_ASSERT(mMemoryBudgetInBytes >= (1024 * 1024 * 100)); // at least 100MB
}

ActiveFramePool::~ActiveFramePool() {}

void ActiveFramePool::put(KeyFrame* key)
{
    if (key == nullptr)
        return;

    Q_ASSERT(key->pos() > 0);

    key->loadFile();

    auto it = mCacheFramesMap.find(key);
    const bool keyExistsInPool = (it != mCacheFramesMap.end());
    if (keyExistsInPool)
    {
        // move the keyframe to the front of the list, if the key already exists in frame pool
        mCacheFramesList.erase(it->second);
    }
    mCacheFramesList.push_front(key);
    mCacheFramesMap[key] = mCacheFramesList.begin();

    key->addEventListener(this);

    if (!keyExistsInPool)
    {
        mTotalUsedMemory += key->memoryUsage();
        //qDebug() << "Total Memory:" << mTotalUsedMemory;
    }

    discardLeastUsedFrames();
}

size_t ActiveFramePool::size() const
{
    return mCacheFramesMap.size();
}

void ActiveFramePool::clear()
{
    for (KeyFrame* key : mCacheFramesList)
    {
        key->removeEventListner(this);
    }
    mCacheFramesList.clear();
    mCacheFramesMap.clear();
}

void ActiveFramePool::resize(quint64 memoryBudget)
{
    memoryBudget = qMin(memoryBudget, quint64(1024) * 1024 * 1024 * 16); // 16GB
    memoryBudget = qMax(memoryBudget, quint64(1024) * 1024 * 100); // 100MB
    mMemoryBudgetInBytes = memoryBudget;
    discardLeastUsedFrames();
}

bool ActiveFramePool::isFrameInPool(KeyFrame* key)
{
    auto it = mCacheFramesMap.find(key);
    return (it != mCacheFramesMap.end());
}

void ActiveFramePool::onKeyFrameDestroy(KeyFrame* key)
{
    auto it = mCacheFramesMap.find(key);
    if (it != mCacheFramesMap.end())
    {
        mCacheFramesList.erase(it->second);
        mCacheFramesMap.erase(it);

        // Just recalculate the total usage
        // Not safe to call key->memoryUsage() here cuz it's in the KeyFrame's destructor
        recalcuateTotalUsedMemory();
    }
}

void ActiveFramePool::discardLeastUsedFrames()
{
    while (mTotalUsedMemory > mMemoryBudgetInBytes)
    {
        list_iterator_t last = mCacheFramesList.end();
        last--;

        KeyFrame* lastKeyFrame = *last;
        unloadFrame(lastKeyFrame);

        mCacheFramesMap.erase(lastKeyFrame);
        mCacheFramesList.pop_back();

        lastKeyFrame->removeEventListner(this);
    }
}

void ActiveFramePool::unloadFrame(KeyFrame* key)
{
    //qDebug() << "Unload frame:" << key->pos();
    mTotalUsedMemory -= key->memoryUsage();
    key->unloadFile();
    //qDebug() << "Total Memory:" << mTotalUsedMemory;
}

void ActiveFramePool::recalcuateTotalUsedMemory()
{
    mTotalUsedMemory = 0;
    for (KeyFrame* key : mCacheFramesList)
    {
        mTotalUsedMemory += key->memoryUsage();
    }
    //qDebug() << "Total Memory:" << mTotalUsedMemory;
}
