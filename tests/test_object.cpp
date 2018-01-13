
#include "catch.hpp"

#include <memory>
#include <QDomDocument>
#include <QDomElement>
#include <QTemporaryDir>
#include "object.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"


TEST_CASE("Object")
{
    Object* obj = new Object;
    SECTION("Init an Object")
    {
        obj->init();
        REQUIRE(obj->getLayerCount() == 0);
        REQUIRE(obj->getColourCount() > 0);
    }

    SECTION("Add a bitmap layer")
    {
        REQUIRE(obj->getLayerCount() == 0);
        obj->addNewBitmapLayer();
        REQUIRE(obj->getLayerCount() == 1);

        REQUIRE(obj->getLayer(0)->type() == Layer::BITMAP);
    }
    delete obj;
}

/*

void TestObject::testAddVectorLayer()
{
    std::unique_ptr< Object > obj( new Object );
    
    QCOMPARE( obj->getLayerCount(), 0 );
    obj->addNewVectorLayer();
    QCOMPARE( obj->getLayerCount(), 1 );
    
    Layer* layer = obj->getLayer( 0 );
    QCOMPARE( layer->type(), Layer::VECTOR );
}

void TestObject::testAddCameraLayer()
{
    std::unique_ptr< Object > obj( new Object );
    
    QCOMPARE( obj->getLayerCount(), 0 );
    obj->addNewCameraLayer();
    QCOMPARE( obj->getLayerCount(), 1 );
    
    Layer* layer = obj->getLayer( 0 );
    QCOMPARE( layer->type(), Layer::CAMERA );
}

void TestObject::testAddSoundLayer()
{
    std::unique_ptr< Object > obj( new Object );
    
    QCOMPARE( obj->getLayerCount(), 0 );
    obj->addNewSoundLayer();
    QCOMPARE( obj->getLayerCount(), 1 );
    
    Layer* layer = obj->getLayer( 0 );
    QCOMPARE( layer->type(), Layer::SOUND );
}


void TestObject::testAddMoreLayers()
{
    std::unique_ptr< Object > obj( new Object );
    QCOMPARE( obj->getLayerCount(), 0 );

    obj->addNewSoundLayer();
    QCOMPARE( obj->getLayerCount(), 1 );
    QCOMPARE( obj->getLayer( 0 )->type(), Layer::SOUND );

    obj->addNewCameraLayer();
    QCOMPARE( obj->getLayerCount(), 2 );
    QCOMPARE( obj->getLayer( 1 )->type(), Layer::CAMERA );

    obj->addNewBitmapLayer();
    QCOMPARE( obj->getLayerCount(), 3 );
    QCOMPARE( obj->getLayer( 2 )->type(), Layer::BITMAP );
}

void TestObject::testLayerID()
{
    std::unique_ptr< Object > obj( new Object );

    Layer* bitmapLayer = obj->addNewBitmapLayer();
    QCOMPARE( bitmapLayer->id(), 1 );
    QCOMPARE( obj->getUniqueLayerID(), 2 );

    Layer* vectorLayer = obj->addNewVectorLayer();
    QCOMPARE( vectorLayer->id(), 2 );
    QCOMPARE( obj->getUniqueLayerID(), 3 );
}

void TestObject::testMoveLayer()
{
    std::unique_ptr< Object > obj( new Object );

    obj->addNewBitmapLayer();
    obj->addNewVectorLayer();
    QCOMPARE( obj->getLayer( 0 )->id(), 1 );
    QCOMPARE( obj->getLayer( 1 )->id(), 2 );

    obj->moveLayer( 0, 1 );
    QCOMPARE( obj->getLayer( 0 )->id(), 2 );
    QCOMPARE( obj->getLayer( 1 )->id(), 1 );

}

void TestObject::testLoadXML()
{
    std::unique_ptr< Object > obj( new Object );

    QString strXMLContent;
    QTextStream sout( &strXMLContent );
    sout << "<!DOCTYPE PencilDocument><object>";
    sout << "</object>";
    sout.flush();

    QDomDocument doc;
    doc.setContent( strXMLContent );
    QDomElement e = doc.firstChildElement( "object" );
    QVERIFY( !e.isNull() );

    QVERIFY( obj->loadXML( e ) );
    
}

void TestObject::testExportColorPalette()
{
    std::shared_ptr< Object > obj = std::make_shared<Object>();

    obj->addColour(ColourRef(QColor(255, 254, 253, 100), "TestColor1"));

    QTemporaryDir dir;
    if (dir.isValid())
    {
        QString sOutPath = dir.path() + "/testPalette.xml";
        QVERIFY(obj->exportPalette(sOutPath));
        QVERIFY(obj->importPalette(sOutPath));

        ColourRef c = obj->getColour(0);

        QVERIFY(c.name == "TestColor1");
        QCOMPARE(c.colour.red(), 255);
        QCOMPARE(c.colour.green(), 254);
        QCOMPARE(c.colour.blue(), 253);
        QCOMPARE(c.colour.alpha(), 100);
    }

}
*/
