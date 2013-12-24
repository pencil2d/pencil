#ifndef TEST_LAYER_MANAGER_H
#define TEST_LAYER_MANAGER_H

#include "AutoTest.h"

class Object;

class TestLayerManager : QObject
{
    Q_OBJECT

public:


private slots:
    void initTestCase();
    void cleanupTestCase();

private:
    Object* m_pObject;
};

#endif