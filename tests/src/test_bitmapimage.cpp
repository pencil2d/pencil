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
#include "catch.hpp"

#include "bitmapimage.h"
#include <QElapsedTimer>
#include <QDebug>

TEST_CASE("BitmapImage constructors")
{
    SECTION("Init an Bitmap Image")
    {
        auto b = std::make_shared<BitmapImage>();
        REQUIRE(b->image()->isNull());

        REQUIRE(b->width() == 0);
        REQUIRE(b->height() == 0);
        REQUIRE(b->top() == 0);
        REQUIRE(b->left() == 0);
    }

    SECTION("Init with color and boundary")
    {
        auto b = std::make_shared<BitmapImage>(QRect(10, 20, 30, 40), Qt::red);

        REQUIRE(b->left() == 10);
        REQUIRE(b->top() == 20);
        REQUIRE(b->width() == 30);
        REQUIRE(b->height() == 40);

        QRgb rgb = b->image()->pixel(0, 0);
        REQUIRE(rgb == qRgb(255, 0, 0));
    }

    SECTION("Clone a BitmapImage")
    {
        auto b = std::make_shared<BitmapImage>(QRect(20, 20, 100, 100), Qt::red);
        auto b2 = b->clone();

        REQUIRE(b->pos() == b2->pos());
        REQUIRE(b->length() == b2->length());
        
        REQUIRE(b->left() == b2->left());
        REQUIRE(b->right() == b2->right());
        REQUIRE(b->width() == b2->width());
        REQUIRE(b->height() == b2->height());
        
        QImage* img1 = b->image();
        QImage* img2 = b2->image();
        REQUIRE(img1 != img2);
        REQUIRE((*img1) == (*img2));
    }

    SECTION("#947 Initial Color")
    {
        // A new bitmap image must be fully transparent
        // otherwise a pixel dot will appear in center of canvas
        // ref: https://github.com/pencil2d/pencil/pull/947

        auto b = std::make_shared<BitmapImage>();
        for (int x = 0; x < b->width(); ++x)
        {
            for (int y = 0; y < b->height(); ++y)
            {
                QRgb color = b->pixel(x, y);
                REQUIRE(qAlpha(color) == 0);
            }
        }
    }
}

TEST_CASE("BitmapImage functions")
{
    SECTION("moveTopLeft()")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 50, 50), Qt::red);
        b->moveTopLeft(QPoint(20, 10));

        REQUIRE(b->topLeft() == QPoint(20, 10));
        REQUIRE(b->width() == 50);
        REQUIRE(b->height() == 50);
    }
}

