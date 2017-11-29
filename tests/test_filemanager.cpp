


#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QImage>
#include "fileformat.h"
#include "filemanager.h"
#include "util.h"
#include "object.h"
#include "catch.hpp"

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

        REQUIRE(pObj == NULL);
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

        REQUIRE(pObj == NULL);
        REQUIRE(fm.error().code() == Status::ERROR_INVALID_PENCIL_FILE);
    }
}

/*

void TestFileManager::testMinimalOldPencilDocument()
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
        OnScopeExit(delete o);

        QVERIFY(o != nullptr);
        QVERIFY(fm.error().ok());
        QVERIFY(o->getLayerCount() == 0);
    }
    else
    {
        QFAIL("Can't open temp file.");
    }
}

void TestFileManager::testOneLayerInFile()
{
    QTemporaryFile tmpFile;
    if (!tmpFile.open())
    {
        QFAIL("temp file");
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
    OnScopeExit(delete obj);

    QVERIFY(obj->getLayerCount() == 1);
}

void TestFileManager::testBitmapLayer()
{
    QTemporaryFile tmpFile;
    if (!tmpFile.open())
    {
        QFAIL("temp file");
    }
    QFile theXML(tmpFile.fileName());
    theXML.open(QIODevice::WriteOnly);

    QTextStream fout(&theXML);
    fout << "<!DOCTYPE PencilDocument><document>";
    fout << "  <object>";
    fout << "    <layer name='MyLayer' id='5' visibility='1' type='1' >";
    fout << "      <image frame='1' topLeftY='0' src='003.001.png' topLeftX='0' />";
    fout << "    </layer>";
    fout << "  </object>";
    fout << "</document>";
    theXML.close();

    FileManager fm;
    Object* obj = fm.load(theXML.fileName());
    OnScopeExit(delete obj);

    Layer* layer = obj->getLayer(0);
    QVERIFY2(layer->name() == "MyLayer", "LayerName is different");
    QCOMPARE(layer->id(), 5);
    QCOMPARE(layer->visible(), true);
    QCOMPARE(layer->type(), Layer::BITMAP);

    QVERIFY(layer->getKeyFrameAt(1) != nullptr);

}

void TestFileManager::testBitmapLayer2()
{
    QTemporaryFile tmpFile;
    if (!tmpFile.open())
    {
        QFAIL("temp file");
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
    OnScopeExit(delete obj);

    Layer* layer = obj->getLayer(0);
    QVERIFY2(layer->name() == "MyLayer", "LayerName is different");
    QCOMPARE(layer->id(), 5);
    QCOMPARE(layer->visible(), true);
    QCOMPARE(layer->type(), Layer::BITMAP);

    QVERIFY(layer->getKeyFrameAt(1) != nullptr);
}

void TestFileManager::testGeneratePCLX()
{
    QTemporaryDir testDir("PENCIL_TEST_XXXXXXXX");
    if (!testDir.isValid())
    {
        QFAIL("bad.");
    }

    QString strMainXMLPath = testDir.path() + "/" + PFF_XML_FILE_NAME;

    QFile theXML(strMainXMLPath);
    theXML.open(QIODevice::WriteOnly);

    QTextStream fout(&theXML);
    fout << "<!DOCTYPE PencilDocument><document>";
    fout << "  <object>";
    fout << "    <layer name='MyLayer' id='5' visibility='1' type='1' >";
    fout << "      <image frame='1' topLeftY='0' src='003.001.png' topLeftX='0' />";
    fout << "    </layer>";
    fout << "  </object>";
    fout << "</document>";
    theXML.close();

    QDir dir(testDir.path());
    if (dir.mkdir(PFF_DATA_DIR))
    {
        dir.cd(PFF_DATA_DIR);
    }
    QImage img(10, 10, QImage::Format_ARGB32_Premultiplied);
    img.save(dir.path() + "/003.001.png");

    QTemporaryFile tmpPCLX("PENCIL_TEST_XXXXXXXX.pclx");
    tmpPCLX.open();

    bool ok = JlCompress::compressDir(tmpPCLX.fileName(), testDir.path());
    QVERIFY(ok);
}

void TestFileManager::testLoadPCLX()
{
    QTemporaryDir testDir("PENCIL_TEST_XXXXXXXX");
    if (!testDir.isValid())
    {
        QFAIL("bad.");
    }

    QString strMainXMLPath = testDir.path() + "/" + PFF_XML_FILE_NAME;

    QFile theXML(strMainXMLPath);
    theXML.open(QIODevice::WriteOnly);

    QTextStream fout(&theXML);
    fout << "<!DOCTYPE PencilDocument><document>";
    fout << "  <object>";
    fout << "    <layer name='MyBitmapLayer' id='5' visibility='1' type='1' >";
    fout << "      <image frame='1' topLeftY='0' src='005.001.png' topLeftX='0' />";
    fout << "    </layer>";
    fout << "  </object>";
    fout << "</document>";
    theXML.close();

    QDir dir(testDir.path());
    if (dir.mkdir(PFF_DATA_DIR))
    {
        dir.cd(PFF_DATA_DIR);
    }
    QImage img(10, 10, QImage::Format_ARGB32_Premultiplied);
    img.save(dir.path() + "/005.001.png");

    QTemporaryFile tmpPCLX("PENCIL_TEST_XXXXXXXX.pclx");
    tmpPCLX.open();

    JlCompress::compressDir(tmpPCLX.fileName(), testDir.path());

    FileManager fm;
    Object* o = fm.load(tmpPCLX.fileName());

    QVERIFY(fm.error().ok());

    Layer* layer = o->getLayer(0);
    QVERIFY(layer->name() == "MyBitmapLayer");
    QVERIFY(layer->id() == 5);
}

*/
