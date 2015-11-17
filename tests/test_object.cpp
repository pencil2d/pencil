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

