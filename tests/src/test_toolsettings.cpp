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

class MockSettings : public ToolSettings {
public:
    enum PropertyKey {
        START = 0,
        WIDTH = START,
        ENABLED = 1,
        SCALE = 2,
        END = 3,
    };

    MockSettings() {
        mTypeRanges = { { START, END } };
    }

protected:
    QString identifier(int key) const override {
        switch (key) {
            case WIDTH: return "width";
            case ENABLED: return "enabled";
            case SCALE: return "scale";
            default: return "invalid";
        }
    }
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

    SECTION("setDefaults sets default properties") {
        toolSettings.setDefaults(defaultProps);
        REQUIRE(toolSettings.getInfo(MockSettings::WIDTH).intValue() == 5);
        REQUIRE(toolSettings.getInfo(MockSettings::ENABLED).boolValue() == true);
    }

    SECTION("updateDefaults adds to existing properties") {
        toolSettings.setDefaults({ { MockSettings::WIDTH, PropertyInfo(0, 10, 3) } });

        QHash<int, PropertyInfo> extra = {
            { MockSettings::ENABLED, PropertyInfo(false, false) }
        };

        toolSettings.updateDefaults(extra);
        REQUIRE(toolSettings.getInfo(MockSettings::ENABLED).boolValue() == false);
        REQUIRE(toolSettings.getInfo(MockSettings::WIDTH).intValue() == 3);
    }

    SECTION("setBaseValue overrides individual value") {
        toolSettings.setDefaults(defaultProps);
        toolSettings.setBaseValue(MockSettings::WIDTH, 8);
        REQUIRE(toolSettings.getInfo(MockSettings::WIDTH).intValue() == 8);
    }

    SECTION("restoreDefaults resets to original defaults") {
        toolSettings.setDefaults(defaultProps);
        toolSettings.setBaseValue(MockSettings::WIDTH, 9);
        toolSettings.restoreDefaults();
        REQUIRE(toolSettings.getInfo(MockSettings::WIDTH).intValue() == 5);  // back to default
    }

    SECTION("save and load persist values") {
        toolSettings.setDefaults(defaultProps);
        toolSettings.load("mocktool", settings); // loads and stores
        toolSettings.setBaseValue(MockSettings::WIDTH, 7);  // override
        toolSettings.save(settings);

        // New settings instance to simulate reload
        MockSettings reloaded;
        reloaded.setDefaults(defaultProps);
        reloaded.load("mocktool", settings);
        REQUIRE(reloaded.getInfo(MockSettings::WIDTH).intValue() == 7);
    }

    SECTION("requireMigration returns true when version is bumped") {
        toolSettings.setDefaults(defaultProps);
        toolSettings.setVersion(1);  // Set current version
        toolSettings.load("mocktool", settings);
        toolSettings.save(settings);

        MockSettings newToolSettings;
        newToolSettings.setDefaults(defaultProps);
        newToolSettings.setVersion(2);  // Simulate version bump

        REQUIRE(newToolSettings.requireMigration(settings, 2) == true);
    }

    SECTION("ToolSetting can only use settings from valid range") {
        toolSettings.setDefaults(defaultProps);
        REQUIRE(toolSettings.isValidType(StrokeSettings::FEATHER_ENABLED) == false);
    }
}
