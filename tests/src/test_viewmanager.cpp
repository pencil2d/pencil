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

#include "viewmanager.h"
#include "editor.h"
#include "object.h"
#include "camera.h"
#include "layercamera.h"


TEST_CASE("ViewManager")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    SECTION("init")
    {
        ViewManager* viewMgr = new ViewManager(editor);
        REQUIRE(viewMgr->getView() == QTransform());
        REQUIRE(viewMgr->getView().isIdentity());
    }
    delete editor;
}

TEST_CASE("ViewManager::translation")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    SECTION("translate(x, y) = (10, 20)")
    {
        ViewManager v(editor);
        v.init();
        
        v.translate(10, 20);
        REQUIRE(v.mapCanvasToScreen(QPointF(0, 0)) == QPointF(10, 20));
    }

    SECTION("translate twice")
    {
        ViewManager v(editor);
        v.init();

        v.translate(0, 15);
        REQUIRE(v.mapCanvasToScreen(QPointF(0, 0)) == QPointF(0, 15));

        v.translate(0, 30);
        REQUIRE(v.mapCanvasToScreen(QPointF(0, 0)) == QPointF(0, 30));
    }

    delete editor;
}

TEST_CASE("ViewManager::rotate()")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    SECTION("Rotate 90 degrees")
    {
        ViewManager v(editor);
        v.init();

        v.rotate(90.f); // counter-clockwise rotation
        REQUIRE(v.rotation() == 90.f);
        REQUIRE(v.mapCanvasToScreen(QPointF(1, 0)) == QPointF(0, 1));
        REQUIRE(v.mapCanvasToScreen(QPointF(23.6, 0)) == QPointF(0, 23.6));
    }

    SECTION("Rotate 180 degrees")
    {
        ViewManager v(editor);
        v.init();

        v.rotate(180.f); // counter-clockwise rotation
        REQUIRE(v.rotation() == 180.f);
        REQUIRE(v.mapCanvasToScreen(QPointF(1, 0)) == QPointF(-1, 0));
        REQUIRE(v.mapCanvasToScreen(QPointF(92.1, 0)) == QPointF(-92.1, 0));
    }

    SECTION("Rotate twice")
    {
        ViewManager v(editor);
        v.init();

        v.rotate(45.f); // counter-clockwise rotation
        REQUIRE(v.rotation() == 45.f);
        REQUIRE(v.mapCanvasToScreen(QPointF(1, 0)) == QPointF(cos(M_PI / 4), sin(M_PI / 4)));

        v.rotate(90);
        REQUIRE(v.rotation() == 90.f);
        REQUIRE(v.mapCanvasToScreen(QPointF(1, 0)) == QPointF(0, 1));
    }

    delete editor;
}

TEST_CASE("ViewManager::scale")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    SECTION("Scale 2.0")
    {
        ViewManager v(editor);
        v.init();

        v.scale(2);
        REQUIRE(v.scaling() == 2.0f);
        REQUIRE(v.mapCanvasToScreen(QPointF(1, 1)) == QPointF(2, 2));
    }

    SECTION("Scale twice")
    {
        ViewManager v(editor);
        v.init();

        v.scale(2);
        REQUIRE(v.scaling() == 2.f);
        REQUIRE(v.mapCanvasToScreen(QPointF(1, 1)) == QPointF(2, 2));

        v.scale(6);
        REQUIRE(v.scaling() == 6.f);
        REQUIRE(v.mapCanvasToScreen(QPointF(1, 1)) == QPointF(6, 6));
        REQUIRE(v.mapCanvasToScreen(QPointF(-1, 2)) == QPointF(-6, 12));
    }

    SECTION("Scale over the maximum value")
    {
        ViewManager v(editor);
        v.init();

        // set a ridiculously large number
        // should be clamp to 100.f, the maximum scaling value
        v.scale(10000);
        REQUIRE(v.mapCanvasToScreen(QPointF(1, 1)) == QPointF(100, 100));
    }

    SECTION("Scale over the minimum value")
    {
        ViewManager v(editor);
        v.init();

        v.scale(0.00005f);

        QPointF p1 = v.mapCanvasToScreen(QPointF(1, 1));
        QPointF p2 = QPointF(0.01, 0.01);

        REQUIRE(std::abs(p1.x() - p2.x()) < 0.000001);
        REQUIRE(std::abs(p1.y() - p2.y()) < 0.000001);
    }

    delete editor;
}

