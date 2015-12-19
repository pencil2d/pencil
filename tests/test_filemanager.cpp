
#include "test_filemanager.h"

#include <QTemporaryFile>
#include <QScopedPointer>
#include "filemanager.h"
#include "util.h"
#include "object.h"

typedef std::shared_ptr< FileManager > FileManagerPtr;


void TestObjectSaveLoader::testCase1()
{
    FileManagerPtr fm = std::make_shared< FileManager >();
    QVERIFY( fm->error() == Status::OK );
}

void TestObjectSaveLoader::testNotExistFile()
{
    FileManager fm;

    QString strDummyPath = "hahaha_blala.pcl";
    Object* obj = fm.load( strDummyPath );

    QVERIFY2( obj == NULL, "File doesn't exist.");
    QVERIFY2( fm.error().code() == Status::FILE_NOT_FOUND, "" );
}

void TestObjectSaveLoader::testInvalidXML()
{
    QString strBadXMLPath = QDir::tempPath() + "/bad.pcl";

    // make a fake xml.
    QFile badXMLFile( strBadXMLPath );
    badXMLFile.open( QIODevice::WriteOnly );

    QTextStream fout( &badXMLFile );
    fout << "%% haha, this is not a xml file.";
    badXMLFile.close();

    FileManager fm;
    Object* pObj = fm.load( strBadXMLPath );

    QVERIFY( pObj == NULL );
    QVERIFY( fm.error().code() == Status::ERROR_INVALID_XML_FILE );
}

void TestObjectSaveLoader::testInvalidPencilDocument()
{
    QString strBadXMLPath = QDir::tempPath() + "/bad.pcl";

    QFile badXMLFile( strBadXMLPath );
    badXMLFile.open( QIODevice::WriteOnly );

    QTextStream fout( &badXMLFile );
    fout << "<!DOCTYPE NotPencilDocument><document></document>";
    badXMLFile.close();

    FileManager fm;
    Object* pObj = fm.load( strBadXMLPath );

    QVERIFY( pObj == NULL );
    QVERIFY( fm.error().code() == Status::ERROR_INVALID_PENCIL_FILE );
}

void TestObjectSaveLoader::testMinimalOldPencilDocument()
{
    QTemporaryFile minimalDoc;
    if ( minimalDoc.open() )
    {
        QFile minXML( minimalDoc.fileName() );
        minXML.open( QIODevice::WriteOnly );

        QTextStream fout( &minXML );
        fout << "<!DOCTYPE PencilDocument><document>";
        fout << "  <object></object>";
        fout << "</document>";
        minXML.close();

        FileManager fm;
        Object* o = fm.load( minimalDoc.fileName() );
        OnScopeExit( delete o );

        QVERIFY( o != nullptr );
        QVERIFY( fm.error().ok() );
        QVERIFY( o->getLayerCount() == 0 );
    }
    else
    {
        QFAIL( "Can't open temp file." );
    }
}

void TestObjectSaveLoader::testOneLayerInFile()
{
    QTemporaryFile tmpFile;
    if ( !tmpFile.open() )
    {
        QFAIL( "temp file" );
    }
    QFile theXML( tmpFile.fileName() );
    theXML.open( QIODevice::WriteOnly );
    
    QTextStream fout( &theXML );
    fout << "<!DOCTYPE PencilDocument><document>";
    fout << "  <object>";
    fout <<	"    <layer name='MyLayer' id='5' visibility='1' type='1'></layer>";
    fout << "  </object>";
    fout << "</document>";
    theXML.close();
        
    FileManager fm;
    Object* obj = fm.load( theXML.fileName() );
    OnScopeExit( delete obj );
        
    QVERIFY( obj->getLayerCount() == 1 );
}

void TestObjectSaveLoader::testBitmapLayer()
{
    QTemporaryFile tmpFile;
    if ( !tmpFile.open() )
    {
        QFAIL( "temp file" );
    }
    QFile theXML( tmpFile.fileName() );
    theXML.open( QIODevice::WriteOnly );
    
    QTextStream fout( &theXML );
    fout << "<!DOCTYPE PencilDocument><document>";
    fout << "  <object>";
    fout <<	"    <layer name='MyLayer' id='5' visibility='1' type='1'></layer>";
    fout << "  </object>";
    fout << "</document>";
    theXML.close();
    
    FileManager fm;
    Object* obj = fm.load( theXML.fileName() );
    OnScopeExit( delete obj );
    
    Layer* layer = obj->getLayer( 0 );
    QVERIFY2( layer->name() == "MyLayer", "LayerName is different" );
    QCOMPARE( layer->id(), 5 );
    QCOMPARE( layer->visible(), true );
    QCOMPARE( layer->type(), Layer::BITMAP );
}

