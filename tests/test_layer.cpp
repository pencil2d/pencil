#include "test_layer.h"

#include <memory>
#include "catch.hpp"

#include "layer.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"
#include "layersound.h"
#include "object.h"
#include "util.h"


TEST_CASE("LayerType")
{
    Object* object = new Object;

    SECTION("Bitmap Layer")
    {
        Layer* bitmapLayer = new LayerBitmap(object);
        REQUIRE(bitmapLayer->type() == Layer::BITMAP);
        delete bitmapLayer;
    }
    SECTION("Vector Layer")
    {
        Layer* vecLayer = new LayerVector(object);
        REQUIRE(vecLayer->type() == Layer::VECTOR);
        delete vecLayer;
    }
    SECTION("Camera Layer")
    {
        Layer* cameraLayer = new LayerCamera(object);
        REQUIRE(cameraLayer->type() == Layer::CAMERA);
        delete cameraLayer;
    }
    SECTION("Sound Layer")
    {
        Layer* soundLayer = new LayerSound(object);
        REQUIRE(soundLayer->type() == Layer::SOUND);
        delete soundLayer;
    }

    delete object;
}

SCENARIO("Add key frames into a Layer", "[Layer]")
{
    Object* object = new Object;

    GIVEN("A Bitmap Layer")
    {
        Layer* layer = new LayerBitmap(object);

        REQUIRE(layer->addNewEmptyKeyAt(0) == false); // first key position is 1.
        REQUIRE(layer->keyFrameCount() == 0);

        WHEN("Add a keyframe")
        {
            REQUIRE(layer->addNewEmptyKeyAt(1) == true);

            THEN("keyframe can be found in the layer")
            {
                REQUIRE(layer->getMaxKeyFramePosition() == 1);
                REQUIRE(layer->keyFrameCount() == 1);
                REQUIRE(layer->getKeyFrameAt(1) != nullptr);
            }
        }

        WHEN("Add 2 keyframes")
        {
            REQUIRE(layer->addNewEmptyKeyAt(1) == true);
            REQUIRE(layer->addNewEmptyKeyAt(3) == true);

            THEN("2 keyframes are in the layer")
            {
                REQUIRE(layer->getMaxKeyFramePosition() == 3);
                REQUIRE(layer->getKeyFrameAt(3) != nullptr);
                REQUIRE(layer->keyFrameCount() == 2);
            }
        }
        delete layer;
    }

    GIVEN("A Vector Layer")
    {
        Layer* layer = new LayerVector(object);

        REQUIRE(layer->addNewEmptyKeyAt(0) == false); // first key position is 1.
        REQUIRE(layer->keyFrameCount() == 0);

        WHEN("Add a keyframe")
        {
            REQUIRE(layer->addNewEmptyKeyAt(1) == true);

            THEN("keyframe can be found in the layer")
            {
                REQUIRE(layer->getMaxKeyFramePosition() == 1);
                REQUIRE(layer->keyFrameCount() == 1);
                REQUIRE(layer->getKeyFrameAt(1) != nullptr);
            }
        }
        delete layer;
    }

    GIVEN("A Camera Layer")
    {
        Layer* layer = new LayerCamera(object);

        REQUIRE(layer->addNewEmptyKeyAt(0) == false); // first key position is 1.
        REQUIRE(layer->keyFrameCount() == 0);

        WHEN("Add a keyframe")
        {
            REQUIRE(layer->addNewEmptyKeyAt(1) == true);

            THEN("keyframe can be found in the layer")
            {
                REQUIRE(layer->getMaxKeyFramePosition() == 1);
                REQUIRE(layer->keyFrameCount() == 1);
                REQUIRE(layer->getKeyFrameAt(1) != nullptr);
            }
        }
        delete layer;
    }
    delete object;
}

TEST_CASE("Test Layer::keyExists()", "[Layer]")
{
    Object* obj = new Object;

    SECTION("Fresh new Layer")
    {
        Layer* layer = obj->addNewBitmapLayer();
        REQUIRE(layer->keyExists(1) == true); // there is a frame at 1 in default.

    }
    SECTION("Key exists at 15")
    {
        Layer* layer = obj->addNewBitmapLayer();
        REQUIRE(layer->addNewEmptyKeyAt(15));
        REQUIRE(layer->keyExists(16) == false);
        REQUIRE(layer->keyExists(15) == true);
        REQUIRE(layer->keyExists(14) == false);
        REQUIRE(layer->keyExists(13) == false);
        REQUIRE(layer->keyExists(12) == false);
    }
    SECTION("Key exists 10 20 30")
    {
        Layer* layer = obj->addNewBitmapLayer();

        REQUIRE(layer->addNewEmptyKeyAt(10));
        REQUIRE(layer->keyExists(10) == true);

        REQUIRE(layer->addNewEmptyKeyAt(20));
        REQUIRE(layer->keyExists(20) == true);

        REQUIRE(layer->addNewEmptyKeyAt(30));
        REQUIRE(layer->keyExists(30) == true);

        // test false case
        REQUIRE(layer->keyExists(0) == false);
        REQUIRE(layer->keyExists(21) == false);
        REQUIRE(layer->keyExists(31) == false);
        REQUIRE(layer->keyExists(1000) == false);
        REQUIRE(layer->keyExists(-333) == false);
    }

    delete obj;
}

