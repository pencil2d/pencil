
#include "editor.h"
#include "layermanager.h"


LayerManager::LayerManager( QObject* pParant = 0 ) : QObject( pParant )
{

}

LayerManager::~LayerManager()
{

}

bool LayerManager::Initialize( Editor* pEditor )
{
    if ( pEditor == NULL )
    {
        return false;
    }

    m_pEditor = pEditor;
    return true;
}

