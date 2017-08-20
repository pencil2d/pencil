#ifndef TEST_VIEWMANAGER_H
#define TEST_VIEWMANAGER_H

#include <QObject>
#include "AutoTest.h"

class Editor;


class TestViewManager : public QObject
{
    Q_OBJECT
public:
    TestViewManager();

private slots: // test cases
    void initTestCase();
    void cleanupTestCase();

    void testTranslation10();
    void testTranslation2Times();

    void testRotation90();

    void testScaling2();
    void testScaling2Times();

    void testMaxScalingValue();
    void testMinScalingValue();

private:
    Editor* mEditor = nullptr;
};

DECLARE_TEST(TestViewManager);

#endif // TEST_VIEWMANAGER_H
