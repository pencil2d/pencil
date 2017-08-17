#include "test_viewmanager.h"
#include "viewmanager.h"
#include "object.h"

TestViewManager::TestViewManager()
{

}

void TestViewManager::initTestCase()
{
    Object* object = new Object();
    object->init();

    mEditor = new Editor();
    mEditor->setObject(object);
}

void TestViewManager::cleanupTestCase()
{
    delete mEditor;
}

void TestViewManager::testTranslation10()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();

    v.translate(10, 10);

    QCOMPARE( QPointF(10 ,10), v.mapCanvasToScreen(QPointF(0,0)) );
}

void TestViewManager::testTranslation2Times()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();

    v.translate(0, 15);
    QCOMPARE( v.mapCanvasToScreen(QPointF(0, 0)), QPointF(0, 15));

    v.translate(0, 15); // will be 15+15 = 30
    QCOMPARE( v.mapCanvasToScreen(QPointF(0, 0)), QPointF(0, 30));
}

void TestViewManager::testRotation90()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();

    v.rotate(90); // counter-clockwise rotation
    QCOMPARE( v.mapCanvasToScreen(QPointF(1, 0)), QPointF(0, 1));
}

void TestViewManager::testScaling2()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();

    v.scale(2);
    QCOMPARE(v.mapCanvasToScreen(QPointF(1, 1)), QPointF(2, 2));
    QCOMPARE(v.scaling(), 2.0f);
}

void TestViewManager::testScaling2Times()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();

    v.scale(2);
    QCOMPARE(v.mapCanvasToScreen(QPointF(1, 1)), QPointF(2, 2));
    QCOMPARE(v.scaling(), 2.0f);

    v.scale(3);
    QCOMPARE(v.mapCanvasToScreen(QPointF(1, 1)), QPointF(6, 6));
    QCOMPARE(v.scaling(), 6.0f);
}
