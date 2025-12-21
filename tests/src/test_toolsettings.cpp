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
#include "toolsettings.h"

#include <QSettings>
#include <QTemporaryDir>

class MockSettings : public ToolSettingsBase {
public:
    enum PropertyKey {
        START = 0,
        WIDTH = START,
        ENABLED = 1,
        SCALE = 2,
        END = 3,
    };

    MockSettings() {
        mSettings.setRanges({ { START, END } });

        mSettings.insertIdentifiers({
            { WIDTH,        "Width" },
            { ENABLED,      "Enabled" },
            { SCALE,        "Scale" },
        });
    }

    ToolSettings& general() override { return mSettings; }

    PropertyInfo getInfo(int rawPropertyType) const override { return mSettings.getInfo(rawPropertyType); }

private:

    ToolSettings mSettings;
};

TEST_CASE("ToolSettings behavior", "[ToolSettings]") {
    QTemporaryDir tempDir;
    QString settingsPath = tempDir.path() + "/test.ini";
    QSettings settings(settingsPath, QSettings::IniFormat);

    MockSettings toolSettings;

    QHash<int, PropertyInfo> defaultProps = {
        { MockSettings::WIDTH, PropertyInfo(1, 10, 5) },
        { MockSettings::ENABLED, PropertyInfo(true, false) },
        { MockSettings::SCALE, PropertyInfo(0.5, 2.0, 1.0) }
    };

    SECTION("calling insertProperties inserts properties into the ToolSetting") {
        toolSettings.general().insertProperties(defaultProps);
        REQUIRE(toolSettings.getInfo(MockSettings::WIDTH).intValue() == 5);
        REQUIRE(toolSettings.getInfo(MockSettings::ENABLED).boolValue() == true);
    }

    SECTION("updateDefaults adds to existing properties") {
        toolSettings.general().insertProperties({ { MockSettings::WIDTH, PropertyInfo(0, 10, 3) } });

        QHash<int, PropertyInfo> extra = {
            { MockSettings::ENABLED, PropertyInfo(false, false) }
        };

        toolSettings.general().insertProperties(extra);
        REQUIRE(toolSettings.getInfo(MockSettings::ENABLED).boolValue() == false);
        REQUIRE(toolSettings.getInfo(MockSettings::WIDTH).intValue() == 3);
    }

    SECTION("setBaseValue overrides individual value") {
        toolSettings.general().insertProperties(defaultProps);
        toolSettings.general().setBaseValue(MockSettings::WIDTH, 8);
        REQUIRE(toolSettings.getInfo(MockSettings::WIDTH).intValue() == 8);
    }

    SECTION("restoreDefaults resets to original defaults") {
        toolSettings.general().insertProperties(defaultProps);
        toolSettings.general().setBaseValue(MockSettings::WIDTH, 9);
        toolSettings.general().restoreProperties();
        REQUIRE(toolSettings.getInfo(MockSettings::WIDTH).intValue() == 5);  // back to default
    }

    SECTION("save and load persist values") {
        toolSettings.general().insertProperties(defaultProps);
        toolSettings.general().loadFrom("mocktool", settings); // loads and stores
        toolSettings.general().setBaseValue(MockSettings::WIDTH, 7);  // override
        toolSettings.general().storeTo(settings);

        // New settings instance to simulate reload
        MockSettings reloaded;
        reloaded.general().insertProperties(defaultProps);
        reloaded.general().loadFrom("mocktool", settings);
        REQUIRE(reloaded.getInfo(MockSettings::WIDTH).intValue() == 7);
    }

    SECTION("Test migration from old settings") {
        toolSettings.general().insertProperties(defaultProps);

        GIVEN("The application is launched with old tool settings") {
            settings.setValue("brushWidth", 50);
            settings.sync();

            REQUIRE(toolSettings.general().requireMigration(settings, ToolSettings::VERSION_1) == true);

            WHEN("Migrating from the old settings") {
                toolSettings.general().setBaseValue(MockSettings::WIDTH, settings.value("brushWidth", 12).toInt());
                settings.remove("brushWidth");

                THEN("Migration is no longer required") {
                    REQUIRE(toolSettings.general().requireMigration(settings, ToolSettings::VERSION_1) == false);
                }
            }
        }
    }

    SECTION("Ensure migration of modified settings when applicable") {
        toolSettings.general().insertProperties(defaultProps);
        toolSettings.general().setVersion(ToolSettings::VERSION_1);  // Set current version
        toolSettings.general().loadFrom("mocktool", settings);
        toolSettings.general().storeTo(settings);

        REQUIRE(toolSettings.general().requireMigration(settings, ToolSettings::VERSION_1) == false);

        GIVEN("We update the settings version") {
            toolSettings.general().setVersion(ToolSettings::VERSION_3);

            WHEN("Old settings require migration") {
                REQUIRE(toolSettings.general().requireMigration(settings, ToolSettings::VERSION_2) == true);

                THEN("After settings have been migrated and stored, migration of that particular version is no longer required") {
                    toolSettings.general().storeTo(settings);
                    REQUIRE(toolSettings.general().requireMigration(settings, ToolSettings::VERSION_2) == false);
                }
            }
        }
    }

    SECTION("ToolSetting can only use settings from valid range") {
        toolSettings.general().insertProperties(defaultProps);
        REQUIRE(toolSettings.general().isValidType(StrokeSettings::FEATHER_ENABLED) == false);
    }
}
