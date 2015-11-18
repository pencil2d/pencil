#include "test_object.h"
#include <memory>
#include "object.h"


TestObject::TestObject()
{
}

void TestObject::initTestCase()
{
}

void TestObject::cleanupTestCase()
{
}

void TestObject::testCase1()
{
    std::unique_ptr< Object > obj( new Object );

    QCOMPARE( obj->getLayerCount(), 0 );
    obj->init();
    QCOMPARE( obj->getLayerCount(), 3 );
}

void TestObject::testAddBitmapLayer()
{
    std::unique_ptr< Object > obj( new Object );

    QCOMPARE( obj->getLayerCount(), 0 );
    obj->addNewBitmapLayer();
    QCOMPARE( obj->getLayerCount(), 1 );

    Layer* layer = obj->getLayer( 0 );
    QCOMPARE( layer->type(), Layer::BITMAP );
}

void TestObject::testAddVectorLayer()
{
    std::unique_ptr< Object > obj( new Object );
    
    QCOMPARE( obj->getLayerCount(), 0 );
    obj->addNewVectorLayer();
    QCOMPARE( obj->getLayerCount(), 1 );
    
    Layer* layer = obj->getLayer( 0 );
    QCOMPARE( layer->type(), Layer::VECTOR );
}

void TestObject::testAddCameraLayer()
{
    std::unique_ptr< Object > obj( new Object );
    
    QCOMPARE( obj->getLayerCount(), 0 );
    obj->addNewCameraLayer();
    QCOMPARE( obj->getLayerCount(), 1 );
    
    Layer* layer = obj->getLayer( 0 );
    QCOMPARE( layer->type(), Layer::CAMERA );
}

void TestObject::testAddSoundLayer()
{
    std::unique_ptr< Object > obj( new Object );
    
    QCOMPARE( obj->getLayerCount(), 0 );
    obj->addNewSoundLayer();
    QCOMPARE( obj->getLayerCount(), 1 );
    
    Layer* layer = obj->getLayer( 0 );
    QCOMPARE( layer->type(), Layer::SOUND );
}


void TestObject::testAddMoreLayers()
{
    std::unique_ptr< Object > obj( new Object );
    QCOMPARE( obj->getLayerCount(), 0 );

    obj->addNewSoundLayer();
    QCOMPARE( obj->getLayerCount(), 1 );
    QCOMPARE( obj->getLayer( 0 )->type(), Layer::SOUND );

    obj->addNewCameraLayer();
    QCOMPARE( obj->getLayerCount(), 2 );
    QCOMPARE( obj->getLayer( 1 )->type(), Layer::CAMERA );

    obj->addNewBitmapLayer();
    QCOMPARE( obj->getLayerCount(), 3 );
    QCOMPARE( obj->getLayer( 2 )->type(), Layer::BITMAP );
}
