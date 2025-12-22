/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef TOOLSETTINGS_H
#define TOOLSETTINGS_H

#include <QHash>
#include <QSettings>
#include <QDebug>
#include <QVector>
#include <QPair>

#include "pencildef.h"

struct PropertyInfo
{
    enum ValueType {
        INTEGER,
        REAL,
        BOOL,
        INVALID
    };

    PropertyInfo() {
        mValueType = INVALID;
        mBaseValue = -1;
    }
    PropertyInfo(int min, int max, int defaultValue)
        : mValueType(INTEGER) {
        mMinValue = min;
        mMaxValue = max;
        mDefaultValue = defaultValue;
        setBaseValue(defaultValue);
    }
    PropertyInfo(qreal min, qreal max, qreal defaultValue)
        : mValueType(REAL) {
        mMinValue = min;
        mMaxValue = max;
        mDefaultValue = defaultValue;
        setBaseValue(defaultValue);
    }

    PropertyInfo(bool base, bool defaultValue)
        : mValueType(BOOL) {
        mMinValue = false;
        mMaxValue = true;
        mDefaultValue = defaultValue;
        setBaseValue(base);
    }

    PropertyInfo(bool base) : PropertyInfo(base, base) {}
    PropertyInfo(int base) : PropertyInfo(base, base, base) {}
    PropertyInfo(qreal base) : PropertyInfo(base, base, base) {}

    void setBaseValue(int newValue) {
        Q_ASSERT(mValueType == INTEGER);
        mBaseValue = qBound(mMinValue.toInt(), newValue, mMaxValue.toInt());
    }

    void setBaseValue(qreal newValue) {
        Q_ASSERT(mValueType == REAL);
        mBaseValue = qBound(mMinValue.toReal(), newValue, mMaxValue.toReal());
    }

    void setBaseValue(bool newValue) {
        Q_ASSERT(mValueType == BOOL);
        mBaseValue = newValue;
    }

    QVariant toVariant() const {
        switch (mValueType) {
            case INTEGER:
                return mBaseValue;
            case REAL:
                return mBaseValue;
            case BOOL:
                return mBaseValue;
            case INVALID:
                return "INVALID";
        }
    }

    int intValue() const {
        if (mValueType != INTEGER) {
            Q_ASSERT(false);
            return -1;
        }

        return mBaseValue.toInt();
    }

    qreal realValue() const {
        if (mValueType != REAL) {
            Q_ASSERT(false);
            return -1.0;
        }

        return mBaseValue.toReal();
    }

    bool boolValue() const {
        if (mValueType != BOOL) {
            Q_ASSERT(false);
            return false;
        }
        return mBaseValue.toBool();
    }

    qreal minReal() const {
        if (mValueType != REAL) {
            return -1.0;
        }
        return mMinValue.toReal();
    }

    qreal maxReal() const {
        if (mValueType != REAL) {
            return -1.0;
        }

        return mMaxValue.toReal();
    }

    int minInt() const {
        if (mValueType != INTEGER) {
            return -1;
        }
        return mMinValue.toInt();
    }

    int maxInt() const {
        if (mValueType != INTEGER) {
            return -1;
        }

        return mMaxValue.toInt();
    }

    void resetBaseValue() {
        switch (mValueType) {
        case INTEGER:
            mBaseValue = mDefaultValue;
            break;
        case REAL:
            mBaseValue = mDefaultValue;
            break;
        case BOOL:
            mBaseValue = mDefaultValue;
            break;
        case INVALID:
            break;
        }
    }

    /// Returns the default value as an real, otherwise -1 if it hasn't been specified or the type doesn't match
    int defaultInt() {
        if (mValueType != INTEGER) {
            return -1;
        }
        return mDefaultValue.toInt();
    }

    /// Returns the default value as an real, otherwise -1.0 if it hasn't been specified or the type doesn't match
    qreal defaultReal() {
        if (mValueType != REAL) {
            return -1.0;
        }
        return mDefaultValue.toReal();
    }

