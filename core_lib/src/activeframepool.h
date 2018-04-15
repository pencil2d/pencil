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

#ifndef ACTIVEFRAMEPOOL_H
#define ACTIVEFRAMEPOOL_H

#include <list>
#include <unordered_map>
#include "keyframe.h"


/** 
 * ActiveFramePool implemented a LRU cache to keep tracking the most recent accessed key frames
 * A key frame will be unloaded if it's not accessed for a while (at the end of cache list)
 * The ActiveFramePool will be updated whenever Editor::scrubTo() gets called.
 */
class ActiveFramePool : public KeyFrameEventListener
{
public:
    explicit ActiveFramePool(int maxSize);

    void put(KeyFrame* key);
    size_t size() const;
    void clear();

    void onKeyFrameDestroy(KeyFrame*) override;

private:
    void unloadFrame(KeyFrame* key);

    typedef std::list<KeyFrame*>::iterator list_iterator_t;

    std::list<KeyFrame*> mCacheFramesList;
    std::unordered_map<KeyFrame*, list_iterator_t> mCacheFramesMap;
    size_t mMaxSize = 200;
};

#endif // ACTIVEFRAMEPOOL_H
