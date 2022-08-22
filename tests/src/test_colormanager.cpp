/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "catch.hpp"

#include "colormanager.h"
#include "object.h"
#include "editor.h"
#include "layer.h"
#include "layermanager.h"

TEST_CASE("ColorManager Initial Test")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    SECTION("init")
    {
        ColorManager* cm = new ColorManager(editor);
        REQUIRE(cm != nullptr);
        REQUIRE(cm->init() == true);
    }

    delete editor;
}

TEST_CASE("ColorManager set color tests")
{
    Object* object = new Object;

    Editor* editor = new Editor;
    editor->setObject(object);
    ColorManager* cm = new ColorManager(editor);
    cm->init();

    Object* layerObj = new Object;
    Editor* layerEdit = new Editor;
    editor->setObject(object);
    LayerManager* layerMgr = new LayerManager(layerEdit);
    layerMgr->init();

    layerObj->init();
    // 0 = camera, 1 = vector, 2 = bitmap
    layerObj->addNewCameraLayer();
    layerObj->addNewVectorLayer();
    layerObj->addNewBitmapLayer();


    SECTION("setColor non vector layer")
    {
        cm->workingLayerChanged(layerObj->getLayer(2));
        cm->setFrontColor(QColor(255,0,0));
        REQUIRE(cm->frontColor() == QColor(255,0,0));
    }

    SECTION("setColor vector layer")
    {
        object->addColorAtIndex(0, QColor(255,255,255));
        cm->workingLayerChanged(layerObj->getLayer(1));
        cm->setIndexedColor(QColor(255,255,255));
        REQUIRE(cm->frontColor() == QColor(255,255,255));
    }


    delete layerEdit;
    delete editor;
}

TEST_CASE("Save and Load")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);
    ColorManager* cm2 = new ColorManager(editor);
    cm2->init();

    Object* objToLoad = new Object;
    objToLoad->data()->setCurrentColor(QColor(255,255,0));

    Object* objToSave = new Object;
    objToSave->data()->setCurrentColor(QColor(45,45,255));

    SECTION("load")
    {
        cm2->setFrontColor(QColor(0,0,0));
        REQUIRE(cm2->frontColor() == QColor(0,0,0));
        REQUIRE(cm2->load(objToLoad) == Status::OK);
        REQUIRE(cm2->frontColor() == QColor(255,255,0));
    }

    SECTION("save")
    {
        cm2->setFrontColor(QColor(0,255,0));
        REQUIRE(cm2->frontColor() == QColor(0,255,0));
        REQUIRE(objToSave->data()->getCurrentColor() == QColor(45,45,255));
        REQUIRE(cm2->save(objToSave) == Status::OK);
        REQUIRE(objToSave->data()->getCurrentColor() == QColor(0,255,0));

    }

    delete objToSave;
    delete objToLoad;
    delete editor;

}

TEST_CASE("colorManager setColorNumber")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);
    ColorManager* cm3 = new ColorManager(editor);
    cm3->init();

    SECTION("set number")
    {
        cm3->setColorNumber(0);
        REQUIRE(cm3->frontColorNumber() == 0);
        cm3->setColorNumber(4);
        REQUIRE(cm3->frontColorNumber() == 4);
        cm3->setColorNumber(10000);
        REQUIRE(cm3->frontColorNumber() == 10000);
    }
    delete editor;
}
