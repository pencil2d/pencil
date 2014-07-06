#include "object.h"
#include "editor.h"
#include "layerimage.h"
#include "layermanager.h"


LayerManager::LayerManager( QObject* pParent )
    : BaseManager( pParent )
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

void LayerManager::setCurrentLayer( int layerIndex )
{
    m_currentLayerIndex = layerIndex;
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
