#include "test_bitmapimage.h"
#include "bitmapimage.h"

void TestBitmapImage::initTestCase()
{

}

void TestBitmapImage::cleanupTestCase()
{

}

void TestBitmapImage::testInitImage()
{
    BitmapImage* b = new BitmapImage;
    std::shared_ptr< BitmapImage > sp(b);
}

void TestBitmapImage::testInitSize()
{
    BitmapImage* b = new BitmapImage;
    std::shared_ptr< BitmapImage > sp(b);

    QCOMPARE(b->width(), 0);
    QCOMPARE(b->height(), 0);
    QCOMPARE(b->top(), 0);
    QCOMPARE(b->left(), 0);
}

void TestBitmapImage::testInitWithColorAndBoundary()
{
    BitmapImage* b = new BitmapImage(QRect(10, 20, 30, 40), Qt::red);
    std::shared_ptr< BitmapImage > sp(b);

    QCOMPARE(b->left(), 10);
    QCOMPARE(b->top(), 20);
    QCOMPARE(b->width(), 30);
    QCOMPARE(b->height(), 40);
}