TEST_CASE("Test Layer::firstKeyFramePosition()")
{
    Object* obj = new Object;
    
    SECTION("Key at 1")
    {        
        Layer* layer = obj->addNewBitmapLayer();
        REQUIRE(layer->firstKeyFramePosition() == 1);
        
        layer->addNewEmptyKeyAt(99);
        REQUIRE(layer->firstKeyFramePosition() == 1);

        layer->moveKeyFrameForward(1);
        REQUIRE(layer->firstKeyFramePosition() == 1); // always has a key at 1
    }
}

/*

void TestLayer::testGetFirstFramePosition()
{

}

void TestLayer::testGetMaxFramePosition()
{
    Layer* layer = m_pObject->addNewBitmapLayer();;
    OnScopeExit( m_pObject->deleteLayer( layer ) );

    // 1 at beginning.
    QCOMPARE( layer->getMaxKeyFramePosition(), 1 );

    QVERIFY( layer->addNewEmptyKeyAt( 3 ) );
    QCOMPARE( layer->getMaxKeyFramePosition(), 3 );

    QVERIFY( layer->addNewEmptyKeyAt( 8 ) );
    QCOMPARE( layer->getMaxKeyFramePosition(), 8 );

    QVERIFY( layer->addNewEmptyKeyAt( 100 ) );
    QCOMPARE( layer->getMaxKeyFramePosition(), 100 );

    QVERIFY( layer->addNewEmptyKeyAt( 80 ) );
    QCOMPARE( layer->getMaxKeyFramePosition(), 100 );
}

void TestLayer::testRemoveKeyFrame()
{
    Layer* layer = m_pObject->addNewBitmapLayer();;
    OnScopeExit( m_pObject->deleteLayer( layer ) );

    layer->removeKeyFrame( 1 );
    QCOMPARE( layer->getMaxKeyFramePosition(), 0 ); 

    for ( int i = 2; i <= 20; ++i )
    {
        QVERIFY( layer->addNewEmptyKeyAt( i ) );
    }

    QCOMPARE( layer->keyExists( 20 ), true );
    layer->removeKeyFrame( 20 );
    QCOMPARE( layer->keyExists( 20 ), false );

    QCOMPARE( layer->keyExists( 8 ), true );
    layer->removeKeyFrame( 8 );
    QCOMPARE( layer->keyExists( 8 ), false );

    QCOMPARE( layer->keyExists( 19 ), true );

    layer->removeKeyFrame( 19 );
    QCOMPARE( layer->getMaxKeyFramePosition(), 18 );

    layer->removeKeyFrame( 18 );
    QCOMPARE( layer->getMaxKeyFramePosition(), 17 );
}

void TestLayer::testPreviousKeyFramePosition()
{
    Layer* pLayer = m_pObject->addNewBitmapLayer();;
    OnScopeExit( m_pObject->deleteLayer( pLayer ) );

    QCOMPARE( pLayer->getPreviousKeyFramePosition( 1 ), 1 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 10 ), 1 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 100 ), 1 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 1000 ), 1 );

    pLayer->addNewEmptyKeyAt( 2 );
    pLayer->addNewEmptyKeyAt( 8 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 2 ), 1 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 8 ), 2 );

    QCOMPARE( pLayer->getPreviousKeyFramePosition( -5 ), 1 );

    pLayer->addNewEmptyKeyAt( 15 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 16 ), 15 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 17 ), 15 );

    pLayer->removeKeyFrame( 15 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 16 ), 8 );
}

void TestLayer::testNextKeyFramePosition()
{
    Layer* pLayer = m_pObject->addNewBitmapLayer();;
    OnScopeExit( m_pObject->deleteLayer( pLayer ) );

    QCOMPARE( pLayer->getNextKeyFramePosition( 1 ), 1 );
    QCOMPARE( pLayer->getNextKeyFramePosition( 10 ), 1 );
    QCOMPARE( pLayer->getNextKeyFramePosition( 100 ), 1 );

    pLayer->addNewEmptyKeyAt( 5 );
    QCOMPARE( pLayer->getNextKeyFramePosition( 1 ), 5 );
    QCOMPARE( pLayer->getNextKeyFramePosition( 2 ), 5 );
}
*/