TEST_CASE("BitmapImage autoCrop")
{
    SECTION("Empty image (all transparent)")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);
        b->enableAutoCrop(true);
        b->autoCrop();

        // Empty images should have empty bounds
        REQUIRE(b->width() == 0);
        REQUIRE(b->height() == 0);
    }

    SECTION("Single pixel at origin")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);
        b->setPixel(0, 0, qRgba(255, 0, 0, 255)); // Red pixel at top-left
        b->enableAutoCrop(true);
        b->autoCrop();

        REQUIRE(b->width() == 1);
        REQUIRE(b->height() == 1);
        REQUIRE(b->left() == 0);
        REQUIRE(b->top() == 0);
    }

    SECTION("Single pixel at center")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);
        b->setPixel(50, 50, qRgba(255, 0, 0, 255)); // Red pixel at center
        b->enableAutoCrop(true);
        b->autoCrop();

        REQUIRE(b->width() == 1);
        REQUIRE(b->height() == 1);
        REQUIRE(b->left() == 50);
        REQUIRE(b->top() == 50);
    }

    SECTION("Single pixel at bottom-right corner")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);
        b->setPixel(99, 99, qRgba(255, 0, 0, 255)); // Red pixel at bottom-right
        b->enableAutoCrop(true);
        b->autoCrop();

        REQUIRE(b->width() == 1);
        REQUIRE(b->height() == 1);
        REQUIRE(b->left() == 99);
        REQUIRE(b->top() == 99);
    }

    SECTION("Centered content with transparent borders")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);

        // Draw a 20x20 red rectangle at position (40, 40)
        for (int x = 40; x < 60; ++x)
        {
            for (int y = 40; y < 60; ++y)
            {
                b->setPixel(x, y, qRgba(255, 0, 0, 255));
            }
        }

        b->enableAutoCrop(true);
        b->autoCrop();

        REQUIRE(b->width() == 20);
        REQUIRE(b->height() == 20);
        REQUIRE(b->left() == 40);
        REQUIRE(b->top() == 40);
    }

    SECTION("Content at edges (top and left)")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);

        // Draw L-shape at top-left corner (50x50)
        for (int x = 0; x < 50; ++x)
        {
            b->setPixel(x, 0, qRgba(255, 0, 0, 255)); // Top edge
        }
        for (int y = 0; y < 50; ++y)
        {
            b->setPixel(0, y, qRgba(255, 0, 0, 255)); // Left edge
        }

        b->enableAutoCrop(true);
        b->autoCrop();

        REQUIRE(b->width() == 50);
        REQUIRE(b->height() == 50);
        REQUIRE(b->left() == 0);
        REQUIRE(b->top() == 0);
    }

    SECTION("Content at edges (bottom and right)")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);

        // Draw L-shape at bottom-right corner (50x50)
        for (int x = 50; x < 100; ++x)
        {
            b->setPixel(x, 99, qRgba(255, 0, 0, 255)); // Bottom edge
        }
        for (int y = 50; y < 100; ++y)
        {
            b->setPixel(99, y, qRgba(255, 0, 0, 255)); // Right edge
        }

        b->enableAutoCrop(true);
        b->autoCrop();

        REQUIRE(b->width() == 50);
        REQUIRE(b->height() == 50);
        REQUIRE(b->left() == 50);
        REQUIRE(b->top() == 50);
    }

    SECTION("Inverted T shape (worst case for row-based approach)")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);

        // Draw horizontal line at top
        for (int x = 0; x < 100; ++x)
        {
            b->setPixel(x, 0, qRgba(255, 0, 0, 255));
        }
        // Draw vertical line in middle
        for (int y = 0; y < 100; ++y)
        {
            b->setPixel(50, y, qRgba(255, 0, 0, 255));
        }

        b->enableAutoCrop(true);
        b->autoCrop();

        REQUIRE(b->width() == 100);
        REQUIRE(b->height() == 100);
        REQUIRE(b->left() == 0);
        REQUIRE(b->top() == 0);
    }

    SECTION("Diagonal line from corner to corner")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);

        // Draw diagonal line from (0,0) to (99,99)
        for (int i = 0; i < 100; ++i)
        {
            b->setPixel(i, i, qRgba(255, 0, 0, 255));
        }

        b->enableAutoCrop(true);
        b->autoCrop();

        REQUIRE(b->width() == 100);
        REQUIRE(b->height() == 100);
        REQUIRE(b->left() == 0);
        REQUIRE(b->top() == 0);
    }

    SECTION("Already minimally bounded (no change expected)")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 50, 50), Qt::red);
        b->enableAutoCrop(true);
        b->autoCrop();

        // Image is already fully opaque, so bounds shouldn't change
        REQUIRE(b->width() == 50);
        REQUIRE(b->height() == 50);
        REQUIRE(b->left() == 0);
        REQUIRE(b->top() == 0);
    }

    SECTION("AutoCrop disabled (no change expected)")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);
        b->setPixel(50, 50, qRgba(255, 0, 0, 255));
        b->enableAutoCrop(false); // Disable autoCrop
        b->autoCrop();

        // Bounds should NOT change because autoCrop is disabled
        REQUIRE(b->width() == 100);
        REQUIRE(b->height() == 100);
        REQUIRE(b->left() == 0);
        REQUIRE(b->top() == 0);
    }

    SECTION("Large image with padding consideration")
    {
        // Test with a larger image to ensure scanline padding is handled correctly
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 1000, 1000), Qt::transparent);

        // Draw a rectangle at position (100, 100) with size (800, 800)
        for (int x = 100; x < 900; ++x)
        {
            for (int y = 100; y < 900; ++y)
            {
                b->setPixel(x, y, qRgba(255, 0, 0, 255));
            }
        }

        b->enableAutoCrop(true);
        b->autoCrop();

        REQUIRE(b->width() == 800);
        REQUIRE(b->height() == 800);
        REQUIRE(b->left() == 100);
        REQUIRE(b->top() == 100);
    }

    SECTION("Multiple separate pixels")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);

        // Place pixels at various positions
        b->setPixel(10, 10, qRgba(255, 0, 0, 255));
        b->setPixel(90, 20, qRgba(0, 255, 0, 255));
        b->setPixel(30, 80, qRgba(0, 0, 255, 255));
        b->setPixel(70, 70, qRgba(255, 255, 0, 255));

        b->enableAutoCrop(true);
        b->autoCrop();

        // Bounds should be from (10,10) to (90,80)
        REQUIRE(b->width() == 81); // 90 - 10 + 1
        REQUIRE(b->height() == 71); // 80 - 10 + 1
        REQUIRE(b->left() == 10);
        REQUIRE(b->top() == 10);
    }

    SECTION("Horizontal line (single row)")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);

        // Draw horizontal line from (20,50) to (79,50)
        for (int x = 20; x < 80; ++x)
        {
            b->setPixel(x, 50, qRgba(255, 0, 0, 255));
        }

        b->enableAutoCrop(true);
        b->autoCrop();

        REQUIRE(b->width() == 60);
        REQUIRE(b->height() == 1);
        REQUIRE(b->left() == 20);
        REQUIRE(b->top() == 50);
    }

    SECTION("Vertical line (single column)")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 100, 100), Qt::transparent);

        // Draw vertical line from (50,20) to (50,79)
        for (int y = 20; y < 80; ++y)
        {
            b->setPixel(50, y, qRgba(255, 0, 0, 255));
        }

        b->enableAutoCrop(true);
        b->autoCrop();

        REQUIRE(b->width() == 1);
        REQUIRE(b->height() == 60);
        REQUIRE(b->left() == 50);
        REQUIRE(b->top() == 20);
    }
}