    /// Returns the default value as an bool, otherwise false if it hasn't been specified or the type doesn't match
    bool defaultBool() {
        if (mValueType != BOOL) {
            return false;
        }
        return mDefaultValue.toBool();
    }

    ValueType type() const { return mValueType; }

private:
    ValueType mValueType;
    QVariant mBaseValue;
    QVariant mMinValue;
    QVariant mMaxValue;
    QVariant mDefaultValue;
};

struct ToolSettings
{
    enum Version {
        NOT_SET = 0,
        VERSION_1 = 1,
        VERSION_2,
        VERSION_3
    };

    ~ToolSettings() {}

    /* Inserts properties into the ToolSetting model for loading and saving */
    void insertProperties(const QHash<int, PropertyInfo>& properties) {
        mProps.insert(properties);
    }

    /*  Loads properties for the given tool from the input QSetting

        If an existing value is found in QSettings then that will be the BaseValue, otherwise
        it'll use use the default value.

        @param toolIdentifier The identifier for the tool. This is later used to look up settings, so make sure it's consistent
        @param settings The QSettings instance that contains the settings to load properties from
    */
    void loadFrom(const QString& toolIdentifier, QSettings& settings) {
        mIdentifier = toolIdentifier;
        settings.beginGroup(mIdentifier);

        Q_ASSERT(mProps.count() > 0);
        for (auto it = mProps.begin(); it != mProps.end(); ++it) {

            PropertyInfo& info = it.value();
            const QString& settingName = identifier(it.key());
            loadProperty(settingName, info, settings);
        }
        settings.endGroup();
    }

    /* Store all tool property changes into the input QSetting instance */
    void storeTo(QSettings& settings) {
        settings.setValue(mVersionKey, mVersion);
        settings.beginGroup(mIdentifier);

        for (auto it = mProps.begin(); it != mProps.end(); ++it) {

            QString propertyId = identifier(it.key());
            if (it.value().type() == PropertyInfo::INVALID) {
                Q_ASSERT_X(false, __func__, "Wrong state, expected a value type INTEGER|REAL|BOOL but got INVALID. You've probably misconfigured the property. "
                                            "Ensure the property has been setup correctly and try again.");
                continue;
            }
            settings.setValue(propertyId, it.value().toVariant());
        }
        settings.endGroup();
        settings.sync();
    }

    /*  Sets the BaseValue for the given property

        @param rawType The type that identifies with the value, eg. StrokeSettings::WIDTH_VALUE
        @param info A PropertyInfo struct which can either be INTEGER|REAL|BOOL
    */
    void setBaseValue(int rawType, const PropertyInfo& info)
    {
        if (!isValidType(rawType)) { return; }

        switch (info.type())
        {
        case PropertyInfo::INTEGER:
            mProps[rawType].setBaseValue(info.intValue());
            break;
        case PropertyInfo::REAL:
            mProps[rawType].setBaseValue(info.realValue());
            break;
        case PropertyInfo::BOOL:
            mProps[rawType].setBaseValue(info.boolValue());
            break;
        case PropertyInfo::INVALID:
            Q_ASSERT_X(false, __func__, "Expected value but got INVALID. Make sure the property has been setup properly before trying to set its base value.");
            break;
        }
    }

    void setVersion(Version version) { mVersion = version; }

    PropertyInfo getInfo(int rawPropertyType) const
    {
        return mProps[rawPropertyType];
    }

    void restoreProperties() {
        for (auto it = mProps.begin(); it != mProps.end(); ++it) {
            it.value().resetBaseValue();
        }
    }

