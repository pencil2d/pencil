#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include "AutoTest.h"
class Object;

class TestObject : public QObject
{
    Q_OBJECT

public:
    TestObject();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testCase1();
    void testAddBitmapLayer();
    void testAddVectorLayer();
    void testAddCameraLayer();
    void testAddSoundLayer();
    
    void testAddMoreLayers();

    void testLayerID();
    void testMoveLayer();

    void testLoadXML();

    void testExportColorPalette();

private:
};


DECLARE_TEST( TestObject )

#endif // TESTOBJECT_H
