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

#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QImage>
#include "qminiz.h"
#include "fileformat.h"
#include "filemanager.h"
#include "util.h"
#include "object.h"
#include "bitmapimage.h"
#include "layerbitmap.h"


TEST_CASE("FileManager Initial Test")
{
    SECTION("Initial error code")
    {
        FileManager f;
        REQUIRE(f.error() == Status::OK);
    }
}

TEST_CASE("FileManager invalid operations")
{
    SECTION("Open a non-existing file")
    {
        FileManager fm;

        QString strDummyPath = "hahaha_blala.pcl";
        Object* obj = fm.load(strDummyPath);

        REQUIRE(obj == nullptr);
        REQUIRE(fm.error().code() == Status::FILE_NOT_FOUND);
    }

    SECTION("Bad XML")
    {
        QString strBadXMLPath = QDir::tempPath() + "/bad.pcl";

        // make a fake xml.
        QFile badXMLFile(strBadXMLPath);
        badXMLFile.open(QIODevice::WriteOnly);

        QTextStream fout(&badXMLFile);
        fout << "%% haha, this is not a xml file.";
        badXMLFile.close();

        FileManager fm;
        Object* pObj = fm.load(strBadXMLPath);

        REQUIRE(pObj == nullptr);
        REQUIRE(fm.error().code() == Status::ERROR_INVALID_XML_FILE);
    }

    SECTION("A valid xml, but doesn't follow pencil2d's rule")
    {
        QString strBadXMLPath = QDir::tempPath() + "/bad.pcl";

        QFile badXMLFile(strBadXMLPath);
        badXMLFile.open(QIODevice::WriteOnly);

        QTextStream fout(&badXMLFile);
        fout << "<!DOCTYPE BlahBlahRoot><document></document>";
        badXMLFile.close();

        FileManager fm;
        Object* pObj = fm.load(strBadXMLPath);

        REQUIRE(pObj == nullptr);
        REQUIRE(fm.error().code() == Status::ERROR_INVALID_PENCIL_FILE);
    }
}

TEST_CASE("FileManager Loading XML Tests")
{
    SECTION("Minimal working xml")
    {
        QTemporaryFile minimalDoc;
        if (minimalDoc.open())
        {
            QFile minXML(minimalDoc.fileName());
            minXML.open(QIODevice::WriteOnly);

            QTextStream fout(&minXML);
            fout << "<!DOCTYPE PencilDocument><document>";
            fout << "  <object></object>";
            fout << "</document>";
            minXML.close();

            FileManager fm;
            Object* o = fm.load(minimalDoc.fileName());

            REQUIRE(o != nullptr);
            REQUIRE(fm.error().ok());
            REQUIRE(o->getLayerCount() == 1); // have at least one cam layer

            delete o;
        }
    }

    SECTION("Xml with one bitmap layer")
    {
        QTemporaryFile tmpFile;
        if (!tmpFile.open())
        {
            REQUIRE(false);
        }
        QFile theXML(tmpFile.fileName());
        theXML.open(QIODevice::WriteOnly);

        QTextStream fout(&theXML);
        fout << "<!DOCTYPE PencilDocument><document>";
        fout << "  <object>";
        fout << "    <layer name='MyLayer' id='5' visibility='1' type='1'></layer>";
        fout << "  </object>";
        fout << "</document>";
        theXML.close();

        FileManager fm;
        Object* obj = fm.load(theXML.fileName());
        REQUIRE(obj->getLayerCount() == 2); // one bitmap layer and one default cam layer
        REQUIRE(obj->getLayer(0)->name() == "MyLayer");
        REQUIRE(obj->getLayer(0)->id() == 5);
        REQUIRE(obj->getLayer(0)->visible() == true);
        REQUIRE(obj->getLayer(0)->type() == Layer::BITMAP);

        delete obj;
    }

    SECTION("xml with one bitmap layer and one key")
    {
        QTemporaryFile tmpFile;
        if (!tmpFile.open())
        {
            REQUIRE(false);
        }
        QFile theXML(tmpFile.fileName());
        theXML.open(QIODevice::WriteOnly);

        QTextStream fout(&theXML);
        fout << "<!DOCTYPE PencilDocument><document>";
        fout << "  <object>";
        fout << "    <layer name='GoodLayer' id='5' visibility='0' type='1' >";
        fout << "      <image frame='1' topLeftY='0' src='003.001.png' topLeftX='0' />";
        fout << "    </layer>";
        fout << "  </object>";
        fout << "</document>";
        theXML.close();

        FileManager fm;
        Object* obj = fm.load(theXML.fileName());

        Layer* layer = obj->getLayer(0);
        REQUIRE(layer->name() == "GoodLayer");
        REQUIRE(layer->id() == 5);
        REQUIRE(layer->visible() == false);
        REQUIRE(layer->type() == Layer::BITMAP);

        REQUIRE(layer->getKeyFrameAt(1) != nullptr);
        REQUIRE(layer->keyFrameCount() == 1);

        delete obj;
    }

    SECTION("xml with 1 bitmap layer and 2 keys")
    {
        QTemporaryFile tmpFile;
        if (!tmpFile.open())
        {
            REQUIRE(false);
        }
        QFile theXML(tmpFile.fileName());
        theXML.open(QIODevice::WriteOnly);

        QTextStream fout(&theXML);
        fout << "<!DOCTYPE PencilDocument><document>";
        fout << "  <object>";
        fout << "    <layer name='MyLayer' id='5' visibility='1' type='1' >";
        fout << "      <image frame='1' topLeftY='0' src='003.001.png' topLeftX='0' />";
        fout << "      <image frame='2' topLeftY='0' src='003.001.png' topLeftX='0' />";
        fout << "    </layer>";
        fout << "  </object>";
        fout << "</document>";
        theXML.close();

        FileManager fm;
        Object* obj = fm.load(theXML.fileName());

        Layer* layer = obj->getLayer(0);
        REQUIRE(layer->name() == "MyLayer");
        REQUIRE(layer->id() == 5);
        REQUIRE(layer->visible() == true);
        REQUIRE(layer->type() == Layer::BITMAP);

        REQUIRE(layer->getKeyFrameAt(1) != nullptr);
        delete obj;
    }
}