    /* Checks whether keys referred to in settings needs to be migrated from the input version */
    bool requireMigration(QSettings& settings, ToolSettings::Version version) {

        if (hasLegacySettings(settings) && !settings.contains(mVersionKey)) {
            // Let's assume we're dealing with an existing user
            return true;
        }

        int migrationNumber = static_cast<int>(version);

        return settings.contains(mVersionKey) && migrationNumber < mVersion && migrationNumber > settings.value(mVersionKey).toInt();
    }

    bool hasLegacySettings(QSettings& settings) const {
        // Crude check for existing settings...
        return settings.contains("brushWidth") || settings.contains("pencilWidth") || settings.contains("penWidth");
    }

    bool isValidType(int rawType) const {
        QString rangeCheck;
        for (const auto& r : mTypeRanges) {

            #ifdef QT_DEBUG
                rangeCheck += QString("[%1...%2]").arg(r.first).arg(r.second);
            #endif
            if (rawType >= r.first && rawType < r.second) {
                return true;
            }
        }
        qWarning() << __FUNCTION__ << ":" << QString("Expected a valid type in range of %1 but instead got: %2. Make sure the input value matches one of the ranges of the tool setting").arg(rangeCheck).arg(rawType);
        return false;
    }

    void insertIdentifiers(const QHash<int, QString>& identifiers) {
        this->mIdentifiers.insert(identifiers);
    }

    QString identifier(int rawKey) const {
        auto it = mIdentifiers.find(rawKey);
        Q_ASSERT_X(it != mIdentifiers.end(),
                   "ToolSettings::identifier",
                   QString("No identifier matching the key %1 found").arg(rawKey).toUtf8().constData());
        return it.value();
    }

    void setRanges(const QVector<QPair<int, int>>& ranges) { mTypeRanges = ranges; }
    void addRange(QPair<int, int> range) { mTypeRanges.append(range); }
    const QVector<QPair<int, int>>& typeRanges() const { return mTypeRanges; }

private:

    void loadProperty(const QString& settingName, PropertyInfo& info, const QSettings& settings) {
        switch (info.type()) {
            case PropertyInfo::INTEGER: {
                QVariant value = settings.value(settingName, info.defaultInt());
                info.setBaseValue(value.toInt());
                break;
            }
            case PropertyInfo::REAL: {
                QVariant value = settings.value(settingName, info.defaultReal());
                info.setBaseValue(value.toReal());
                break;
            }
            case PropertyInfo::BOOL: {
                QVariant value = settings.value(settingName, info.defaultBool());
                info.setBaseValue(value.toBool());
                break;
            }
            case PropertyInfo::INVALID: {
                Q_ASSERT_X(false, __func__, "Wrong state, expected a value type but got INVALID. You've probably misconfigured the property. "
                                            "Ensure the property has been setup correctly and try again.");
                break;
            }
        }
    }

    // The list of ranges that are valid for the given tool. ToolSettings can inherit its parents cases as well
    // eg. PolyLineTool uses both StrokeSettings range as well as it's own
    QVector<QPair<int, int>> mTypeRanges;
    QHash<int, QString> mIdentifiers;
    QHash<int, PropertyInfo> mProps;

    QString mIdentifier = "undefined";
    Version mVersion = VERSION_1;
    QString mVersionKey = "ToolSettings_Version";
};

struct ToolSettingsBase {

    virtual ~ToolSettingsBase() {}

    virtual ToolSettings& general() = 0;
    virtual PropertyInfo getInfo(int rawPropertyType) const = 0;
};

struct StrokeSettings: public ToolSettingsBase
{

    enum Type {
        START               = 100,
        WIDTH_VALUE         = START,

        FEATHER_VALUE       = 101,
        STABILIZATION_VALUE = 102,
        PRESSURE_ENABLED         = 103,
        INVISIBILITY_ENABLED     = 104,
        FEATHER_ENABLED          = 105,
        ANTI_ALIASING_ENABLED    = 106,
        FILLCONTOUR_ENABLED      = 107,

        END                 = 199,
    };

