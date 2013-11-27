
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
}

void TestLayer::testGetMaxFramePosition()
{
    LayerBitmap* pLayer = new LayerBitmap( m_pObject );

    QVERIFY( pLayer->addImageAtFrame( 3 ) );
    QCOMPARE( pLayer->getMaxFramePosition(), 3 );

    QVERIFY( pLayer->addImageAtFrame( 8 ) );
    QCOMPARE( pLayer->getMaxFramePosition(), 8 );

    QVERIFY( pLayer->addImageAtFrame( 100 ) );
    QCOMPARE( pLayer->getMaxFramePosition(), 100 );
}


