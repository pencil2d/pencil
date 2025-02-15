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

#include "layervector.h"
#include "vectorimage.h"


#include <memory>
#include <QDir>
#include <QDomElement>
#include <QTemporaryDir>
#include <QTextStream>

TEST_CASE("Load vector layer from XML")
{
    std::unique_ptr<Layer> vectorLayer(new LayerVector(1));
    QTemporaryDir dataDir;
    REQUIRE(dataDir.isValid());
    QDomDocument doc;
    doc.setContent(QString("<layer id='1' name='Vector Layer' visibility='1'></layer>"));
    QDomElement layerElem = doc.documentElement();
    ProgressCallback nullCallback = []() {};

    QFile vecFile(dataDir.filePath("temp.vec"));
    vecFile.open(QIODevice::WriteOnly);

    QTextStream fout(&vecFile);
    fout << "<!DOCTYPE PencilVectorImage>";
    fout << "<image type='vector'/>";
    vecFile.close();

    auto createFrame = [&layerElem, &doc, &dataDir, &vecFile](QString src = "001.001.vec", int frame = 1)
    {
        QDomElement frameElem = doc.createElement("image");
        frameElem.setAttribute("src", src);
        frameElem.setAttribute("frame", frame);
        layerElem.appendChild(frameElem);
        if (QDir(src).isAbsolute()) {
            src = QDir(dataDir.path()).relativeFilePath(src);
        }
        vecFile.copy(dataDir.filePath(src));
    };

    SECTION("No frames")
    {
        vectorLayer->loadDomElement(layerElem, dataDir.path(), []() {});

        REQUIRE(vectorLayer->keyFrameCount() == 0);
    }

    SECTION("Single frame")
    {
        createFrame("001.001.vec", 1);

        vectorLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(vectorLayer->keyFrameCount() == 1);
        VectorImage* frame = static_cast<VectorImage*>(vectorLayer->getKeyFrameAt(1));
        REQUIRE(frame != nullptr);
        REQUIRE(QDir(frame->fileName()) == QDir(dataDir.filePath("001.001.vec")));
    }

    SECTION("Multiple frames")
    {
        createFrame("001.001.vec", 1);
        createFrame("001.002.vec", 2);

        vectorLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(vectorLayer->keyFrameCount() == 2);
        for (int i = 1; i <= 2; i++)
        {
            VectorImage* frame = static_cast<VectorImage*>(vectorLayer->getKeyFrameAt(i));
            REQUIRE(frame != nullptr);
            REQUIRE(QDir(frame->fileName()) == QDir(dataDir.filePath(QString("001.%1.vec").arg(QString::number(i), 3, QChar('0')))));
        }
    }

    SECTION("Frame with absolute src")
    {
        createFrame(QDir(dataDir.filePath("001.001.vec")).absolutePath());

        vectorLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(vectorLayer->keyFrameCount() == 0);
    }

    SECTION("Frame src outside of data dir")
    {
        QTemporaryDir otherDir;
        createFrame(QDir(dataDir.path()).relativeFilePath(QDir(otherDir.filePath("001.001.vec")).absolutePath()));

        vectorLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(vectorLayer->keyFrameCount() == 0);
    }

    SECTION("Frame src nested in data dir")
    {
        REQUIRE(QDir(dataDir.path()).mkdir("subdir"));
        createFrame("subdir/001.001.vec");

        vectorLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(vectorLayer->keyFrameCount() == 1);
        VectorImage* frame = static_cast<VectorImage*>(vectorLayer->getKeyFrameAt(1));
        REQUIRE(frame != nullptr);
        REQUIRE(QDir(frame->fileName()) == QDir(dataDir.filePath("subdir/001.001.vec")));
    }
}
