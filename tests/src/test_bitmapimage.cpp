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

TEST_CASE("BitmapImage")
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
}