    StrokeSettings() {
        mGeneralSettings.setRanges({ { START, END } });

        mGeneralSettings.insertIdentifiers({
            { WIDTH_VALUE,           "Width" },
            { FEATHER_VALUE,         "Feather" },
            { FEATHER_ENABLED,       "FeatherEnabled" },
            { STABILIZATION_VALUE,   "LineStabilization" },
            { PRESSURE_ENABLED,      "PressureEnabled" },
            { INVISIBILITY_ENABLED,  "InvisibilityEnabled" },
            { ANTI_ALIASING_ENABLED, "AntiAliasingEnabled"},
            { FILLCONTOUR_ENABLED,   "FillContourEnabled" }
        });
    }

    ToolSettings& general() override { return mGeneralSettings; }

    void addRange(const QPair<int, int> range) {
        mGeneralSettings.addRange(range);
    }

    PropertyInfo getInfo(int rawPropertyType) const override {
        return mGeneralSettings.getInfo(rawPropertyType);
    }

    qreal width() const { return getInfo(WIDTH_VALUE).realValue(); }
    qreal feather() const { return getInfo(FEATHER_VALUE).realValue(); }
    int stabilizerLevel() const { return getInfo(STABILIZATION_VALUE).intValue(); }
    bool pressureEnabled() const { return getInfo(PRESSURE_ENABLED).boolValue(); }
    bool invisibilityEnabled() const { return getInfo(INVISIBILITY_ENABLED).boolValue(); }
    bool featherEnabled() const { return getInfo(FEATHER_ENABLED).boolValue(); }
    bool AntiAliasingEnabled() const { return getInfo(ANTI_ALIASING_ENABLED).boolValue(); }
    bool fillContourEnabled() const { return getInfo(FILLCONTOUR_ENABLED).boolValue(); }

private:
    ToolSettings mGeneralSettings;
};

/// This struct is an example of how we can
/// share settings among tools rather than duplicating logic, eg. polyline uses settings from StrokeSettings.
/// The same could be done for PencilTool, BrushTool, Eraser etc...
struct PolylineSettings: public ToolSettingsBase
{
    enum Type {
        START               = 200,

        CLOSEDPATH_ENABLED  = START,
        BEZIERPATH_ENABLED  = 201,

        END                 = 299,
    };

    PolylineSettings() {
        general().addRange({START, END});

        general().insertIdentifiers({
            { CLOSEDPATH_ENABLED, "ClosedPathEnabled"},
            { BEZIERPATH_ENABLED, "BezierPathEnabled" }
        });
    }

    ToolSettings& general() override { return mStrokeSettings.general(); }
    const StrokeSettings& strokeSettings() const { return mStrokeSettings; }

    PropertyInfo getInfo(int rawPropertyType) const override {
        return mStrokeSettings.getInfo(rawPropertyType);
    }

    qreal width() const { return getInfo(StrokeSettings::WIDTH_VALUE).realValue(); }
    bool closedPathEnabled() const { return getInfo(CLOSEDPATH_ENABLED).boolValue(); }
    bool bezierPathEnabled() const { return getInfo(BEZIERPATH_ENABLED).boolValue(); }
    bool AntiAliasingEnabled() const { return getInfo(StrokeSettings::ANTI_ALIASING_ENABLED).boolValue(); }

private:
    StrokeSettings mStrokeSettings;
};

struct BucketSettings: public ToolSettingsBase
{
    enum Type {
        START                           = 300,
        FILLTHICKNESS_VALUE             = START,

        COLORTOLERANCE_VALUE            = 301,
        FILLEXPAND_VALUE                = 302,
        FILLLAYERREFERENCEMODE_VALUE    = 303,
        FILLMODE_VALUE                  = 304,
        COLORTOLERANCE_ENABLED          = 305,
        FILLEXPAND_ENABLED              = 306,

        END                             = 399,
    };

