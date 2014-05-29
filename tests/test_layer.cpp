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


void TestLayer::testAddNewKeyFrameAt()
{
    Layer* pLayer = new LayerBitmap( m_pObject );
    QScopedPointer<Layer> ptr( pLayer );
    bool bOK = false;

    bOK = pLayer->addNewKeyFrameAt( 0 );
    QVERIFY2( bOK == false, "Frame Number must > 0." );

    bOK = pLayer->addNewKeyFrameAt( 1 );
    QVERIFY2( bOK == false, "Already has a key frame at position 1." );

    QCOMPARE( pLayer->addNewKeyFrameAt( 2 ), true );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 2 );
}

void TestLayer::testHasKeyFrameAtPosition()
{
    Layer* pLayer = new LayerBitmap( m_pObject );
    QScopedPointer<Layer> ptr( pLayer );

    QCOMPARE( pLayer->hasKeyFrameAtPosition( 1 ), true ); // there is a frame at 1 in default.

    QVERIFY( pLayer->addNewKeyFrameAt( 15 ) );
    QCOMPARE( pLayer->hasKeyFrameAtPosition( 15 ), true );
    QCOMPARE( pLayer->hasKeyFrameAtPosition( 10 ), false );

    QVERIFY( pLayer->addNewKeyFrameAt( 10 ) );
    QCOMPARE( pLayer->hasKeyFrameAtPosition( 10 ), true );

    // test false case
    QCOMPARE( pLayer->hasKeyFrameAtPosition( 0 ), false );
    QCOMPARE( pLayer->hasKeyFrameAtPosition( 1000 ), false );
    QCOMPARE( pLayer->hasKeyFrameAtPosition( -333 ), false );
}

void TestLayer::testGetFirstFramePosition()
{
    Layer* pLayer = new LayerBitmap( m_pObject );
    QScopedPointer<Layer> ptr( pLayer );

    QCOMPARE( pLayer->getFirstKeyFramePosition(), 1 );
    pLayer->addNewKeyFrameAt( 99 );

    QCOMPARE( pLayer->getFirstKeyFramePosition(), 1 );
}


void TestLayer::testGetMaxFramePosition()
{
    Layer* pLayer = new LayerBitmap( m_pObject );

    // 1 at beginning.
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 1 );

    QVERIFY( pLayer->addNewKeyFrameAt( 3 ) );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 3 );

    QVERIFY( pLayer->addNewKeyFrameAt( 8 ) );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 8 );

    QVERIFY( pLayer->addNewKeyFrameAt( 100 ) );
    QCOMPARE( pLayer->getMaxKeyFramePosition(), 100 );

    QVERIFY( pLayer->addNewKeyFrameAt( 80 ) );
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
        QVERIFY( pLayer->addNewKeyFrameAt( i ) );
    }

    QCOMPARE( pLayer->hasKeyFrameAtPosition( 20 ), true );
    pLayer->removeKeyFrame( 20 );
    QCOMPARE( pLayer->hasKeyFrameAtPosition( 20 ), false );

    QCOMPARE( pLayer->hasKeyFrameAtPosition( 8 ), true );
    pLayer->removeKeyFrame( 8 );
    QCOMPARE( pLayer->hasKeyFrameAtPosition( 8 ), false );

    QCOMPARE( pLayer->hasKeyFrameAtPosition( 19 ), true );

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

    pLayer->addNewKeyFrameAt( 2 );
    pLayer->addNewKeyFrameAt( 8 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 2 ), 1 );
    QCOMPARE( pLayer->getPreviousKeyFramePosition( 8 ), 2 );

    QCOMPARE( pLayer->getPreviousKeyFramePosition( -5 ), 1 );

    pLayer->addNewKeyFrameAt( 15 );
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

    pLayer->addNewKeyFrameAt( 5 );
    QCOMPARE( pLayer->getNextKeyFramePosition( 1 ), 5 );
    QCOMPARE( pLayer->getNextKeyFramePosition( 2 ), 5 );
}
