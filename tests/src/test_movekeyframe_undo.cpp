/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

// This test validates that the MoveKeyFramesCommand undo/redo logic
// correctly restores frames to their original positions using
// batch moveSelectedFrames() instead of per-frame moveKeyFrame().

#include "catch.hpp"

#include "layer.h"
#include "layerbitmap.h"
#include "bitmapimage.h"
#include "object.h"

// Helper: simulates the FIXED undo logic using moveSelectedFrames (batch)
static bool simulateUndoBatch(Layer* layer, const QList<int>& positions, int frameOffset)
{
    layer->deselectAll();
    for (int position : positions) {
        layer->setFrameSelected(position + frameOffset, true);
    }
    bool ok = layer->moveSelectedFrames(-frameOffset);
    layer->deselectAll();
    return ok;
}

// Helper: simulates the FIXED redo logic using moveSelectedFrames (batch)
static bool simulateRedoBatch(Layer* layer, const QList<int>& positions, int frameOffset)
{
    layer->deselectAll();
    for (int position : positions) {
        layer->setFrameSelected(position, true);
    }
    bool ok = layer->moveSelectedFrames(frameOffset);
    layer->deselectAll();
    return ok;
}

// Helper: simulates the OLD BUGGY undo using per-frame moveKeyFrame (forward order)
static bool simulateUndoPerFrame(Layer* layer, const QList<int>& positions, int frameOffset)
{
    bool allSucceeded = true;
    for (int position : positions) {
        bool ok = layer->moveKeyFrame(position + frameOffset, -frameOffset);
        if (!ok) allSucceeded = false;
    }
    return allSucceeded;
}

TEST_CASE("MoveKeyFramesCommand undo: batch moveSelectedFrames approach", "[MoveKeyFrameUndo]")
{
    Object* obj = new Object;

    SECTION("Two adjacent frames moved right by +1")
    {
        Layer* layer = obj->addNewBitmapLayer();

        BitmapImage* imgA = new BitmapImage(QRect(0,0,10,10), QColor(255,0,0));
        BitmapImage* imgB = new BitmapImage(QRect(0,0,10,10), QColor(0,255,0));
        layer->addKeyFrame(5, imgA);
        layer->addKeyFrame(6, imgB);

        QList<int> positions = {5, 6};
        int frameOffset = 1;

        // Perform the original move
        layer->setFrameSelected(5, true);
        layer->setFrameSelected(6, true);
        REQUIRE(layer->moveSelectedFrames(frameOffset));
        layer->deselectAll();

        REQUIRE(layer->keyExists(6));
        REQUIRE(layer->keyExists(7));
        REQUIRE_FALSE(layer->keyExists(5));

        // Undo using batch approach
        REQUIRE(simulateUndoBatch(layer, positions, frameOffset));

        CHECK(layer->keyExists(5));
        CHECK(layer->keyExists(6));
        CHECK_FALSE(layer->keyExists(7));
        CHECK(layer->getKeyFrameAt(5) == imgA);
        CHECK(layer->getKeyFrameAt(6) == imgB);
    }

    SECTION("Two adjacent frames moved left by -1")
    {
        Layer* layer = obj->addNewBitmapLayer();

        BitmapImage* imgA = new BitmapImage(QRect(0,0,10,10), QColor(255,0,0));
        BitmapImage* imgB = new BitmapImage(QRect(0,0,10,10), QColor(0,255,0));
        layer->addKeyFrame(5, imgA);
        layer->addKeyFrame(6, imgB);

        QList<int> positions = {5, 6};
        int frameOffset = -1;

        layer->setFrameSelected(5, true);
        layer->setFrameSelected(6, true);
        REQUIRE(layer->moveSelectedFrames(frameOffset));
        layer->deselectAll();

        REQUIRE(layer->keyExists(4));
        REQUIRE(layer->keyExists(5));
        REQUIRE_FALSE(layer->keyExists(6));

        // Undo using batch approach
        REQUIRE(simulateUndoBatch(layer, positions, frameOffset));

        CHECK(layer->keyExists(5));
        CHECK(layer->keyExists(6));
        CHECK_FALSE(layer->keyExists(4));
        CHECK(layer->getKeyFrameAt(5) == imgA);
        CHECK(layer->getKeyFrameAt(6) == imgB);
    }

    SECTION("Three adjacent frames moved right by +2")
    {
        Layer* layer = obj->addNewBitmapLayer();

        BitmapImage* imgA = new BitmapImage(QRect(0,0,10,10), QColor(255,0,0));
        BitmapImage* imgB = new BitmapImage(QRect(0,0,10,10), QColor(0,255,0));
        BitmapImage* imgC = new BitmapImage(QRect(0,0,10,10), QColor(0,0,255));
        layer->addKeyFrame(5, imgA);
        layer->addKeyFrame(6, imgB);
        layer->addKeyFrame(7, imgC);

        QList<int> positions = {5, 6, 7};
        int frameOffset = 2;

        layer->setFrameSelected(5, true);
        layer->setFrameSelected(6, true);
        layer->setFrameSelected(7, true);
        REQUIRE(layer->moveSelectedFrames(frameOffset));
        layer->deselectAll();

        REQUIRE(layer->keyExists(7));
        REQUIRE(layer->keyExists(8));
        REQUIRE(layer->keyExists(9));
        REQUIRE_FALSE(layer->keyExists(5));
        REQUIRE_FALSE(layer->keyExists(6));

        // Undo using batch approach
        REQUIRE(simulateUndoBatch(layer, positions, frameOffset));

        CHECK(layer->keyExists(5));
        CHECK(layer->keyExists(6));
        CHECK(layer->keyExists(7));
        CHECK_FALSE(layer->keyExists(8));
        CHECK_FALSE(layer->keyExists(9));
        CHECK(layer->getKeyFrameAt(5) == imgA);
        CHECK(layer->getKeyFrameAt(6) == imgB);
        CHECK(layer->getKeyFrameAt(7) == imgC);
    }

    SECTION("Non-adjacent frames moved right by +1")
    {
        Layer* layer = obj->addNewBitmapLayer();

        BitmapImage* imgA = new BitmapImage(QRect(0,0,10,10), QColor(255,0,0));
        BitmapImage* imgB = new BitmapImage(QRect(0,0,10,10), QColor(0,255,0));
        BitmapImage* imgC = new BitmapImage(QRect(0,0,10,10), QColor(0,0,255));
        layer->addKeyFrame(3, imgA);
        layer->addKeyFrame(7, imgB);
        layer->addKeyFrame(10, imgC);

        QList<int> positions = {3, 7, 10};
        int frameOffset = 1;

        layer->setFrameSelected(3, true);
        layer->setFrameSelected(7, true);
        layer->setFrameSelected(10, true);
        REQUIRE(layer->moveSelectedFrames(frameOffset));
        layer->deselectAll();

        // Undo using batch approach
        REQUIRE(simulateUndoBatch(layer, positions, frameOffset));

        CHECK(layer->keyExists(3));
        CHECK(layer->keyExists(7));
        CHECK(layer->keyExists(10));
        CHECK_FALSE(layer->keyExists(4));
        CHECK_FALSE(layer->keyExists(8));
        CHECK_FALSE(layer->keyExists(11));
        CHECK(layer->getKeyFrameAt(3) == imgA);
        CHECK(layer->getKeyFrameAt(7) == imgB);
        CHECK(layer->getKeyFrameAt(10) == imgC);
    }

    delete obj;
}

