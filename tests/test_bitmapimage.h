#ifndef TESTBITMAPIMAGE_H
#define TESTBITMAPIMAGE_H

#include "AutoTest.h"

class TestBitmapImage : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void initImage();
    void testInitSize();
};

DECLARE_TEST( TestBitmapImage );

#endif // TESTBITMAPIMAGE_H