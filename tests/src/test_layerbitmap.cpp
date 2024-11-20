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

#include "layerbitmap.h"
#include "bitmapimage.h"

#include <memory>
#include <QDir>
#include <QDomElement>
#include <QTemporaryDir>

TEST_CASE("Load bitmap layer from XML")
{
    std::unique_ptr<Layer> bitmapLayer(new LayerBitmap(1));
    QTemporaryDir dataDir;
    REQUIRE(dataDir.isValid());
    QDomDocument doc;
    doc.setContent(QString("<layer id='1' name='Bitmap Layer' visibility='1'></layer>"));
    QDomElement layerElem = doc.documentElement();
    ProgressCallback nullCallback = []() {};

    auto createFrame = [&layerElem, &doc](QString src = "001.001.png", int frame = 1, int topLeftX = 0, int topLeftY = 0)
    {
        QDomElement frameElem = doc.createElement("image");
        frameElem.setAttribute("src", src);
        frameElem.setAttribute("frame", frame);
        frameElem.setAttribute("topLeftX", topLeftX);
        frameElem.setAttribute("topLeftY", topLeftY);
        layerElem.appendChild(frameElem);
    };

    SECTION("No frames")
    {
        bitmapLayer->loadDomElement(layerElem, dataDir.path(), []() {});

        REQUIRE(bitmapLayer->keyFrameCount() == 0);
    }

    SECTION("Single frame")
    {
        createFrame("001.001.png", 1, 0, 0);

        bitmapLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(bitmapLayer->keyFrameCount() == 1);
        BitmapImage* frame = static_cast<BitmapImage*>(bitmapLayer->getKeyFrameAt(1));
        REQUIRE(frame != nullptr);
        REQUIRE(frame->top() == 0);
        REQUIRE(frame->left() == 0);
        REQUIRE(QDir(frame->fileName()) == QDir(dataDir.filePath("001.001.png")));
    }

    SECTION("Multiple frames")
    {
        createFrame("001.001.png", 1);
        createFrame("001.002.png", 2);

        bitmapLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(bitmapLayer->keyFrameCount() == 2);
        for (int i = 1; i <= 2; i++)
        {
            BitmapImage* frame = static_cast<BitmapImage*>(bitmapLayer->getKeyFrameAt(i));
            REQUIRE(frame != nullptr);
            REQUIRE(frame->top() == 0);
            REQUIRE(frame->left() == 0);
            REQUIRE(QDir(frame->fileName()) == QDir(dataDir.filePath(QString("001.%1.png").arg(QString::number(i), 3, QChar('0')))));
        }
    }

    SECTION("Frame with absolute src")
    {
        createFrame(QDir(dataDir.filePath("001.001.png")).absolutePath());

        bitmapLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(bitmapLayer->keyFrameCount() == 0);
    }

    SECTION("Frame src outside of data dir")
    {
        QTemporaryDir otherDir;
        createFrame(QDir(dataDir.path()).relativeFilePath(QDir(otherDir.filePath("001.001.png")).absolutePath()));

        bitmapLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(bitmapLayer->keyFrameCount() == 0);
    }

    SECTION("Frame src nested in data dir")
    {
        createFrame("subdir/001.001.png");

        bitmapLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(bitmapLayer->keyFrameCount() == 1);
        BitmapImage* frame = static_cast<BitmapImage*>(bitmapLayer->getKeyFrameAt(1));
        REQUIRE(frame != nullptr);
        REQUIRE(frame->top() == 0);
        REQUIRE(frame->left() == 0);
        REQUIRE(QDir(frame->fileName()) == QDir(dataDir.filePath("subdir/001.001.png")));
    }
}
