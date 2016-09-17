#include "test_layer.h"

#include "layer.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"
#include "layersound.h"
#include "object.h"
#include "util.h"
#include <memory>

TestLayer::TestLayer()
{
}

void TestLayer::initTestCase()
{
    m_pObject = new Object();
    m_pObject->init();
}

void TestLayer::cleanupTestCase()
{
    delete m_pObject;
}

void TestLayer::testCase1()
{
    QVERIFY2( true, "Failure" );
}

void TestLayer::testLayerType()
{
    std::unique_ptr< Layer > bitmapLayer( new LayerBitmap( m_pObject ) );
    QVERIFY( bitmapLayer->type() == Layer::BITMAP );

    std::unique_ptr< Layer > vecLayer( new LayerVector( m_pObject ) );
    QVERIFY( vecLayer->type() == Layer::VECTOR );

    std::unique_ptr< Layer > cameraLayer( new LayerCamera( m_pObject ) );
    QVERIFY( cameraLayer->type() == Layer::CAMERA );

    std::unique_ptr< Layer > soundLayer( new LayerSound( m_pObject ) );
    QVERIFY( soundLayer->type() == Layer::SOUND );
}

void TestLayer::testAddNewKeyAtBitmap()
{
    std::unique_ptr< Layer > spLayer( new LayerBitmap( m_pObject ) );
    
    bool bOK = false;

    bOK = spLayer->addNewEmptyKeyAt( 0 );
    QVERIFY2( bOK == false, "Frame Number must > 0." );

    QVERIFY( spLayer->addNewEmptyKeyAt( 1 ) );

    QCOMPARE( spLayer->addNewEmptyKeyAt( 2 ), true );
    QCOMPARE( spLayer->getMaxKeyFramePosition(), 2 );
}

void TestLayer::testAddNewKeyAtVector()
{
    Layer* layer = m_pObject->addNewVectorLayer();;
    OnScopeExit( m_pObject->deleteLayer( layer ) );

    bool bOK = false;

    bOK = layer->addNewEmptyKeyAt( 0 );
    QVERIFY2( bOK == false, "Frame Number must > 0." );

    QVERIFY( layer->addNewEmptyKeyAt( 1 ) == false );
    
    QCOMPARE( layer->addNewEmptyKeyAt( 2 ), true );
    QCOMPARE( layer->getMaxKeyFramePosition(), 2 );
}

void TestLayer::testHasKeyFrameAtPosition()
{
    Layer* layer = m_pObject->addNewBitmapLayer();;
    OnScopeExit( m_pObject->deleteLayer( layer ) );

    QCOMPARE( layer->keyExists( 1 ), true ); // there is a frame at 1 in default.

    QVERIFY( layer->addNewEmptyKeyAt( 15 ) );
    QCOMPARE( layer->keyExists( 15 ), true );
    QCOMPARE( layer->keyExists( 10 ), false );

    QVERIFY( layer->addNewEmptyKeyAt( 10 ) );
    QCOMPARE( layer->keyExists( 10 ), true );

    // test false case
    QCOMPARE( layer->keyExists( 0 ), false );
    QCOMPARE( layer->keyExists( 1000 ), false );
    QCOMPARE( layer->keyExists( -333 ), false );
}

void TestLayer::testGetFirstFramePosition()
{
    Layer* layer = m_pObject->addNewBitmapLayer();;
    OnScopeExit( m_pObject->deleteLayer( layer ) );

    QCOMPARE( layer->firstKeyFramePosition(), 1 );
    layer->addNewEmptyKeyAt( 99 );

    QCOMPARE( layer->firstKeyFramePosition(), 1 );
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
