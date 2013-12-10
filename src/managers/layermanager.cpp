#include "editor.h"
#include "layermanager.h"


LayerManager::LayerManager( QObject* pParant = 0 ) : QObject( pParant )
{
}

LayerManager::~LayerManager()
{
}

bool LayerManager::init( Editor* pEditor )
{
    if ( pEditor == NULL )
    {
        return false;
    }

    m_pEditor = pEditor;
    return true;
}

Layer* LayerManager::currentLayer()
{
    return currentLayer( 0 );
}

Layer* LayerManager::currentLayer( int incr )
{
    Q_ASSERT( m_pEditor != NULL );

    return m_pEditor->object()->getLayer( m_pEditor->m_nCurrentLayerIndex + incr );
}