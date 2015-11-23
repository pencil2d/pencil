#include "test_layermanager.h"
#include "object.h"
#include "editor.h"
#include "layermanager.h"


void TestLayerManager::initTestCase()
{
    Object* object = new Object();
    object->init();

    mEditor = new Editor();
    mEditor->setObject( object );
    
    mLayerManager = new LayerManager( mEditor );
    mLayerManager->setEditor( mEditor );
    mLayerManager->init();
}

void TestLayerManager::cleanupTestCase()
{
    delete mEditor;
}

void TestLayerManager::testNewLayerManager()
{
    QCOMPARE( mLayerManager->count(), 3 );
    QCOMPARE( mLayerManager->currentLayerIndex(), 0 );
}