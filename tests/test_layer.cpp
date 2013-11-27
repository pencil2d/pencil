
#include "layer.h"
#include "layerbitmap.h"
#include "object.h"
#include "test_layer.h"

TestLayer::TestLayer()
{
}

void TestLayer::initTestCase()
{
    m_pObject = new Object();
    m_pObject->defaultInitialisation();
}

void TestLayer::cleanupTestCase()
{
    delete m_pObject;
}

void TestLayer::testCase1()
{
    QVERIFY2(true, "Failure");
}

void TestLayer::testLayerType()
{
    LayerBitmap* pLayer = new LayerBitmap( m_pObject );
    QVERIFY( pLayer->type() == Layer::BITMAP );

    delete pLayer;
}


void TestLayer::testBitmapLayerAddImage()
{
    LayerBitmap* pLayer = new LayerBitmap( m_pObject );

    bool bOK = false;

    bOK = pLayer->addImageAtFrame( 0 );
    QVERIFY2( bOK == false, "Frame Number must > 0." );

    bOK = pLayer->addImageAtFrame( 1 );
    QVERIFY2( bOK == false, "Already has a key frame at position 1." );

    bOK = pLayer->addImageAtFrame( 2 );
    QCOMPARE( bOK, true );
    QCOMPARE( pLayer->getMaxFramePosition(), 2 );

    delete pLayer;
}

void TestLayer::testGetMaxFramePosition()
{
    LayerBitmap* pLayer = new LayerBitmap( m_pObject );

    // 1 at beginning.
    QCOMPARE( pLayer->getMaxFramePosition(), 1 );

    QVERIFY( pLayer->addImageAtFrame( 3 ) );
    QCOMPARE( pLayer->getMaxFramePosition(), 3 );

    QVERIFY( pLayer->addImageAtFrame( 8 ) );
    QCOMPARE( pLayer->getMaxFramePosition(), 8 );

    QVERIFY( pLayer->addImageAtFrame( 100 ) );
    QCOMPARE( pLayer->getMaxFramePosition(), 100 );

    QVERIFY( pLayer->addImageAtFrame( 80 ) );
    QCOMPARE( pLayer->getMaxFramePosition(), 100 );

    delete pLayer;
}

void TestLayer::testHasKeyframeAtPosition()
{
    LayerBitmap* pLayer = new LayerBitmap( m_pObject );

    QCOMPARE( pLayer->hasKeyframeAtPosition( 1 ), true ); // there is a frame at 1 in default.

    QVERIFY( pLayer->addImageAtFrame( 15 ) );
    QCOMPARE( pLayer->hasKeyframeAtPosition( 15 ), true );
    QCOMPARE( pLayer->hasKeyframeAtPosition( 10 ), false );

    QVERIFY( pLayer->addImageAtFrame( 10 ) );
    QCOMPARE( pLayer->hasKeyframeAtPosition( 10 ), true );

    // test false case
    QCOMPARE( pLayer->hasKeyframeAtPosition( 0 ), false );
    QCOMPARE( pLayer->hasKeyframeAtPosition( 1000 ), false );
    QCOMPARE( pLayer->hasKeyframeAtPosition( -333 ), false );

    delete pLayer;
}

void TestLayer::testGetFramePositionAt()
{
    LayerBitmap* pLayer = new LayerBitmap( m_pObject );

    QCOMPARE( pLayer->getFramePositionAt( 0 ), 1 );
    //QVERIFY( pLayer )

    delete pLayer;
}


