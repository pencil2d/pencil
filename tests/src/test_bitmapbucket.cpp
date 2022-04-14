#include "catch.hpp"

#include "layermanager.h"
#include "filemanager.h"
#include "scribblearea.h"

#include <QDir>

#include "layerbitmap.h"

#include "object.h"
#include "editor.h"
#include "bitmapbucket.h"
#include "bitmapimage.h"

#include "basetool.h"

void dragAndFill(QPointF movePoint, Editor* editor, QColor color, QRect bounds, Properties properties) {
    int moveX = 0;

    BitmapBucket bucket = BitmapBucket(editor, color, bounds, movePoint, properties);
    QPointF movingPoint = movePoint;

    int fillCount = 0;
    while (moveX < 40) {
        moveX++;
        movingPoint.setX(movingPoint.x()+1);

        bucket.paint(movingPoint, [&fillCount] (BucketState state, int, int ) {

            if (state == BucketState::DidFillTarget) {
                fillCount++;
            }
        });
    }

    // Make sure that only 4 fills has been applied
    // otherwise we're spilling onto unwanted pixels
    REQUIRE(fillCount == 4);
}

void verifyPixels(QPoint referencePoint, const BitmapImage* image, QRgb fillColor)
{
    REQUIRE(image->constScanLine(referencePoint.x(), referencePoint.y()) == fillColor);

    // pixels that are not transparent nor the given fill color, should be left untouched
    REQUIRE(image->constScanLine(referencePoint.x()+4, referencePoint.y()) != fillColor);
    REQUIRE(image->constScanLine(referencePoint.x()+5, referencePoint.y()) != fillColor);
    REQUIRE(image->constScanLine(referencePoint.x()+6, referencePoint.y()) != fillColor);
}

TEST_CASE("BitmapBucket - Fill drag logic")
{
    FileManager fm;
    Object* obj = fm.load(":/fill-drag-test/fill-drag-test.pcl");
    Editor* editor = new Editor;
    ScribbleArea* scribbleArea = new ScribbleArea(nullptr);
    editor->setScribbleArea(scribbleArea);
    editor->setObject(obj);
    editor->init();

    Properties properties;

    QDir dir;
    QString resultsPath = dir.currentPath() + "/fill-drag-test/";

    dir.mkpath(resultsPath);

    properties.bucketFillReferenceMode = 0;
    properties.bucketFillToLayerMode = 0;
    properties.bucketFillExpandEnabled = false;
    properties.fillMode = 0;

    QColor fillColor = QColor(255,255,0,100);

    BitmapImage beforeFill = *static_cast<LayerBitmap*>(editor->layers()->currentLayer())->getBitmapImageAtFrame(1);

    QPoint pressPoint = beforeFill.bounds().topLeft();

    pressPoint.setX(pressPoint.x()+3);
    pressPoint.setY(pressPoint.y()+7);

    REQUIRE(beforeFill.constScanLine(pressPoint.x(), pressPoint.y()) == 0);

    SECTION("FillTo: CurrentLayer - Reference: Current Layer")
    {
        dragAndFill(pressPoint, editor, fillColor, beforeFill.bounds(), properties);

        BitmapImage* image = static_cast<LayerBitmap*>(editor->layers()->currentLayer())->getLastBitmapImageAtFrame(1);

        image->writeFile(resultsPath + "test1.png");

        verifyPixels(pressPoint, image, fillColor.rgba());
    }

    SECTION("FillTo: Layer below - reference: current layer")
    {
        properties.bucketFillToLayerMode = 1;

        dragAndFill(pressPoint, editor, fillColor, beforeFill.bounds(), properties);

        // Verify that colors are correct on layer below
        BitmapImage* image = static_cast<LayerBitmap*>(editor->layers()->getLayer(editor->currentLayerIndex()-1))->getLastBitmapImageAtFrame(1);

        image->writeFile(resultsPath + "test2.png");

        verifyPixels(pressPoint, image, qPremultiply(fillColor.rgba()));
    }


    SECTION("FillTo: Layer below - reference: all layers")
    {
        properties.bucketFillToLayerMode = 1;
        properties.bucketFillReferenceMode = 1;

        dragAndFill(pressPoint, editor, fillColor, beforeFill.bounds(), properties);

        BitmapImage* image = static_cast<LayerBitmap*>(editor->layers()->getLayer(editor->currentLayerIndex()-1))->getLastBitmapImageAtFrame(1);

        image->writeFile(resultsPath + "test3.png");

        verifyPixels(pressPoint, image, qPremultiply(fillColor.rgba()));
    }

    SECTION("FillTo: Current layer - reference: all layers")
    {
        properties.bucketFillToLayerMode = 0;
        properties.bucketFillReferenceMode = 1;

        dragAndFill(pressPoint, editor, fillColor, beforeFill.bounds(), properties);

        BitmapImage* image = static_cast<LayerBitmap*>(editor->layers()->currentLayer())->getLastBitmapImageAtFrame(1);

        image->writeFile(resultsPath + "test4.png");

        verifyPixels(pressPoint, image, fillColor.rgba());
    }
}
