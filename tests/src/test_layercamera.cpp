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

#include "layer.h"
#include "layercamera.h"
#include "camera.h"
#include "object.h"

#include "filemanager.h"

#include <QTemporaryDir>

SCENARIO("Create camera keyframe with linear easing")
{
    GIVEN("A Camera Layer with one keyframe")
    {
        Layer* layer = new LayerCamera(1);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        WHEN("Adding a keyframe")
        {
            REQUIRE(layer->addNewKeyFrameAt(1) == true);

            Camera* camera = camLayer->getCameraAtFrame(1);
            THEN("Camera easing is set to linear and path point hasn't been moved")
            {
                REQUIRE(camera->getEasingType() == CameraEasingType::LINEAR);
                REQUIRE_FALSE(camera->pathControlPointMoved());
                REQUIRE(camLayer->getCenteredPathPoint(1) == camera->translation());
            }
        }
    }
}

SCENARIO("Add a second keyframe and see that the path point of the first keyframe is updated")
{
    GIVEN("A Camera layer with multiple keyframes")
    {
        Layer* layer = new LayerCamera(1);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        layer->addNewKeyFrameAt(1);
        layer->addNewKeyFrameAt(5);

        WHEN("Transforming the second keyframe")
        {
            Camera* camera = camLayer->getCameraAtFrame(1);
            camera->translate(100, 100);
            THEN("The camera path mid point of the previous frame is updated to the center of the the path between the two keyframes")
            {
                Camera* camera2 = camLayer->getCameraAtFrame(5);
                QLineF line(camera->translation(), camera2->translation());
                REQUIRE_FALSE(camera->pathControlPointMoved());
                REQUIRE(-camLayer->getCenteredPathPoint(1) == line.pointAt(0.5));
            }
        }
    }
}

SCENARIO("Add keyframe after having interpolated the previous keyframe and see that the translation is kept")
{
    GIVEN("A Camera layer with multiple keyframes")
    {
        Layer* layer = new LayerCamera(1);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        layer->addNewKeyFrameAt(1);

        Camera* camera = camLayer->getCameraAtFrame(1);
        camera->translate(300, 100);
        WHEN("Adding a new keyframe after the previous frame was interpolated")
        {
            layer->addNewKeyFrameAt(5);
            THEN("The camera is placed at the interpolated position, not at 0,0")
            {
                Camera* camera2 = camLayer->getCameraAtFrame(5);
                REQUIRE(camera2->translation() == QPoint(300, 100));
            }
        }
    }
}

SCENARIO("Remove a camera keyframe and see that the path is properly reset")
{
    GIVEN("A Camera layer with multiple keyframes")
    {
        Layer* layer = new LayerCamera(1);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        layer->addNewKeyFrameAt(1);
        layer->addNewKeyFrameAt(5);

        Camera* camera = camLayer->getCameraAtFrame(1);
        REQUIRE(camera->translation() == -camLayer->getCenteredPathPoint(1));
        camera->translate(300, 100);

        WHEN("Removing the last keyframe with transformation applied")
        {
            REQUIRE(layer->removeKeyFrame(5) == true);
            THEN("The path of the previous keyframe is reset to the center of the camera")
            {
                REQUIRE_FALSE(camera->pathControlPointMoved());
                REQUIRE(camera->translation() == -camLayer->getCenteredPathPoint(1));
            }
        }
    }
}

SCENARIO("When deleting an in between keyframe, the previous keyframe will try to recover its initial control point, if possible")
{
    GIVEN("A Camera layer with multiple keyframes where the keys are added sequentially")
    {
        Layer* layer = new LayerCamera(1);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        layer->addNewKeyFrameAt(1);
        Camera* cameraFirst = camLayer->getCameraAtFrame(1);
        layer->addNewKeyFrameAt(5);
        Camera* cameraMiddle = camLayer->getCameraAtFrame(5);
        cameraMiddle->translate(500, 200);

        layer->addNewKeyFrameAt(25);
        Camera* cameraLast = camLayer->getCameraAtFrame(25);
        cameraLast->translate(200, 400);

        cameraMiddle->setPathControlPoint(QPointF(600, 350));

        WHEN("Removing the in-between keyframe")
        {
            REQUIRE(layer->removeKeyFrame(5) == true);
            THEN("The path of the previous keyframe is not set because it hasn't been modified")
            {
                REQUIRE_FALSE(cameraFirst->pathControlPointMoved());
            }
        }
    }

    GIVEN("A Camera layer with multiple keyframes where the third frame is added in-between")
    {
        Layer* layer = new LayerCamera(2);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        layer->addNewKeyFrameAt(1);
        Camera* cameraFirst = camLayer->getCameraAtFrame(1);

        layer->addNewKeyFrameAt(25);
        Camera* cameraLast = camLayer->getCameraAtFrame(25);
        cameraLast->translate(700, 200);

        QPointF initialControlPoint = QPointF(100, 200);
        cameraFirst->setPathControlPoint(initialControlPoint);
        cameraFirst->setPathControlPointMoved(true);

        layer->addNewKeyFrameAt(5);

        REQUIRE(cameraFirst->getPathControlPoint() != initialControlPoint);
        WHEN("Removing the in-between keyframe")
        {
            REQUIRE(layer->removeKeyFrame(5) == true);
            THEN("The control point of the previous keyframe will be recovered")
            {
                REQUIRE(cameraFirst->pathControlPointMoved());
                REQUIRE(cameraFirst->getPathControlPoint() == initialControlPoint);
            }
        }
    }
}

