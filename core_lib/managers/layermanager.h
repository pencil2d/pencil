#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include "basemanager.h"

class Layer;
class LayerBitmap;
class LayerVector;
class LayerCamera;
class LayerSound;


class LayerManager : public BaseManager
{
    Q_OBJECT

public:
    LayerManager( QObject* pParant );
    ~LayerManager();
    bool init() override;
    Status onObjectLoaded( Object* ) override;

    // Layer Management
    Layer* currentLayer();
    Layer* currentLayer( int offset );
    Layer* getLayer( int index );
    int    getLastCameraLayer();
    int    currentLayerIndex();
    void   setCurrentLayer( int nIndex );
    void   setCurrentLayer( Layer* layer );
    int    count();

    bool   deleteCurrentLayer();

    void gotoNextLayer();
    void gotoPreviouslayer();
    
    LayerBitmap* createBitmapLayer( const QString& strLayerName );
    LayerVector* createVectorLayer( const QString& strLayerName );
    LayerCamera* createCameraLayer( const QString& strLayerName );
    LayerSound*  createSoundLayer( const QString& strLayerName );
    
    // KeyFrame Management
    int  LastFrameAtFrame( int frameIndex );
    int  firstKeyFrameIndex();
    int  lastKeyFrameIndex();

    void gotoLastKeyFrame();
    void gotoFirstKeyFrame();

    int  projectLength();

    void layerUpdated( int layerId );

Q_SIGNALS:
    void currentLayerChanged( int n );
    void layerCountChanged( int count );

private:
    int mCurrentLayerIndex = 0; // the current layer to be edited/displayed
    int lastCameraLayer = 0;
};

#endif
