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
    ActiveFramePool(int maxSize);

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