SCENARIO("When adding a keyframe in-between two othes where the control points has been modified, the curve will be preserved")
{
    GIVEN("A Camera layer with multiple keyframes where the third frame is added in-between")
    {
        Layer* layer = new LayerCamera(1);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        layer->addNewKeyFrameAt(1);
        Camera* cameraFirst = camLayer->getCameraAtFrame(1);

        layer->addNewKeyFrameAt(25);
        Camera* cameraLast = camLayer->getCameraAtFrame(25);
        cameraLast->translate(-700, -500);

        QPointF initialControlPoint = QPointF(100, 400);
        cameraFirst->setPathControlPoint(initialControlPoint);
        cameraFirst->setPathControlPointMoved(true);

        WHEN("Adding the third frame in-between two others that has had their control points moved")
        {
            REQUIRE(layer->addNewKeyFrameAt(16));
            Camera* cameraMiddle = camLayer->getCameraAtFrame(16);
            THEN("The curve has been preserved")
            {
                REQUIRE(cameraFirst->pathControlPointMoved());
                REQUIRE(cameraMiddle->pathControlPointMoved());
                REQUIRE(cameraFirst->getPathControlPoint() == QPointF(62.5, 250));
                REQUIRE(cameraMiddle->getPathControlPoint() == QPointF(475, 462.5));
            }
        }
    }
}

SCENARIO("Loading a project and see that all camera properties are set, if applicable")
{
    FileManager fileMan;
    QTemporaryDir tempDir;
    QString name = "camera-path-test.pclx";
    const QString path = tempDir.path() + "/" + name;
    QFile::copy(":/" + name, path);

    Object* object = fileMan.load(path);
    GIVEN("A Camera layer with 2 keyframes")
    {
        LayerCamera* cameraLayer = static_cast<LayerCamera*>(object->getLayer(0));
        WHEN("The project has loaded")
        {
            THEN("The keyframe(s) has been loaded properly")
            {
                // <camera s="1" r="0" pathCPY="-1317.8543267067691" frame="1" pathCPM="1" dx="0" pathCPX="91.250978402496912" dy="0" easing="9"/>
                Camera* cam = cameraLayer->getCameraAtFrame(1);
                REQUIRE(cam->translation() == QPointF(0, 0));
                REQUIRE(cam->rotation() == 0);
                REQUIRE(cam->scaling() == 1);
                REQUIRE(cam->getEasingType() == static_cast<CameraEasingType>(9));
                REQUIRE(cam->pathControlPointMoved() == true);

                // <camera s="1.7482683523433076" r="-89.931555642192322" pathCPY="-306.81323380187314" frame="20" pathCPM="0" dx="-1723.3735323962537" pathCPX="861.68676619812686" dy="613.62646760374628" easing="29"/>
                Camera* cam2 = cameraLayer->getCameraAtFrame(20);
                REQUIRE(cam2->translation() == QPointF(-1723.3735323962537, 613.62646760374628));
                REQUIRE(cam2->rotation() == -89.931555642192322);
                REQUIRE(cam2->scaling() == 1.7482683523433076);
                REQUIRE(cam2->getEasingType() == static_cast<CameraEasingType>(29));
                REQUIRE(cam2->pathControlPointMoved() == false);

            }
        }
    }

    delete object;
}

SCENARIO("Loading a project and see that control points are not moved")
{
    FileManager fileMan;
    QTemporaryDir tempDir;
    QString name = "camera-path-test-2.pclx";
    const QString path = tempDir.path() + "/" + name;
    QFile::copy(":/" + name, path);

    Object* object = fileMan.load(path);
    GIVEN("A Camera layer with 2 keyframes")
    {
        LayerCamera* cameraLayer = static_cast<LayerCamera*>(object->getLayer(0));
        WHEN("The project has loaded")
        {
            THEN("The camera has been moved but the control points are left untouched")
            {
                // <camera frame="1" dx="106.64557334043445" s="1" dy="-9.9980225006655701" r="0"/>
                Camera* cam = cameraLayer->getCameraAtFrame(1);
                REQUIRE(cam->translation() == QPointF(106.64557334043445, -9.9980225006655701));
                REQUIRE(cam->rotation() == 0);
                REQUIRE(cam->scaling() == 1);
                REQUIRE(cam->getEasingType() == static_cast<CameraEasingType>(0));
                REQUIRE(cam->pathControlPointMoved() == false);

                // <camera frame="18" dx="-1544.0704911892358" s="1" dy="-455.57351924588636" r="0"/>
                Camera* cam2 = cameraLayer->getCameraAtFrame(18);
                REQUIRE(cam2->translation() == QPointF(-1544.0704911892358, -455.57351924588636));
                REQUIRE(cam2->rotation() == 0);
                REQUIRE(cam2->scaling() == 1);
                REQUIRE(cam2->getEasingType() == static_cast<CameraEasingType>(0));
                REQUIRE(cam2->pathControlPointMoved() == false);

                QLineF line(cam->translation(), cam2->translation());

                // Because the control points haven't been touched, its position will be centered between the two frames
                REQUIRE(-cameraLayer->getCenteredPathPoint(1) == line.pointAt(0.5));

            }
        }
    }

    delete object;
}
