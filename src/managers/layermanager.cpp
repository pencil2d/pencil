#include "object.h"
#include "layerimage.h"
#include "layermanager.h"


LayerManager::LayerManager( QObject* pParant = 0 ) : QObject( pParant )
{
}

LayerManager::~LayerManager()
{
}

bool LayerManager::setObject( Object* pObject )
{
    if ( pObject == NULL )
    {
        return false;
    }

    m_pObject = pObject;
    return true;
}


// Layer management
Layer* LayerManager::currentLayer()
{
    return currentLayer( 0 );
}

Layer* LayerManager::currentLayer( int incr )
{
    Q_ASSERT( m_pObject != NULL );

    return m_pObject->getLayer( m_currentLayerIndex + incr );
}

int LayerManager::currentLayerIndex()
{
    return m_currentLayerIndex;
}

void LayerManager::setCurrentLayerIndex( int layerIndex )
{
    m_currentLayerIndex = layerIndex;
}


// Key frame management
int LayerManager::currentFrameIndex()
{
    return m_currentFrameIndex;
}

void LayerManager::setCurrentFrameIndex( int frameIndex )
{
    m_currentFrameIndex = frameIndex;
}

void LayerManager::gotoNextLayer()
{
    if ( m_currentLayerIndex < m_pObject->getLayerCount() - 1 )
    {
        m_currentLayerIndex += 1;
    }
}

void LayerManager::gotoPreviouslayer()
{
    if ( m_currentLayerIndex > 0 )
    {
        m_currentLayerIndex -= 1;
    }
}

int LayerManager::LastFrameAtFrame( int frameIndex )
{
    for ( int i = frameIndex; i >= 0; i -= 1 )
    {
        for ( int layerIndex = 0; layerIndex < m_pObject->getLayerCount( ); ++layerIndex )
        {
            LayerImage* pLayer = static_cast<LayerImage*>( m_pObject->getLayer( layerIndex ) );
            if ( pLayer->hasKeyframeAtPosition( i ) )
            {
                return i;
            }
        }
    }
    return -1;
}