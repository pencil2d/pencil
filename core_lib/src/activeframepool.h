#ifndef ACTIVEFRAMEPOOL_H
#define ACTIVEFRAMEPOOL_H

#include <list>
#include <unordered_map>
#include "keyframe.h"

class ActiveFramePool : public KeyFrameEventListener
{
public:
    ActiveFramePool();

    void put(KeyFrame* key);
    size_t size() const;
    void clear();

    void onKeyFrameDestroy(KeyFrame*) override;

private:
    void unloadFrame(KeyFrame* key);

    typedef typename std::list<KeyFrame*>::iterator list_iterator_t;

    std::list<KeyFrame*> mCacheFramesList;
    std::unordered_map<KeyFrame*, list_iterator_t> mCacheFramesMap;
    size_t mMaxSize = 10;
};

#endif // ACTIVEFRAMEPOOL_H
