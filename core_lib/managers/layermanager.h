#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include "basemanager.h"

class Layer;

class LayerManager : public BaseManager
{
    Q_OBJECT

public:
    LayerManager( QObject* pParant );
    ~LayerManager();
    bool initialize() override;

    Layer* currentLayer();
    Layer* currentLayer( int incr );
    int currentLayerIndex();
    void setCurrentLayerIndex( int );

    void gotoNextLayer();
    void gotoPreviouslayer();

    int LastFrameAtFrame( int frameIndex );

    int  currentFrameIndex();
    void setCurrentFrameIndex( int );
    int firstKeyFrameIndex();
    int lastKeyFrameIndex();

private:
    int m_currentLayerIndex; // the current layer to be edited/displayed
    int m_currentFrameIndex; // the current key frame to be edited/displayed
};

#endif
