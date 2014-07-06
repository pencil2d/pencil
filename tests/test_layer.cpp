#include "layer.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "object.h"
#include "test_layer.h"

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
    Layer* pBitmapLayer = new LayerBitmap( m_pObject );
    QScopedPointer<Layer> ptr( pBitmapLayer );

    QVERIFY( pBitmapLayer->type() == Layer::BITMAP );

    Layer* pVecLayer = new LayerVector( m_pObject );
    QScopedPointer<Layer> ptr2( pVecLayer );

    QVERIFY( pVecLayer->type() == Layer::VECTOR );
}


void TestLayer::testAddNewKeyAt( )
{
    Layer* pLayer = new LayerBitmap( m_pObject );
    QScopedPointer<Layer> ptr( pLayer );
    bool bOK = false;

    bOK = pLayer->addNewKeyAt( 0 );
    QVERIFY2( bOK == false, "Frame Number must > 0." );

    bOK = pLayer->addNewKeyAt( 1 );
    QVERIFY2( bOK == false, "Already has a key frame at position 1." );

    QCOMPARE( pLayer->addNewKeyAt( 2 ), true );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 2 );
}

void TestLayer::testHasKeyFrameAtPosition()
{
    Layer* pLayer = new LayerBitmap( m_pObject );
    QScopedPointer<Layer> ptr( pLayer );

    QCOMPARE( pLayer->keyExists( 1 ), true ); // there is a frame at 1 in default.

    QVERIFY( pLayer->addNewKeyAt( 15 ) );
	QCOMPARE( pLayer->keyExists( 15 ), true );
	QCOMPARE( pLayer->keyExists( 10 ), false );

    QVERIFY( pLayer->addNewKeyAt( 10 ) );
	QCOMPARE( pLayer->keyExists( 10 ), true );

    // test false case
	QCOMPARE( pLayer->keyExists( 0 ), false );
	QCOMPARE( pLayer->keyExists( 1000 ), false );
	QCOMPARE( pLayer->keyExists( -333 ), false );
}

void TestLayer::testGetFirstFramePosition()
{
    Layer* pLayer = new LayerBitmap( m_pObject );
    QScopedPointer<Layer> ptr( pLayer );

    QCOMPARE( pLayer->firstKeyFramePosition(), 1 );
    pLayer->addNewKeyAt( 99 );

    QCOMPARE( pLayer->firstKeyFramePosition(), 1 );
}


void TestLayer::testGetMaxFramePosition()
{
    Layer* pLayer = new LayerBitmap( m_pObject );

    // 1 at beginning.
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 1 );

    QVERIFY( pLayer->addNewKeyAt( 3 ) );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 3 );

    QVERIFY( pLayer->addNewKeyAt( 8 ) );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 8 );

    QVERIFY( pLayer->addNewKeyAt( 100 ) );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 100 );

    QVERIFY( pLayer->addNewKeyAt( 80 ) );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 100 );

    delete pLayer;
}

void TestLayer::testRemoveKeyFrame()
{
    Layer* pLayer = new LayerBitmap( m_pObject );

    pLayer->removeKeyFrame( 1 );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 1 ); // you can't delete the only 1 KeyFrame

    for ( int i = 2; i <= 20; ++i )
    {
        QVERIFY( pLayer->addNewKeyAt( i ) );
    }

	QCOMPARE( pLayer->keyExists( 20 ), true );
    pLayer->removeKeyFrame( 20 );
	QCOMPARE( pLayer->keyExists( 20 ), false );

	QCOMPARE( pLayer->keyExists( 8 ), true );
    pLayer->removeKeyFrame( 8 );
	QCOMPARE( pLayer->keyExists( 8 ), false );

	QCOMPARE( pLayer->keyExists( 19 ), true );

    pLayer->removeKeyFrame( 19 );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 18 );

    pLayer->removeKeyFrame( 18 );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 17 );

    delete pLayer;
}

void TestLayer::testPreviousKeyFramePosition()
{
    Layer* pLayer = new LayerBitmap( m_pObject );
    QScopedPointer<Layer> ptr( pLayer );

    QCOMPARE( pLayer->getPreviousKeyFramePosition( 1 ), 1 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 10 ), 1 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 100 ), 1 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 1000 ), 1 );

    pLayer->addNewKeyAt( 2 );
    pLayer->addNewKeyAt( 8 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 2 ), 1 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 8 ), 2 );

    QCOMPARE( pLayer->getPreviousKeyFramePosition( -5 ), 1 );

    pLayer->addNewKeyAt( 15 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 16 ), 15 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 17 ), 15 );

    pLayer->removeKeyFrame( 15 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 16 ), 8 );
}

void TestLayer::testNextKeyFramePosition()
{
    Layer* pLayer = new LayerBitmap( m_pObject );
    QScopedPointer<Layer> ptr( pLayer );

    QCOMPARE( pLayer->getNextKeyFramePosition( 1 ), 1 );
    QCOMPARE( pLayer->getNextKeyFramePosition( 10 ), 1 );
    QCOMPARE( pLayer->getNextKeyFramePosition( 100 ), 1 );

    pLayer->addNewKeyAt( 5 );
    QCOMPARE( pLayer->getNextKeyFramePosition( 1 ), 5 );
    QCOMPARE( pLayer->getNextKeyFramePosition( 2 ), 5 );
}
