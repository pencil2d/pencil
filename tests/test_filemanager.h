
#ifndef TEST_OBJECTSAVELOADER_H
#define TEST_OBJECTSAVELOADER_H


#include <QString>
#include <QtTest>
#include "AutoTest.h"


class TestFileManager : public QObject
{
    Q_OBJECT


private slots:
    void testCase1();
    void testNotExistFile();
    void testInvalidXML();
    void testInvalidPencilDocument();
    void testMinimalOldPencilDocument();
    void testOneLayerInFile();
    void testBitmapLayer();
    void testBitmapLayer2();

    void testGeneratePCLX();
    void testLoadPCLX();
};

DECLARE_TEST(TestFileManager)

#endif // TEST_OBJECTSAVELOADER_H
