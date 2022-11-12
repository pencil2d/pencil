#include "vectorimage.h"
#include "catch.hpp"

TEST_CASE("VectorImage removeColor")
{
    auto vImage = VectorImage();
    auto bezier = BezierCurve({ QPoint(50,50), QPoint(100,100)});
    SECTION("Ensure that number is changed")
    {
        bezier.setColorNumber(3);
        vImage.addCurve(bezier, 1.0);

        vImage.removeColor(3);

        REQUIRE(vImage.curve(0).getColorNumber() == 2);
    }

    SECTION("Can't get below zero")
    {
        bezier.setColorNumber(0);
        vImage.addCurve(bezier, 1.0);

        vImage.removeColor(0);

        REQUIRE(vImage.curve(0).getColorNumber() == 0);
    }
}
