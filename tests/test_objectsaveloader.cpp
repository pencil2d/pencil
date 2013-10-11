
#include "objectsaveloader.h"
#include "test_objectsaveloader.h"

TestObjectSaveLoader::TestObjectSaveLoader()
{
}

void TestObjectSaveLoader::testCase1()
{
    ObjectSaveLoader* pSaveLoader = new ObjectSaveLoader();
    QVERIFY2(true, "Failure");
}


