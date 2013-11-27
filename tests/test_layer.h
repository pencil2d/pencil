
#ifndef TEST_LAYER_H
#define TEST_LAYER_H


#include <QString>
#include <QtTest>
#include "AutoTest.h"


class TestLayer : public QObject
{
    Q_OBJECT

public:
    TestLayer();

private Q_SLOTS:
    void testCase1();
    
};

DECLARE_TEST(TestLayer)

#endif // TEST_LAYER_H
