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

void dragAndFill(QPointF movePoint, Editor* editor, QColor color, QRect bounds, Properties properties, int fillCountThreshold) {
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

    REQUIRE(fillCount == fillCountThreshold);
}

void verifyOnlyPixelsInsideSegmentsAreFilled(QPoint referencePoint, const BitmapImage* image, QRgb fillColor)
{
    REQUIRE(image->constScanLine(referencePoint.x(), referencePoint.y()) == fillColor);

    // pixels that are not transparent nor the given fill color, should be left untouched
    REQUIRE(image->constScanLine(referencePoint.x()+4, referencePoint.y()) != fillColor);
    REQUIRE(image->constScanLine(referencePoint.x()+5, referencePoint.y()) != fillColor);
    REQUIRE(image->constScanLine(referencePoint.x()+6, referencePoint.y()) != fillColor);
}

/**
 *    Ascii representation of test project
 *    The "*" represent black strokes.
 *    The space inbetween represents transparency
 *    ***************
 *    *  *   *   *  *
 *    ***************
 *
 *    The test cases are based around filling on the initially transparent area and dragging across the four segments.
 */
TEST_CASE("BitmapBucket - Fill drag behaviour across four segments")
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
    properties.bucketFillExpandEnabled = false;
    properties.fillMode = 0;

    QColor fillColor = QColor(255,255,0,100);

    BitmapImage beforeFill = *static_cast<LayerBitmap*>(editor->layers()->currentLayer())->getBitmapImageAtFrame(1);

    QPoint pressPoint = beforeFill.bounds().topLeft();

    pressPoint.setX(pressPoint.x()+3);
    pressPoint.setY(pressPoint.y()+7);

    REQUIRE(beforeFill.constScanLine(pressPoint.x(), pressPoint.y()) == 0);

    // The dragging logic is based around that we only fill on either transparent or the same color as the fill color.
    SECTION("Filling on current layer - layer is not pre filled") {
        Layer* strokeLayer = editor->layers()->currentLayer();
        SECTION("When reference is current layer, only transparent color is filled")
        {
            properties.bucketFillReferenceMode = 0;
            dragAndFill(pressPoint, editor, fillColor, beforeFill.bounds(), properties, 4);

            BitmapImage* image = static_cast<LayerBitmap*>(strokeLayer)->getLastBitmapImageAtFrame(1);

            image->writeFile(resultsPath + "test1a.png");

            verifyOnlyPixelsInsideSegmentsAreFilled(pressPoint, image, qPremultiply(fillColor.rgba()));
        }

        SECTION("When reference is all layers, only transparent color is filled")
        {
            properties.bucketFillReferenceMode = 1;

            dragAndFill(pressPoint, editor, fillColor, beforeFill.bounds(), properties, 4);

            BitmapImage* image = static_cast<LayerBitmap*>(strokeLayer)->getLastBitmapImageAtFrame(1);

            image->writeFile(resultsPath + "test1b.png");

            verifyOnlyPixelsInsideSegmentsAreFilled(pressPoint, image, qPremultiply(fillColor.rgba()));
        }
    }

    SECTION("Filling on current layer - layer is pre-filled") {

        // Fill mode is set to `replace` because it makes it easier to compare colors...
        properties.fillMode = 1;
        Layer* strokeLayer = editor->layers()->currentLayer();
        SECTION("When reference is current layer, only pixels matching the fill color are filled"){
            properties.bucketFillReferenceMode = 0;

            dragAndFill(pressPoint, editor, fillColor, beforeFill.bounds(), properties, 4);
            BitmapImage* image = static_cast<LayerBitmap*>(strokeLayer)->getLastBitmapImageAtFrame(1);
            image->writeFile(resultsPath + "test2a-first.png");

            fillColor = QColor(0,255,0,255);
            dragAndFill(pressPoint, editor, fillColor, beforeFill.bounds(), properties, 4);

            image = static_cast<LayerBitmap*>(strokeLayer)->getLastBitmapImageAtFrame(1);
            image->writeFile(resultsPath + "test2a-second.png");

            verifyOnlyPixelsInsideSegmentsAreFilled(pressPoint, image, fillColor.rgba());
        }

        SECTION("When reference is all layers")
        {
            properties.bucketFillReferenceMode = 1;

            dragAndFill(pressPoint, editor, fillColor, beforeFill.bounds(), properties, 4);
            BitmapImage* image = static_cast<LayerBitmap*>(strokeLayer)->getLastBitmapImageAtFrame(1);
            image->writeFile(resultsPath + "test3a-first.png");

            fillColor = QColor(0,255,0,255);
            dragAndFill(pressPoint, editor, fillColor, beforeFill.bounds(), properties, 4);

            image = static_cast<LayerBitmap*>(strokeLayer)->getLastBitmapImageAtFrame(1);
            image->writeFile(resultsPath + "test3a-second.png");

            verifyOnlyPixelsInsideSegmentsAreFilled(pressPoint, image, fillColor.rgba());
        }
    }
}
