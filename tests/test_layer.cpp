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


void TestLayer::testAddImageAtFrame()
{
    LayerImage* pLayer = new LayerBitmap( m_pObject );

    bool bOK = false;

    bOK = pLayer->addNewKeyFrameAt( 0 );
    QVERIFY2( bOK == false, "Frame Number must > 0." );

    bOK = pLayer->addNewKeyFrameAt( 1 );
    QVERIFY2( bOK == false, "Already has a key frame at position 1." );

    bOK = pLayer->addNewKeyFrameAt( 2 );
    QCOMPARE( bOK, true );
    QCOMPARE( pLayer->getMaxFramePosition(), 2 );

    delete pLayer;
}

void TestLayer::testGetMaxFramePosition()
{
    LayerImage* pLayer = new LayerBitmap( m_pObject );

    // 1 at beginning.
    QCOMPARE( pLayer->getMaxFramePosition(), 1 );

    QVERIFY( pLayer->addNewKeyFrameAt( 3 ) );
    QCOMPARE( pLayer->getMaxFramePosition(), 3 );

    QVERIFY( pLayer->addNewKeyFrameAt( 8 ) );
    QCOMPARE( pLayer->getMaxFramePosition(), 8 );

    QVERIFY( pLayer->addNewKeyFrameAt( 100 ) );
    QCOMPARE( pLayer->getMaxFramePosition(), 100 );

    QVERIFY( pLayer->addNewKeyFrameAt( 80 ) );
    QCOMPARE( pLayer->getMaxFramePosition(), 100 );

    delete pLayer;
}

void TestLayer::testHasKeyFrameAtPosition()
{
    LayerImage* pLayer = new LayerBitmap( m_pObject );

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

    delete pLayer;
}

void TestLayer::testGetFramePositionAt()
{
    LayerImage* pLayer = new LayerBitmap( m_pObject );

    QCOMPARE( pLayer->getFramePositionAt( 0 ), 1 );

    QVERIFY( pLayer->addNewKeyFrameAt( 2 ) ); // (1, 2)
    QCOMPARE( pLayer->getFramePositionAt( 1 ), 2 );

    QVERIFY( pLayer->addNewKeyFrameAt( 4 ) ); // (1, 2, 4)
    QCOMPARE( pLayer->getFramePositionAt( 2 ), 4 );

    QVERIFY( pLayer->addNewKeyFrameAt( 3 ) ); // (1, 2, 3, 4)
    QCOMPARE( pLayer->getFramePositionAt( 0 ), 1 );
    QCOMPARE( pLayer->getFramePositionAt( 1 ), 2 );
    QCOMPARE( pLayer->getFramePositionAt( 2 ), 3 );
    QCOMPARE( pLayer->getFramePositionAt( 3 ), 4 );

    delete pLayer;
}

void TestLayer::testRemoveImageAtFrame()
{
    LayerImage* pLayer = new LayerBitmap( m_pObject );

    pLayer->removeKeyFrame( 1 );
    QCOMPARE( pLayer->getMaxFramePosition(), 1 ); // you can't delete the only 1 KeyFrame
    
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
    QCOMPARE( pLayer->getMaxFramePosition(), 18 );

    pLayer->removeKeyFrame( 18 );
    QCOMPARE( pLayer->getMaxFramePosition(), 17 );

    delete pLayer;
}
