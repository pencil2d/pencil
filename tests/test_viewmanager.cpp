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

    v.translate(0, 30);
    QCOMPARE( v.mapCanvasToScreen(QPointF(0, 0)), QPointF(0, 30));
}

void TestViewManager::testRotation90()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();

    v.rotate(90); // counter-clockwise rotation
    QCOMPARE( v.mapCanvasToScreen(QPointF(1, 0)), QPointF(0, 1));
    QCOMPARE(v.mapCanvasToScreen(QPointF(23.6, 0)), QPointF(0, 23.6));
}

void TestViewManager::testRotation180()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();

    v.rotate(180); // counter-clockwise rotation
    QCOMPARE(v.mapCanvasToScreen(QPointF(1, 0)), QPointF(-1, 0));
    QCOMPARE(v.mapCanvasToScreen(QPointF(92.1, 0)), QPointF(-92.1, 0));
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

    v.scale(6);
    QCOMPARE(v.mapCanvasToScreen(QPointF(1, 1)), QPointF(6, 6));
    QCOMPARE(v.scaling(), 6.0f);

    QCOMPARE(v.mapCanvasToScreen(QPointF(-1, 2)), QPointF(-6, 12));
    QCOMPARE(v.scaling(), 6.0f);
}

void TestViewManager::testMaxScalingValue()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();

    // set a ridiculously large number
    // should be clamp to 100.f, the maximum scaling value we set
    v.scale(10000);
    QCOMPARE(v.mapCanvasToScreen(QPointF(1, 1)), QPointF(100, 100));
}

void TestViewManager::testMinScalingValue()
{
    ViewManager v(mEditor);
    v.setEditor(mEditor);
    v.init();

    v.scale(0.0005f);

    QPointF p1 = v.mapCanvasToScreen(QPointF(1, 1));
    QPointF p2 = QPointF(0.01, 0.01);

    QVERIFY(std::abs(p1.x() - p2.x()) < 0.000001);
    QVERIFY(std::abs(p1.y() - p2.y()) < 0.000001);
}
