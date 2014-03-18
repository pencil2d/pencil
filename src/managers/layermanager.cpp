#include "object.h"
#include "editor.h"
#include "layerimage.h"
#include "layermanager.h"


LayerManager::LayerManager( QObject* pParant = 0 ) 
    : BaseManager( pParant )
{
}

LayerManager::~LayerManager()
{
}

bool LayerManager::initialize()
{
    return true;
}

// Layer management
Layer* LayerManager::currentLayer()
{
    return currentLayer( 0 );
}

Layer* LayerManager::currentLayer( int incr )
{
    Q_ASSERT( editor()->object() != NULL );

    return editor()->object()->getLayer( m_currentLayerIndex + incr );
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
    if ( m_currentLayerIndex < editor()->object()->getLayerCount() - 1 )
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
    Object* pObj = editor()->object();
    for ( int i = frameIndex; i >= 0; i -= 1 )
    {
        for ( int layerIndex = 0; layerIndex < pObj->getLayerCount(); ++layerIndex )
        {
            auto pLayer = static_cast<LayerImage*>( pObj->getLayer( layerIndex ) );
            if ( pLayer->hasKeyFrameAtPosition( i ) )
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

        int position = pLayer->getFirstKeyFramePosition();
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

        int position = pLayer->getFirstKeyFramePosition();
        if ( position > maxPosition )
        {
            maxPosition = position;
        }
    }
    return maxPosition;
}