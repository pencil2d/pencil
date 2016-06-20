#include "test_bitmapimage.h"
#include "bitmapimage.h"

void TestBitmapImage::initTestCase()
{

}

void TestBitmapImage::cleanupTestCase()
{

}

void TestBitmapImage::initImage()
{
    BitmapImage* b = new BitmapImage;
    std::shared_ptr< BitmapImage > sp( b );
}


void TestBitmapImage::testInitSize()
{
    BitmapImage* b = new BitmapImage;
    std::shared_ptr< BitmapImage > sp( b );

    QCOMPARE( b->width(), 0 );
}
