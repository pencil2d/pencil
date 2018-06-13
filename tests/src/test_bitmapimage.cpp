/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

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

TEST_CASE("BitmapImage constructors")
{
    SECTION("Init an Bitmap Image")
    {
        auto b = std::make_shared<BitmapImage>();
        REQUIRE(b->image()->isNull() == false);

        REQUIRE(b->width() == 1);
        REQUIRE(b->height() == 1);
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

    SECTION("extend()")
    {
        auto b = std::make_shared<BitmapImage>(QRect(0, 0, 50, 50), Qt::red);

        // before
        REQUIRE(b->topLeft() == QPoint(0, 0));
        REQUIRE(b->size() == QSize(50, 50));

        b->extend(QPoint(-10, -10));

        // after
        REQUIRE(b->topLeft() == QPoint(-10, -10));
        REQUIRE(b->size() == QSize(60, 60));
    }
}
