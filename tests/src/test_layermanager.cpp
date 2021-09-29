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

#include "object.h"
#include "editor.h"
#include "layermanager.h"
#include "pencilerror.h"


TEST_CASE("LayerManager::init()")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    SECTION("Test initial state")
    {
        LayerManager* layerMgr = new LayerManager(editor);
        layerMgr->init();

        object->init();
        object->addNewCameraLayer();
        object->addNewVectorLayer();
        object->addNewBitmapLayer();
        REQUIRE(layerMgr->count() == 3);
        REQUIRE(layerMgr->currentLayerIndex() == 0);
        REQUIRE(layerMgr->getLayer(0)->type() == Layer::CAMERA);
        REQUIRE(layerMgr->getLayer(1)->type() == Layer::VECTOR);
        REQUIRE(layerMgr->getLayer(2)->type() == Layer::BITMAP);
    }
    delete editor;
}

TEST_CASE("LayerManager::deleteLayer()")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    SECTION("delete layers")
    {
        LayerManager* layerMgr = new LayerManager(editor);
        layerMgr->init();

        object->init();

        REQUIRE(layerMgr->count() == 0);
        layerMgr->createCameraLayer("Camera1");
        REQUIRE(layerMgr->count() == 1);
        layerMgr->createVectorLayer("Vector2");
        REQUIRE(layerMgr->count() == 2);
        layerMgr->createBitmapLayer("Bitmap3");
        REQUIRE(layerMgr->count() == 3);
        layerMgr->deleteLayer(2);
        REQUIRE(layerMgr->count() == 2);
        layerMgr->deleteLayer(1);
        REQUIRE(layerMgr->count() == 1);
    }

    SECTION("delete camera layers")
    {
        LayerManager* layerMgr = new LayerManager(editor);
        layerMgr->init();

        // create 2 camera layers
        REQUIRE(layerMgr->count() == 0);
        layerMgr->createCameraLayer("Camera1");
        REQUIRE(layerMgr->count() == 1);
        layerMgr->createCameraLayer("Camera2");
        REQUIRE(layerMgr->count() == 2);

        // delete one of them, ok.
        layerMgr->deleteLayer(1);
        REQUIRE(layerMgr->count() == 1);

        // delete the second, no, cant do it.
        Status st = layerMgr->deleteLayer(0);
        REQUIRE(layerMgr->count() == 1);
        REQUIRE((st == Status::ERROR_NEED_AT_LEAST_ONE_CAMERA_LAYER));
    }
    delete editor;
}

TEST_CASE("Layer::setCurrentLayer(index)") {
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    SECTION("Deselect previous layer") {
        LayerManager* layerMgr = new LayerManager(editor);
        layerMgr->init();

        layerMgr->createBitmapLayer("Bitmap1");
        layerMgr->createBitmapLayer("Bitmap2");
        layerMgr->setCurrentLayer(0);

        Layer* currentLayer = layerMgr->currentLayer();
        currentLayer->addNewKeyFrameAt(1);
        currentLayer->addNewKeyFrameAt(2);

        currentLayer->setFrameSelected(1, true);
        currentLayer->setFrameSelected(2, true);

        REQUIRE(currentLayer->selectedKeyFrameCount() == 2);

        layerMgr->setCurrentLayer(1);

        // Make sure that previous layer has deselected all frames
        REQUIRE(layerMgr->getLayer(0)->selectedKeyFrameCount() == 0);
        REQUIRE(layerMgr->getLayer(1)->selectedKeyFrameCount() == 0);

    }
    delete editor;
}
