#include "object.h"
#include "editor.h"
#include "layerimage.h"
#include "layermanager.h"
#include "layersound.h"

LayerManager::LayerManager( QObject* pParent ) : BaseManager( pParent )
{
}

LayerManager::~LayerManager()
{
}

bool LayerManager::init()
{
    return true;
}

// Layer management
int LayerManager::getLastCameraLayer()
{
    return lastCameraLayer;
}

Layer* LayerManager::currentLayer()
{
    return currentLayer( 0 );
}

Layer* LayerManager::currentLayer( int incr )
{
    Q_ASSERT( editor()->object() != NULL );

    return editor()->object()->getLayer( mCurrentLayerIndex + incr );
}

int LayerManager::currentLayerIndex()
{
    return mCurrentLayerIndex;
}

void LayerManager::setCurrentLayer( int layerIndex )
{
    if ( mCurrentLayerIndex != layerIndex )
    {
        mCurrentLayerIndex = layerIndex;
        Q_EMIT currentLayerChanged( mCurrentLayerIndex );
    }
    if ( editor()->object()!=nullptr )
    {
        if ( editor()->object()->getLayer( layerIndex )->type() == Layer::CAMERA )
        {
            lastCameraLayer = layerIndex;
        }
    }
}

void LayerManager::setCurrentLayer( Layer* layer )
{
    Object* o = editor()->object();

    for ( int i = 0; i < o->getLayerCount(); ++i )
    {
        if ( layer == o->getLayer( i ) )
        {
            setCurrentLayer( i );
            return;
        }
    }
}

void LayerManager::gotoNextLayer()
{
    if ( mCurrentLayerIndex < editor()->object()->getLayerCount() - 1 )
    {
        mCurrentLayerIndex += 1;
		Q_EMIT currentLayerChanged( mCurrentLayerIndex );
    }
}

void LayerManager::gotoPreviouslayer()
{
    if ( mCurrentLayerIndex > 0 )
    {
        mCurrentLayerIndex -= 1;
		Q_EMIT currentLayerChanged( mCurrentLayerIndex );
    }
}

LayerSound* LayerManager::createSoundLayer( QString strLayerName )
{
    LayerSound* layer = editor()->object()->addNewSoundLayer();
    layer->setName( strLayerName );
    
    Q_EMIT layerCountChanged( count() );

    return layer;
}

int LayerManager::LastFrameAtFrame( int frameIndex )
{
    Object* pObj = editor()->object();
    for ( int i = frameIndex; i >= 0; i -= 1 )
    {
        for ( int layerIndex = 0; layerIndex < pObj->getLayerCount(); ++layerIndex )
        {
            auto pLayer = pObj->getLayer( layerIndex );
            if ( pLayer->keyExists( i ) )
            {
                return i;
            }
        }
    }
    return -1;
}

int LayerManager::firstKeyFrameIndex()
{
    int minPosition = INT_MAX;

    Object* pObj = editor()->object();
    for ( int i = 0; i < pObj->getLayerCount(); ++i )
    {
        Layer* pLayer = pObj->getLayer( i );

        int position = pLayer->firstKeyFramePosition();
        if ( position < minPosition )
        {
            minPosition = position;
        }
    }
    return minPosition;
}

int LayerManager::lastKeyFrameIndex()
{
    int maxPosition = 0;

    for ( int i = 0; i < editor()->object()->getLayerCount(); ++i )
    {
        Layer* pLayer = editor()->object()->getLayer( i );

        int position = pLayer->getMaxKeyFramePosition();
        if ( position > maxPosition )
        {
            maxPosition = position;
        }
    }
    return maxPosition;
}

int LayerManager::count()
{
    return editor()->object()->getLayerCount();
}

bool LayerManager::deleteCurrentLayer()
{
    if ( currentLayer()->type() == Layer::CAMERA )
    {
        return false;
    }

    editor()->object()->deleteLayer( currentLayerIndex() );

    if ( currentLayerIndex() == editor()->object()->getLayerCount() )
    {
        setCurrentLayer( currentLayerIndex() - 1 );
    }

    Q_EMIT layerCountChanged( count() );

    return true;
}

int LayerManager::projectLength()
{
    int maxFrame = -1;

    Object* pObject = editor()->object();
    for ( int i = 0; i < pObject->getLayerCount(); i++ )
    {
        int frame = pObject->getLayer( i )->getMaxKeyFramePosition();
        if ( frame > maxFrame )
        {
            maxFrame = frame;
        }
    }
    return maxFrame;
}

void LayerManager::gotoLastKeyFrame()
{
    int nFrame = lastKeyFrameIndex();
    editor()->scrubTo( nFrame );
}

void LayerManager::gotoFirstKeyFrame()
{
    int nFrame = firstKeyFrameIndex();
    editor()->scrubTo( nFrame );
}
