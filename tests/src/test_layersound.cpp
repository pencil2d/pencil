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

#include "layersound.h"
#include "soundclip.h"

#include <memory>
#include <QDir>
#include <QDomElement>
#include <QTemporaryDir>

TEST_CASE("Load sound layer from XML")
{
    std::unique_ptr<Layer> soundLayer(new LayerSound(1));
    QTemporaryDir dataDir;
    REQUIRE(dataDir.isValid());
    QDomDocument doc;
    doc.setContent(QString("<layer id='1' name='Sound Layer' visibility='1'></layer>"));
    QDomElement layerElem = doc.documentElement();
    ProgressCallback nullCallback = []() {};

    auto createFrame = [&layerElem, &doc, &dataDir](QString src = "sound_001.wav", int frame = 1)
    {
        QDomElement clipElem = doc.createElement("sound");
        clipElem.setAttribute("src", src);
        clipElem.setAttribute("frame", frame);
        layerElem.appendChild(clipElem);
        if (QDir(src).isAbsolute()) {
            src = QDir(dataDir.path()).relativeFilePath(src);
        }
        QFile soundFile(dataDir.filePath(src));
        soundFile.open(QIODevice::WriteOnly);
        soundFile.close();
    };

    SECTION("No clips")
    {
        soundLayer->loadDomElement(layerElem, dataDir.path(), []() {});

        REQUIRE(soundLayer->keyFrameCount() == 0);
    }

    SECTION("Single clip")
    {
        createFrame("sound_001.wav", 1);

        soundLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(soundLayer->keyFrameCount() == 1);
        SoundClip* frame = static_cast<SoundClip*>(soundLayer->getKeyFrameAt(1));
        REQUIRE(frame != nullptr);
        REQUIRE(QDir(frame->fileName()) == QDir(dataDir.filePath("sound_001.wav")));
    }

    SECTION("Multiple clips")
    {
        createFrame("sound_001.wav", 1);
        createFrame("sound_002.wav", 2);

        soundLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(soundLayer->keyFrameCount() == 2);
        for (int i = 1; i <= 2; i++)
        {
            SoundClip* frame = static_cast<SoundClip*>(soundLayer->getKeyFrameAt(i));
            REQUIRE(frame != nullptr);
            REQUIRE(QDir(frame->fileName()) == QDir(dataDir.filePath(QString("sound_%1.wav").arg(QString::number(i), 3, QChar('0')))));
        }
    }

    SECTION("Clip with absolute src")
    {
        createFrame(QDir(dataDir.filePath("sound_001.wav")).absolutePath());

        soundLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(soundLayer->keyFrameCount() == 0);
    }

    SECTION("Clip src outside of data dir")
    {
        QTemporaryDir otherDir;
        createFrame(QDir(dataDir.path()).relativeFilePath(QDir(otherDir.filePath("sound_001.wav")).absolutePath()));

        soundLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(soundLayer->keyFrameCount() == 0);
    }

    SECTION("Clip src nested in data dir")
    {
        REQUIRE(QDir(dataDir.path()).mkdir("subdir"));
        createFrame("subdir/sound_001.wav");

        soundLayer->loadDomElement(layerElem, dataDir.path(), nullCallback);

        REQUIRE(soundLayer->keyFrameCount() == 1);
        SoundClip* frame = static_cast<SoundClip*>(soundLayer->getKeyFrameAt(1));
        REQUIRE(frame != nullptr);
        REQUIRE(QDir(frame->fileName()) == QDir(dataDir.filePath("subdir/sound_001.wav")));
    }
}
