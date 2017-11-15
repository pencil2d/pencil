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

    void init();
    void cleanup();

    void testInit();

    void testTranslation10();
    void testTranslation2Times();
    void testTranslationQPointF();

    void testRotation90();
    void testRotation180();
    void testRotationTwice();

    void testScaling2();
    void testScaling2Times();
    void testMaxScalingValue();
    void testMinScalingValue();

    void testTranslateAndRotate();

    void testResetView();

    void testEmptyCameraLayer();
    void testCameraLayerWithTwoKeys();
    void testSetCameraLayerAndRemoveIt();

	void testCanvasSize();
	void testLoadViewFromObject1();
	void testLoadViewFromObject2();
    void testSetCameraKey();

private:
    Editor* mEditor = nullptr;
};

DECLARE_TEST(TestViewManager);

#endif // TEST_VIEWMANAGER_H
