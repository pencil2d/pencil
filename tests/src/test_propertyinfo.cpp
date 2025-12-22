/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang
Copyright (C) 2025-2099 Oliver S. Larsen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "catch.hpp"
#include "toolproperties.h"

TEST_CASE("PropertyInfo behavior", "[PropertyInfo]") {

    SECTION("Integer type") {
        PropertyInfo info(1, 10, 5);

        REQUIRE(info.type() == PropertyInfo::INTEGER);

        SECTION("Base value access") {
            REQUIRE(info.intValue() == 5);
        }

        SECTION("Clamping max") {
            info.setBaseValue(15);
            REQUIRE(info.intValue() == 10);
        }

        SECTION("Clamping min") {
            info.setBaseValue(-1);
            REQUIRE(info.intValue() == 1);
        }

        SECTION("Reset to default") {
            info.setBaseValue(7);
            info.resetBaseValue();
            REQUIRE(info.intValue() == 5);
        }

        SECTION("Default value") {
            REQUIRE(info.defaultInt() == 5);
        }
    }

    SECTION("Real type") {
        PropertyInfo info(0.5, 2.5, 1.5);

        REQUIRE(info.type() == PropertyInfo::REAL);

        SECTION("Base value access") {
            REQUIRE(info.realValue() == Approx(1.5));
        }

        SECTION("Clamping max") {
            info.setBaseValue(3.0);
            REQUIRE(info.realValue() == Approx(2.5));
        }

        SECTION("Clamping min") {
            info.setBaseValue(0.0);
            REQUIRE(info.realValue() == Approx(0.5));
        }

        SECTION("Reset to default") {
            info.setBaseValue(2.0);
            info.resetBaseValue();
            REQUIRE(info.realValue() == Approx(1.5));
        }

        SECTION("Default value") {
            REQUIRE(info.defaultReal() == Approx(1.5));
        }
    }

    SECTION("Boolean type") {
        PropertyInfo info(true, false);

        REQUIRE(info.type() == PropertyInfo::BOOL);

        SECTION("Base value access") {
            REQUIRE(info.boolValue() == true);
        }

        SECTION("Reset to default") {
            info.setBaseValue(false);
            info.resetBaseValue();
            REQUIRE(info.boolValue() == false);
        }

        SECTION("Default value") {
            REQUIRE(info.defaultBool() == false);
        }
    }

    SECTION("Invalid type") {
        PropertyInfo info;

        REQUIRE(info.type() == PropertyInfo::INVALID);

        SECTION("Default values for invalid type") {
            REQUIRE(info.defaultInt() == -1);
            REQUIRE(info.defaultReal() == -1.0);
            REQUIRE(info.defaultBool() == false);
        }
    }
}
