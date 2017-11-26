
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
        object->createDefaultLayers(); // create default 3 layers
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
