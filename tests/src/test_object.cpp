/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "catch.hpp"

#include <memory>
#include <QDomDocument>
#include <QDomElement>
#include <QTemporaryDir>
#include "object.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"


TEST_CASE("Object::addXXXLayer()")
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

    SECTION("Add a vector layer")
    {
        REQUIRE(obj->getLayerCount() == 0);
        obj->addNewVectorLayer();
        REQUIRE(obj->getLayerCount() == 1);

        REQUIRE(obj->getLayer(0)->type() == Layer::VECTOR);
    }

    SECTION("Add a camera layer")
    {
        REQUIRE(obj->getLayerCount() == 0);
        obj->addNewCameraLayer();
        REQUIRE(obj->getLayerCount() == 1);

        REQUIRE(obj->getLayer(0)->type() == Layer::CAMERA);
    }

    SECTION("Add a sound layer")
    {
        REQUIRE(obj->getLayerCount() == 0);
        obj->addNewSoundLayer();
        REQUIRE(obj->getLayerCount() == 1);
        REQUIRE(obj->getLayer(0)->type() == Layer::SOUND);
    }

    SECTION("Add 3 layers")
    {
        REQUIRE(obj->getLayerCount() == 0);

        obj->addNewSoundLayer();
        REQUIRE(obj->getLayerCount() == 1);
        REQUIRE(obj->getLayer(0)->type() == Layer::SOUND);

        obj->addNewCameraLayer();
        REQUIRE(obj->getLayerCount() == 2);
        REQUIRE(obj->getLayer(1)->type() == Layer::CAMERA);

        obj->addNewBitmapLayer();
        REQUIRE(obj->getLayerCount() == 3);
        REQUIRE(obj->getLayer(2)->type() == Layer::BITMAP);
    }

    SECTION("Add 500 layers")
    {
        REQUIRE(obj->getLayerCount() == 0);
        for (int i = 0; i < 500; ++i)
        {
            obj->addNewBitmapLayer();
        }
        REQUIRE(obj->getLayerCount() == 500);
    }

    delete obj;
}



TEST_CASE("Object::getUniqueLayerID()")
{
    SECTION("getUniqueLayerID")
    {
        std::unique_ptr<Object> obj(new Object);

        Layer* bitmapLayer = obj->addNewBitmapLayer();
        REQUIRE(bitmapLayer->id() == 1);
        REQUIRE(obj->getUniqueLayerID() == 2);

        Layer* vectorLayer = obj->addNewVectorLayer();
        REQUIRE(vectorLayer->id() == 2);
        REQUIRE(obj->getUniqueLayerID() == 3);
    }
}

/*
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