TEST_CASE("ViewManager mixed translations")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    SECTION("translate & rotate")
    {
        ViewManager v(editor);
        v.init();

        v.translate(10, 0);
        v.rotate(90);
        // translation is always applied first
        // (1, 1) => translate 10 units on x => (11, 1)
        // (11, 1) => rotate 90 deg => (-1, 11)
        REQUIRE(v.mapCanvasToScreen(QPointF(1, 1)) == QPointF(-1, 11));
    }

    delete editor;
}

TEST_CASE("ViewManager::ResetView()")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    SECTION("Reset view matrix")
    {
        ViewManager v(editor);
        v.init();

        v.translate(15, 25);
        v.scale(3.25);

        v.resetView(); // Reset! view matrix will be identity.

        REQUIRE(v.getView().isIdentity());
        REQUIRE(v.mapCanvasToScreen(QPointF(10, 10)) == QPointF(10, 10));
        REQUIRE(v.mapScreenToCanvas(QPointF(99, 10)) == QPointF(99, 10));
    }
    delete editor;
}

TEST_CASE("ViewManager with camera layers")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    SECTION("Empty Camera Layer")
    {
        ViewManager v(editor);
        v.init();

        LayerCamera* layerCam = editor->object()->addNewCameraLayer();
        REQUIRE(layerCam != nullptr);

        Camera* k = static_cast<Camera*>(layerCam->getKeyFrameAt(1));
        k->translate(100, 0);
        v.setCameraLayer(layerCam);

        REQUIRE(k->getView() == v.getView());
        REQUIRE(v.translation() == QPointF(100, 0));

        editor->object()->deleteLayer(0);
    }

    SECTION("Camera Layer with 2 keys")
    {
        ViewManager v(editor);
        v.init();

        // a default key at frame 0
        // 2nd key at frame 10
        LayerCamera* layerCam = editor->object()->addNewCameraLayer();
        layerCam->addKeyFrame(10, new Camera(QPointF(100, 0), 0, 1));

        v.setCameraLayer(layerCam);
        editor->scrubTo(10);

        // get the view matrix from camera layer at frame 10
        QTransform t = v.getView();
        REQUIRE(t.dx() == 100.0);
        REQUIRE(t.dy() == 0);
        REQUIRE(v.mapCanvasToScreen(QPointF(1, 5)) == QPointF(101, 5));

        editor->object()->deleteLayer(0);
    }

    delete editor;
}

/*
void TestViewManager::testCameraLayerWithTwoKeys()
{
    
}

void TestViewManager::testSetCameraLayerAndRemoveIt()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();

    v.translate(0, 100);

    // set a camera layer and then remove it
    LayerCamera* layerCam = mEditor->object()->getLayersByType<LayerCamera>()[0];
    auto k = static_cast<Camera*>(layerCam->getKeyFrameAt(1));
    k->translate(100, 0);

    v.setCameraLayer(layerCam);
    v.setCameraLayer(nullptr);

    REQUIRE(v.translation(), QPointF(0, 100));
}

void TestViewManager::testCanvasSize()
{
	auto v = std::make_shared<ViewManager>();
	v->setCanvasSize( QSize( 100, 200 ) );

	QTransform t = v->getView();

	REQUIRE( t.dx(), 50.0 );
	REQUIRE( t.dy(), 100.0 );
	REQUIRE( t.isRotating(), false );
	REQUIRE( t.isScaling(), false );
}

void TestViewManager::testLoadViewFromObject1()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();
	
    v.setCanvasSize( QSize( 100, 100 ) );
	QTransform t = v.getView();
	REQUIRE( t.dx(), 50.0 );
	REQUIRE( t.dy(), 50.0 );
}

void TestViewManager::testLoadViewFromObject2()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();

	v.setCanvasSize( QSize( 100, 100 ) );
	//QTransform t0;
    //mEditor->object()->data()->setCurrentView( t0.translate( 50.0, 80.0 ) );
	v.load(mEditor->object());

	QTransform t1 = v.getView();
	REQUIRE( t1.dx(), 50.0 );
	REQUIRE( t1.dy(), 50.0 ); // center of canvas
}

void TestViewManager::testSetCameraKey()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();
	v.setCanvasSize( QSize( 100, 100 ) );

	// add a keyframe into camera layer whenever view change.  
	auto camLayer = mEditor->object()->getLayersByType<LayerCamera>()[ 0 ];
	v.setCameraLayer( camLayer );
	v.translate( 20, 20 );

	QTransform t0 = v.getView();

	Camera* c = dynamic_cast<Camera*>( camLayer->getKeyFrameAt( 1 ) );

    QTransform canvasShift = QTransform::fromTranslate(50, 50);
	REQUIRE( t0, c->view * canvasShift);
}
*/