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

void TestLayerManager::testEmptyLayer()
{
    QCOMPARE( mLayerManager->count(), 0 );
}