#include "object.h"
#include "editor.h"
#include "test_layermanager.h"


void TestLayerManager::initTestCase()
{
    m_pObject = new Object();
    m_pObject->init();

    Editor* eidtor = new Editor();
}

void TestLayerManager::cleanupTestCase()
{
}