// Turn a Qt resource file into an actual file on disk
QString QtResourceToFile(QString rscPath, QString filename, QTemporaryDir& tempDir)
{
    QFile fin(rscPath);
    if (!fin.open(QFile::ReadOnly))
    {
        qWarning() << __FUNCTION__ << "Cannot open" << rscPath;
        return "";
    }
    QByteArray content = fin.readAll();
    fin.close();

    QString filePathOnDisk = tempDir.filePath(filename);
    QFile fout(filePathOnDisk);
    if (!fout.open(QFile::WriteOnly))
    {
        qWarning() << __FUNCTION__ << "Cannot write to" << filePathOnDisk;
    }
    fout.write(content);
    fout.close();
    return filePathOnDisk;
}

TEST_CASE("FileManager Load PCLX")
{
    SECTION("Empty PCLX")
    {
        QTemporaryDir tempDir;

        FileManager fm;
        Object* o = fm.load(QtResourceToFile(":/empty.pclx", "empty.pclx", tempDir));
        REQUIRE(o != nullptr);
        if (o)
        {
            // file has 2 bitmap layers, 1 vector layers and 1 cam layers
            REQUIRE(o->getLayerCount() == 4);
        }
        delete o;
    }

    SECTION("Chinese Filename")
    {
        QTemporaryDir tempDir;

        FileManager fm;
        Object* o = fm.load(QtResourceToFile(":/cjk-test.pclx", "許功蓋.pclx", tempDir));
        REQUIRE(o != nullptr);
        if (o)
        {
            // file has 2 bitmap layers, 1 vector layers and 1 cam layers
            REQUIRE(o->getLayerCount() == 4);
        }
        delete o;
    }

    SECTION("Japanese Filename")
    {
        QTemporaryDir tempDir;

        FileManager fm;
        Object* o = fm.load(QtResourceToFile(":/cjk-test.pclx", "構わない.pclx", tempDir));
        REQUIRE(o != nullptr);
        if (o)
        {
            // file has 2 bitmap layers, 1 vector layers and 1 cam layers
            REQUIRE(o->getLayerCount() == 4);
        }
        delete o;
    }

    SECTION("Korean Filename")
    {
        QTemporaryDir tempDir;

        FileManager fm;
        Object* o = fm.load(QtResourceToFile(":/cjk-test.pclx", "대박이야.pclx", tempDir));
        REQUIRE(o != nullptr);
        if (o)
        {
            // file has 2 bitmap layers, 1 vector layers and 1 cam layers
            REQUIRE(o->getLayerCount() == 4);
        }
        delete o;
    }
}