    BucketSettings() {
        mGeneralSettings.setRanges({ { START, END } });

        mGeneralSettings.insertIdentifiers({
            { FILLTHICKNESS_VALUE,          "FillThickness"},
            { COLORTOLERANCE_VALUE,         "ColorTolerance"},
            { COLORTOLERANCE_ENABLED,       "ColorToleranceEnabled"},
            { FILLEXPAND_VALUE,             "FillExpand"},
            { FILLEXPAND_ENABLED,           "FillExpandEnabled"},
            { COLORTOLERANCE_ENABLED,       "ColorToleranceEnabled"},
            { FILLLAYERREFERENCEMODE_VALUE, "FillReferenceMode"},
            { FILLMODE_VALUE,               "FillMode"}
        });
    }

    ToolSettings& general() override { return mGeneralSettings; }

    PropertyInfo getInfo(int rawPropertyType) const override {
        return mGeneralSettings.getInfo(rawPropertyType);
    }

    qreal fillThickness() const { return getInfo(FILLTHICKNESS_VALUE).realValue(); }
    int tolerance() const { return getInfo(COLORTOLERANCE_VALUE).intValue(); }
    int fillExpandAmount() const { return getInfo(FILLEXPAND_VALUE).intValue(); }
    int fillReferenceMode() const { return getInfo(FILLLAYERREFERENCEMODE_VALUE).intValue(); }
    int fillMode() const { return getInfo(FILLMODE_VALUE).intValue(); }
    bool colorToleranceEnabled() const { return getInfo(COLORTOLERANCE_ENABLED).boolValue(); }
    bool fillExpandEnabled() const { return getInfo(FILLEXPAND_ENABLED).boolValue(); }

private:
    ToolSettings mGeneralSettings;
};

struct CameraSettings: public ToolSettingsBase
{
    enum Type {
        START               = 400,
        SHOWPATH_ENABLED    = START,

        PATH_DOTCOLOR_TYPE  = 401,

        END                 = 499,
    };

    CameraSettings() {
        mGeneralSettings.setRanges({ { START, END }});

        mGeneralSettings.insertIdentifiers({
            { SHOWPATH_ENABLED,     "ShowPathEnabled"},
            { PATH_DOTCOLOR_TYPE,   "PathDotColorType"},
        });
    }

    ToolSettings& general() override { return mGeneralSettings; }

    PropertyInfo getInfo(int rawPropertyType) const override {
        return mGeneralSettings.getInfo(rawPropertyType);
    }

    bool showPathEnabled() const { return getInfo(SHOWPATH_ENABLED).boolValue(); }
    DotColorType dotColorType() const { return static_cast<DotColorType>(getInfo(PATH_DOTCOLOR_TYPE).intValue()); }

private:
    ToolSettings mGeneralSettings;
};

// Used by both select and move tool
struct TransformSettings: public ToolSettingsBase
{
    enum Type {
        START                        = 500,
        SHOWSELECTIONINFO_ENABLED    = START,
        ANTI_ALIASING_ENABLED        = 501,
        END                          = 599,
    };

    TransformSettings() {
        mGeneralSettings.setRanges({ { START, END } });

        mGeneralSettings.insertIdentifiers({
            { SHOWSELECTIONINFO_ENABLED,    "ShowSelectionInfoEnabled" },
            { ANTI_ALIASING_ENABLED,        "AntiAliasingEnabled" }
        });
    }

    ToolSettings& general() override { return mGeneralSettings; }

    PropertyInfo getInfo(int rawPropertyType) const override {
        return mGeneralSettings.getInfo(rawPropertyType);
    }

    bool showSelectionInfoEnabled() const { return getInfo(SHOWSELECTIONINFO_ENABLED).boolValue(); }
    bool antiAliasingEnabled() const { return getInfo(ANTI_ALIASING_ENABLED).boolValue(); }

private:
    ToolSettings mGeneralSettings;
};

#endif // TOOLSETTINGS_H
