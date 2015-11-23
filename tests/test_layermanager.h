#ifndef TEST_LAYER_MANAGER_H
#define TEST_LAYER_MANAGER_H

#include "AutoTest.h"

class Object;
class Editor;
class LayerManager;


class TestLayerManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testNewLayerManager();
    
private:
    Editor* mEditor = nullptr;
    LayerManager* mLayerManager = nullptr;
};

DECLARE_TEST( TestLayerManager )

#endif