TEST_CASE("FileManager File-saving")
{
    // https://github.com/pencil2d/pencil/issues/939
    SECTION("#939 Clear action not properly saved")
    {
        FileManager fm;

        // 1. create a animation with one red frame & save it
        Object* o1 = new Object;
        o1->init();
        o1->addNewCameraLayer();
        o1->addNewVectorLayer();
        o1->addNewBitmapLayer();

        LayerBitmap* layer = dynamic_cast<LayerBitmap*>(o1->getLayer(2));
        REQUIRE(layer->addNewKeyFrameAt(2));

        BitmapImage* b1 = layer->getBitmapImageAtFrame(2);
        b1->drawRect(QRectF(0, 0, 10, 10), QPen(QColor(255, 0, 0)), QBrush(Qt::red), QPainter::CompositionMode_SourceOver, false);

        QTemporaryDir testDir("PENCIL_TEST_XXXXXXXX");
        QString animationPath = testDir.path() + "/abc.pclx";
        fm.save(o1, animationPath);
        delete o1;

        // 2. load the animation, and then clear the red frame, save it.
        Object* o2 = fm.load(animationPath);
        layer = dynamic_cast<LayerBitmap*>(o2->getLayer(2));

        BitmapImage* b2 = layer->getBitmapImageAtFrame(2);
        b2->clear();

        fm.save(o2, animationPath);
        delete o2;

        // 3. load the animation again, check whether it's an empty frame
        Object* o3 = fm.load(animationPath);
        layer = dynamic_cast<LayerBitmap*>(o3->getLayer(2));

        BitmapImage* b3 = layer->getBitmapImageAtFrame(2);
        REQUIRE(b3->bounds().isEmpty());

        delete o3;
    }

    //https://github.com/pencil2d/pencil/issues/966
    SECTION("#966 Moving more than 200 frames corrupts frames upon save")
    {
        FileManager fm;

        // 1. Create a animation with 150 frames & save it
        Object* o1 = new Object;
        o1->init();
        o1->addNewCameraLayer();
        o1->addNewVectorLayer();
        o1->addNewBitmapLayer();

        LayerBitmap* layer = dynamic_cast<LayerBitmap*>(o1->getLayer(2));
        for (int i = 100; i < 150; ++i)
        {
            layer->addNewKeyFrameAt(i);
            auto bitmap = layer->getBitmapImageAtFrame(i);
            bitmap->drawRect(QRectF(0, 0, 10, 10), QPen(QColor(255, 0, 0)), QBrush(Qt::red), QPainter::CompositionMode_SourceOver, false);
        }

        QTemporaryDir testDir("PENCIL_TEST_XXXXXXXX");
        QString animationPath = testDir.path() + "/abc.pclx";
        fm.save(o1, animationPath);
        delete o1;

        // 2. Load the animation back and then make some frames unloaded by active frame pool
        Object* o2 = fm.load(animationPath);
        o2->setActiveFramePoolSize(20);

        layer = dynamic_cast<LayerBitmap*>(o2->getLayer(2));
        for (int i = 1; i < 150; ++i)
            o2->updateActiveFrames(i);

        // 3. Move those unloaded frames around
        for (int i = 100; i < 150; ++i)
            layer->setFrameSelected(i, true);

        layer->moveSelectedFrames(-55);
        fm.save(o2, animationPath);
        delete o2;

        // 4. Check no lost frames
        Object* o3 = fm.load(animationPath);
        layer = dynamic_cast<LayerBitmap*>(o3->getLayer(2));
        for (int i = 2; i < 150; ++i)
        {
            auto bitmap = layer->getBitmapImageAtFrame(i);
            if (bitmap)
            {
                REQUIRE(bitmap->image()->width() > 1);
                REQUIRE(bitmap->image()->height() > 1);
            }
        }
        delete o3;
    }
}

TEST_CASE("Empty Sound Frames")
{
    SECTION("Invalid src value")
    {

        QTemporaryFile soundFrameDoc;
        if (soundFrameDoc.open())
        {
            QFile newXML(soundFrameDoc.fileName());
            newXML.open(QIODevice::WriteOnly);

            QTextStream fout(&newXML);
            fout << "<!DOCTYPE PencilDocument><document>";
            fout << "  <object>";
            fout << "       <layer type='4' id='5' name='GoodLayer' visibility='1'>";
            fout << "           <sound frame='1' name='' src=''/>";
            fout << "       </layer>";
            fout << "  </object>";
            fout << "</document>";
            newXML.close();


            FileManager fm;
            Object* newObj = fm.load(soundFrameDoc.fileName());

            REQUIRE(newObj != nullptr);
            REQUIRE(fm.error().ok());
            REQUIRE(newObj->getLayerCount() == 2);
            REQUIRE(newObj->getLayer(0)->type() == 4);
            REQUIRE(newObj->getLayer(0)->id() == 5);
            REQUIRE(newObj->getLayer(0)->name() == "GoodLayer");
            REQUIRE(newObj->getLayer(0)->visible() == true);
            REQUIRE(newObj->getLayer(0)->getKeyFrameAt(1) == nullptr);

            delete newObj;
        }
    }
}
