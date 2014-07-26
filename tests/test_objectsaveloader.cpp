
#include "objectsaveloader.h"
#include "test_objectsaveloader.h"

TestObjectSaveLoader::TestObjectSaveLoader()
{
}

void TestObjectSaveLoader::testCase1()
{
    //ObjectSaveLoader* pSaveLoader = new ObjectSaveLoader(this);
    QVERIFY2(true, "Failure");
}

void TestObjectSaveLoader::testNotExistFile()
{
    ObjectSaveLoader pSaveLoader;

    QString strDummyPath = "hahaha_blala.pcl";
    Object* pObject = pSaveLoader.load( strDummyPath );

    QVERIFY2( pObject == NULL, "File doesn't exist.");
    QVERIFY2( pSaveLoader.error().code() == ERROR_FILE_NOT_EXIST, "" );
}

void TestObjectSaveLoader::testInvalidXML()
{
    QString strBadXMLPath = QDir::tempPath() + "/bad.pcl";
    
    QFile badXMLFile( strBadXMLPath );
    badXMLFile.open( QIODevice::WriteOnly );

    QTextStream fout( &badXMLFile );
    fout << "*&%% haha, this is not a xml file.";
    badXMLFile.close();
    
    ObjectSaveLoader pSaveLoader;
    Object* pObj = pSaveLoader.load( strBadXMLPath );

    QVERIFY( pObj == NULL );
    QVERIFY( pSaveLoader.error().code() == ERROR_INVALID_XML_FILE );
}

void TestObjectSaveLoader::testInvalidPencilDocument()
{
    QString strBadXMLPath = QDir::tempPath() + "/bad.pcl";

    QFile badXMLFile( strBadXMLPath );
    badXMLFile.open( QIODevice::WriteOnly );

    QTextStream fout( &badXMLFile );
    fout << "<!DOCTYPE NotPencilDocument><document></document>";
    badXMLFile.close();

    ObjectSaveLoader pSaveLoader;
    Object* pObj = pSaveLoader.load( strBadXMLPath );

    QVERIFY( pObj == NULL );
    QVERIFY( pSaveLoader.error().code() == ERROR_INVALID_PENCIL_FILE );
}

void TestObjectSaveLoader::testMinimalPencilDocument()
{
    QString strBadXMLPath = QDir::tempPath() + "/minimal.pcl";

    QFile badXMLFile( strBadXMLPath );
    badXMLFile.open( QIODevice::WriteOnly );

    QTextStream fout( &badXMLFile );
    fout << "<!DOCTYPE PencilDocument><document></document>";
    badXMLFile.close();

    ObjectSaveLoader pSaveLoader;
    Object* pObj = pSaveLoader.load( strBadXMLPath );

    QVERIFY( pObj != NULL );
    QVERIFY( pSaveLoader.error().code() == PCL_OK );
}




