
#include "objectsaveloader.h"
#include "test_objectsaveloader.h"
#include "util.h"

void TestObjectSaveLoader::testCase1()
{
    //ObjectSaveLoader* pSaveLoader = new ObjectSaveLoader(this);
    QVERIFY2(true, "Failure");
}

void TestObjectSaveLoader::testNotExistFile()
{
    FileManager fm;

    QString strDummyPath = "hahaha_blala.pcl";
    Object* pObject = fm.load( strDummyPath );

    QVERIFY2( pObject == NULL, "File doesn't exist.");
    QVERIFY2( fm.error().code() == Status::FILE_NOT_FOUND, "" );
}

void TestObjectSaveLoader::testInvalidXML()
{
    QString strBadXMLPath = QDir::tempPath() + "/bad.pcl";
    
    QFile badXMLFile( strBadXMLPath );
    badXMLFile.open( QIODevice::WriteOnly );

    QTextStream fout( &badXMLFile );
    fout << "*&%% haha, this is not a xml file.";
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

void TestObjectSaveLoader::testMinimalPencilDocument()
{
    QString strBadXMLPath = QDir::tempPath() + "/minimal.pcl";
    OnScopeExit( QFile::remove( strBadXMLPath ) );

    QFile badXMLFile( strBadXMLPath );
    badXMLFile.open( QIODevice::WriteOnly );

    QTextStream fout( &badXMLFile );
    fout << "<!DOCTYPE PencilDocument><document></document>";
    badXMLFile.close();

    FileManager fm;
    Object* pObj = fm.load( strBadXMLPath );

    QVERIFY( pObj != NULL );
    QVERIFY( fm.error().ok() );
}