TEST_CASE("BitmapImage autoCrop performance")
{
    SECTION("Profile autoCrop on large image (4000x4000)")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 4000, 4000), Qt::transparent);
        
        // Draw some content in center
        for (int y = 800; y < 1200; ++y) {
            for (int x = 800; x < 1200; ++x) {
                b->setPixel(x, y, qRgba(255, 0, 0, 255));
            }
        }
        
        b->enableAutoCrop(true);
        
        QElapsedTimer timer;
        timer.start();
        b->autoCrop();
        qint64 elapsed = timer.nsecsElapsed();
        
        qDebug() << "Large image (4000x4000) autoCrop took:" << elapsed / 1000.0 << "microseconds";
        
        REQUIRE(b->width() == 400);
        REQUIRE(b->height() == 400);
    }
    
    SECTION("Profile autoCrop worst case - single pixel at corner")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 1000, 1000), Qt::transparent);
        
        // Single pixel at far corner
        b->setPixel(999, 999, qRgba(255, 0, 0, 255));
        
        b->enableAutoCrop(true);
        
        QElapsedTimer timer;
        timer.start();
        b->autoCrop();
        qint64 elapsed = timer.nsecsElapsed();
        
        qDebug() << "Worst case (single pixel at 999,999) autoCrop took:" << elapsed / 1000.0 << "microseconds";
        
        REQUIRE(b->width() == 1);
        REQUIRE(b->height() == 1);
        REQUIRE(b->left() == 999);
        REQUIRE(b->top() == 999);
    }
    
    SECTION("Profile autoCrop with sparse content")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 1000, 1000), Qt::transparent);
        
        // Draw sparse pixels across image
        for (int i = 0; i < 1000; i += 100) {
            b->setPixel(i, i, qRgba(255, 0, 0, 255));
        }
        
        b->enableAutoCrop(true);
        
        QElapsedTimer timer;
        timer.start();
        b->autoCrop();
        qint64 elapsed = timer.nsecsElapsed();
        
        qDebug() << "Sparse content autoCrop took:" << elapsed / 1000.0 << "microseconds";
        
        REQUIRE(b->width() == 901);
        REQUIRE(b->height() == 901);
    }
}
