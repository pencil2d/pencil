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
#include "cameratool.h"

#include "filemanager.h"

#include <QTemporaryDir>

SCENARIO("Create camera keyframe with linear easing")
{
    Object* object = new Object;

    GIVEN("A Camera Layer with one keyframe")
    {
        Layer* layer = new LayerCamera(object);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        WHEN("Adding a keyframe")
        {
            REQUIRE(layer->addNewKeyFrameAt(1) == true);

            Camera* camera = camLayer->getCameraAtFrame(1);
            THEN("Camera easing is set to linear and path point is set to the center of the current frame")
            {
                REQUIRE(camera->getEasingType() == CameraEasingType::LINEAR);
                REQUIRE(camera->getPathControlPoint() == camera->translation());
            }
        }
    }

    delete object;
}

SCENARIO("Add a second keyframe and see that the path point of the first keyframe is updated")
{
    Object* object = new Object;

    GIVEN("A Camera layer with multiple keyframes")
    {
        Layer* layer = new LayerCamera(object);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        layer->addNewKeyFrameAt(1);
        layer->addNewKeyFrameAt(5);

        CameraTool tool(nullptr);

        WHEN("Transforming the second keyframe")
        {
            Camera* camera = camLayer->getCameraAtFrame(1);
            tool.transformView(camLayer, CameraMoveType::CENTER, camera->translation(), QPoint(300,300), 0, 5);
            THEN("The camera path mid point of the previous frame is updated to the center of the the path between the two keyframes")
            {
                Camera* camera2 = camLayer->getCameraAtFrame(5);
                QLineF line(camera->translation(), camera2->translation());
                REQUIRE(-camera->getPathControlPoint() == line.pointAt(0.5));
            }
        }
    }

    delete object;
}

SCENARIO("Add keyframe after having interpolated the previous keyframe and see that the translation is kept")
{
    Object* object = new Object;

    GIVEN("A Camera layer with multiple keyframes")
    {
        Layer* layer = new LayerCamera(object);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        CameraTool tool(nullptr);

        layer->addNewKeyFrameAt(1);

        Camera* camera = camLayer->getCameraAtFrame(1);
        tool.transformView(camLayer, CameraMoveType::CENTER, camera->translation(), QPoint(300,300), 0, 1);
        WHEN("Adding a new keyframe after the previous frame was interpolated")
        {
            layer->addNewKeyFrameAt(5);
            THEN("The camera is placed at the interpolated position, not at 0,0")
            {
                Camera* camera2 = camLayer->getCameraAtFrame(5);
                REQUIRE(camera2->translation() == QPoint(300, 300));
            }
        }
    }

    delete object;
}

SCENARIO("Remove a camera keyframe and see that the path is properly reset")
{
    Object* object = new Object;

    GIVEN("A Camera layer with multiple keyframes")
    {
        Layer* layer = new LayerCamera(object);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        CameraTool tool(nullptr);

        layer->addNewKeyFrameAt(1);
        layer->addNewKeyFrameAt(5);

        Camera* camera = camLayer->getCameraAtFrame(1);
        tool.transformView(camLayer, CameraMoveType::CENTER, camera->translation(), QPoint(300,300), 0, 5);
        REQUIRE(camera->translation() != camera->getPathControlPoint());

        WHEN("Removing the last keyframe with transformation applied")
        {
            REQUIRE(layer->removeKeyFrame(5) == true);
            THEN("The path of the previous keyframe is reset to the center of the camera")
            {
                REQUIRE(camera->translation() == camera->getPathControlPoint());
            }
        }
    }

    delete object;
}

SCENARIO("Loading a project and see that all camera properties are set")
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
                REQUIRE(cam->getPathControlPoint() == QPointF(91.250978402496912, -1317.8543267067691));
                REQUIRE(cam->pathControlPointMoved() == true);

                // <camera s="1.7482683523433076" r="-89.931555642192322" pathCPY="-306.81323380187314" frame="20" pathCPM="0" dx="-1723.3735323962537" pathCPX="861.68676619812686" dy="613.62646760374628" easing="29"/>
                Camera* cam2 = cameraLayer->getCameraAtFrame(20);
                REQUIRE(cam2->translation() == QPointF(-1723.3735323962537, 613.62646760374628));
                REQUIRE(cam2->rotation() == -89.931555642192322);
                REQUIRE(cam2->scaling() == 1.7482683523433076);
                REQUIRE(cam2->getEasingType() == static_cast<CameraEasingType>(29));
                REQUIRE(cam2->getPathControlPoint() == QPointF(861.68676619812686, -306.81323380187314));
                REQUIRE(cam2->pathControlPointMoved() == false);

            }
        }
    }

    delete object;
}
