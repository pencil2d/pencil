
#ifndef TEST_LAYER_H
#define TEST_LAYER_H


#include <QString>
#include <QtTest>
#include "AutoTest.h"

class Object;


class TestLayer : public QObject
{
    Q_OBJECT

public:
    TestLayer();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testCase1();
    void testLayerType();
    void testAddImageAtFrame();
    void testGetMaxFramePosition();
    void testHasKeyFrameAtPosition();
    void testGetFramePositionAt();
    void testRemoveImageAtFrame();

private:
    Object* m_pObject;
};

DECLARE_TEST(TestLayer)

#endif // TEST_LAYER_H
