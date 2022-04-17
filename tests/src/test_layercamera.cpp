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
#include "util.h"

#include "filemanager.h"

#include <QDebug>
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

            Camera* camera = static_cast<Camera*>(camLayer->getKeyFrameAt(1));
            THEN("Camera easing is set to linear and path midpoint is set to the center of the current frame")
            {
                REQUIRE(camera->getEasingType() == CameraEasingType::LINEAR);
                REQUIRE(camera->getPathMidPoint() == camera->translation());
            }
        }
    }

    delete object;
}

SCENARIO("Add a second keyframe and see that the midpoint of the first keyframe is updated")
{
    Object* object = new Object;

    GIVEN("A Camera layer with multiple keyframes")
    {
        Layer* layer = new LayerCamera(object);
        LayerCamera* camLayer = static_cast<LayerCamera*>(layer);

        layer->addNewKeyFrameAt(1);
        layer->addNewKeyFrameAt(5);

        WHEN("Transforming the second keyframe")
        {
            Camera* camera = static_cast<Camera*>(camLayer->getKeyFrameAt(1));
            camLayer->transformCameraView(MoveMode::CENTER, camera->translation(), QPoint(300,300), 5);
            THEN("The camera path mid point of the previous frame is updated to the center of the the path between the two keyframes")
            {
                Camera* camera2 = static_cast<Camera*>(camLayer->getKeyFrameAt(5));
                QLineF line(camera->translation(), camera2->translation());
                REQUIRE(-camera->getPathMidPoint() == line.pointAt(0.5));
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

        layer->addNewKeyFrameAt(1);
        layer->addNewKeyFrameAt(5);

        Camera* camera = static_cast<Camera*>(camLayer->getKeyFrameAt(1));
        camLayer->transformCameraView(MoveMode::CENTER, camera->translation(), QPoint(300,300), 5);
        REQUIRE(camera->translation() != camera->getPathMidPoint());

        WHEN("Removing the last keyframe with transformation applied")
        {
            REQUIRE(layer->removeKeyFrame(5) == true);
            THEN("The path of the previous keyframe is reset to the center of the camera")
            {
                REQUIRE(camera->translation() == camera->getPathMidPoint());
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
    GIVEN("A Camera layer with 2 keyframe")
    {
        Layer* cameraLayer = static_cast<LayerCamera*>(object->getLayer(0));
        WHEN("The project has loaded")
        {
            THEN("The keyframe(s) has been loaded properly")
            {
                // <camera r="0" easing="9" midx="887" frame="1" dx="0" s="1" dy="0" midy="-281.5"/>
                Camera* cam = static_cast<Camera*>(cameraLayer->getKeyFrameAt(1));
                REQUIRE(cam->translation() == QPointF(0, 0));
                REQUIRE(cam->rotation() == 0);
                REQUIRE(cam->scaling() == 1);
                REQUIRE(cam->getEasingType() == static_cast<CameraEasingType>(9));
                REQUIRE(cam->getPathMidPoint() == QPointF(887, -281.5));

                // <camera r="-89.931555642192322" easing="29" midx="0" frame="20" dx="-1774" s="1.7482683523433076" dy="563" midy="0"/>
                Camera* cam2 = static_cast<Camera*>(cameraLayer->getKeyFrameAt(20));
                REQUIRE(cam2->translation() == QPointF(-1774, 563));
                REQUIRE(cam2->rotation() == -89.931555642192322);
                REQUIRE(cam2->scaling() == 1.7482683523433076);
                REQUIRE(cam2->getEasingType() == static_cast<CameraEasingType>(29));
                REQUIRE(cam2->getPathMidPoint() == QPointF(0, 0));

            }
        }
    }

    delete object;
}