TEST_CASE("MoveKeyFramesCommand undo+redo roundtrip", "[MoveKeyFrameUndo]")
{
    Object* obj = new Object;

    SECTION("Undo then redo restores moved state")
    {
        Layer* layer = obj->addNewBitmapLayer();

        BitmapImage* imgA = new BitmapImage(QRect(0,0,10,10), QColor(255,0,0));
        BitmapImage* imgB = new BitmapImage(QRect(0,0,10,10), QColor(0,255,0));
        layer->addKeyFrame(5, imgA);
        layer->addKeyFrame(6, imgB);

        QList<int> positions = {5, 6};
        int frameOffset = 1;

        // Original move
        layer->setFrameSelected(5, true);
        layer->setFrameSelected(6, true);
        REQUIRE(layer->moveSelectedFrames(frameOffset));
        layer->deselectAll();

        // Undo
        REQUIRE(simulateUndoBatch(layer, positions, frameOffset));
        CHECK(layer->keyExists(5));
        CHECK(layer->keyExists(6));
        CHECK_FALSE(layer->keyExists(7));

        // Redo
        REQUIRE(simulateRedoBatch(layer, positions, frameOffset));
        CHECK_FALSE(layer->keyExists(5));
        CHECK(layer->keyExists(6));
        CHECK(layer->keyExists(7));
        CHECK(layer->getKeyFrameAt(6) == imgA);
        CHECK(layer->getKeyFrameAt(7) == imgB);

        // Undo again
        REQUIRE(simulateUndoBatch(layer, positions, frameOffset));
        CHECK(layer->keyExists(5));
        CHECK(layer->keyExists(6));
        CHECK_FALSE(layer->keyExists(7));
        CHECK(layer->getKeyFrameAt(5) == imgA);
        CHECK(layer->getKeyFrameAt(6) == imgB);
    }

    delete obj;
}

TEST_CASE("Regression: per-frame moveKeyFrame undo fails for left moves", "[MoveKeyFrameUndo][!shouldfail]")
{
    // This test documents the bug with the old per-frame approach.
    // Tagged [!shouldfail] because the old approach is expected to fail.
    Object* obj = new Object;

    SECTION("Two adjacent frames moved left by -1: per-frame undo loses a frame")
    {
        Layer* layer = obj->addNewBitmapLayer();

        BitmapImage* imgA = new BitmapImage(QRect(0,0,10,10), QColor(255,0,0));
        BitmapImage* imgB = new BitmapImage(QRect(0,0,10,10), QColor(0,255,0));
        layer->addKeyFrame(5, imgA);
        layer->addKeyFrame(6, imgB);

        QList<int> positions = {5, 6};
        int frameOffset = -1;

        layer->setFrameSelected(5, true);
        layer->setFrameSelected(6, true);
        REQUIRE(layer->moveSelectedFrames(frameOffset));
        layer->deselectAll();

        // Try undo with old per-frame approach
        simulateUndoPerFrame(layer, positions, frameOffset);

        // This SHOULD pass but WON'T with the old approach — position 5 is lost
        REQUIRE(layer->keyExists(5));
        REQUIRE(layer->keyExists(6));
    }

    delete obj;
}
