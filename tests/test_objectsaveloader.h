
#ifndef TEST_OBJECTSAVELOADER_H
#define TEST_OBJECTSAVELOADER_H


#include <QString>
#include <QtTest>
#include "AutoTest.h"


class TestObjectSaveLoader : public QObject
{
    Q_OBJECT

public:
    TestObjectSaveLoader();

private Q_SLOTS:
    void testCase1();
    void testNotExistFile();
    void testInvalidXML();
    void testInvalidPencilDocument();
    void testMinimalPencilDocument();
};

DECLARE_TEST(TestObjectSaveLoader)

#endif // TEST_OBJECTSAVELOADER_H
