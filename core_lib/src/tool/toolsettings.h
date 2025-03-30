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
        mBaseValue.intValue = -1;
    }
    PropertyInfo(int min, int max, int defaultValue)
        : mValueType(INTEGER) {
        setBaseValue(defaultValue);
        mMinValue.intValue = min;
        mMaxValue.intValue = max;
        mDefaultValue.intValue = defaultValue;
    }
    PropertyInfo(qreal min, qreal max, qreal defaultValue)
        : mValueType(REAL) {
        setBaseValue(defaultValue);
        mMinValue.realValue = min;
        mMaxValue.realValue = max;
        mDefaultValue.realValue = defaultValue;
    }

    PropertyInfo(bool base, bool defaultValue)
        : mValueType(BOOL) {
        setBaseValue(base);
        mMinValue.boolValue = false;
        mMaxValue.boolValue = true;
        mDefaultValue.boolValue = defaultValue;
    }

    PropertyInfo(bool base) : PropertyInfo(base, base) {}
    PropertyInfo(int base) : PropertyInfo(base, base, base) {}
    PropertyInfo(qreal base) : PropertyInfo(base, base, base) {}

    void setBaseValue(int newValue) {
        Q_ASSERT(mValueType == INTEGER);
        mBaseValue.intValue = newValue;
    }
    void setBaseValue(qreal newValue) {
        Q_ASSERT(mValueType == REAL);
        mBaseValue.realValue = newValue;
    }
    void setBaseValue(bool newValue) {
        Q_ASSERT(mValueType == BOOL);
        mBaseValue.boolValue = newValue;
    }

    int getInt() const {
        if (mValueType != INTEGER) {
            Q_ASSERT(false);
            return -1;
        }

        return qBound(mMinValue.intValue, mBaseValue.intValue, mMaxValue.intValue);
    }

    qreal getReal() const {
        if (mValueType != REAL) {
            Q_ASSERT(false);
            return -1.0;
        }

        return qBound(mMinValue.realValue, mBaseValue.realValue, mMaxValue.realValue);
    }

    bool getBool() const {
        if (mValueType != BOOL) {
            Q_ASSERT(false);
            return false;
        }
        return mBaseValue.boolValue;
    }

    qreal getMinReal() const {
        if (mValueType != REAL) {
            return -1.0;
        }
        return mMinValue.realValue;
    }

    qreal getMaxReal() const {
        if (mValueType != REAL) {
            return -1.0;
        }

        return mMaxValue.realValue;
    }

    int getMinInt() const {
        if (mValueType != INTEGER) {
            return -1;
        }
        return mMinValue.intValue;
    }

    int getMaxInt() const {
        if (mValueType != INTEGER) {
            return -1;
        }

        return mMaxValue.intValue;
    }

    void resetBaseValue() {
        switch (mValueType) {
        case INTEGER:
            mBaseValue.intValue = mDefaultValue.intValue;
            break;
        case REAL:
            mBaseValue.realValue = mDefaultValue.realValue;
            break;
        case BOOL:
            mBaseValue.boolValue = mDefaultValue.boolValue;
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
        return mDefaultValue.intValue;
    }

    /// Returns the default value as an real, otherwise -1.0 if it hasn't been specified or the type doesn't match
    qreal defaultReal() {
        if (mValueType != REAL) {
            return -1.0;
        }
        return mDefaultValue.realValue;
    }

    /// Returns the default value as an bool, otherwise false if it hasn't been specified or the type doesn't match
    bool defaultBool() {
        if (mValueType != BOOL) {
            return false;
        }
        return mDefaultValue.boolValue;
    }

    ValueType type() const { return mValueType; }
    bool isType(ValueType type) const { return mValueType == type; }
    bool hasValue() {
        return mValueType != INVALID;
    }

private:

    // This union is only meant ot store simple values.
    // Do not attempt to store complicated objects in here.
    union ValueUnion {
        int intValue;
        qreal realValue;
        bool boolValue;
    };

    ValueType mValueType;
    ValueUnion mBaseValue;
    ValueUnion mMinValue;
    ValueUnion mMaxValue;
    ValueUnion mDefaultValue;
};

struct ToolSettings
{
    virtual ~ToolSettings() {}

    void load(const QString& toolIdentifier, QSettings& settings, QHash<int, PropertyInfo> props) {
        mIdentifier = toolIdentifier.toLower();

        settings.beginGroup(mIdentifier);
        for (auto it = props.begin(); it != props.end(); ++it) {

            PropertyInfo info = it.value();
            const QString& settingName = identifier(it.key());
            loadProperty(settingName, info, settings);

            mProps.insert(it.key(), info);
        }
        settings.endGroup();
    }

    void save(QSettings& settings) {
        settings.beginGroup(mIdentifier);
        settings.setValue(mVersionKey, mVersion);

        for (auto it = mProps.begin(); it != mProps.end(); ++it) {

            QString propertyId = identifier(it.key());
            switch (it.value().type())
            {
            case PropertyInfo::INTEGER:
                settings.setValue(propertyId, it.value().getInt());
                break;
            case PropertyInfo::REAL:
                settings.setValue(propertyId, it.value().getReal());
                break;
            case PropertyInfo::BOOL:
                settings.setValue(propertyId, it.value().getBool());
                break;
            case PropertyInfo::INVALID:
                Q_ASSERT_X(false, __func__, "Wrong state, expected a value type but got INVALID. You've probably misconfigured the property. "
                                        "Ensure the property has been setup correctly and try again.");
                break;
            }
        }
        settings.endGroup();
        settings.sync();
    }

    void setBaseValue(int rawType, const PropertyInfo& value)
    {
        switch (value.type())
        {
        case PropertyInfo::INTEGER:
            mProps[rawType].setBaseValue(value.getInt());
            break;
        case PropertyInfo::REAL:
            mProps[rawType].setBaseValue(value.getReal());
            break;
        case PropertyInfo::BOOL:
            mProps[rawType].setBaseValue(value.getBool());
            break;
        case PropertyInfo::INVALID:
            Q_ASSERT_X(false, __func__, "Expected value but got INVALID. Make sure the property has been setup properly before trying to set its base value.");
            break;
        }
    }

    PropertyInfo getInfo(int rawPropertyType) const
    {
        return mProps[rawPropertyType];
    }

    void setDefaults() {
        for (auto it = mProps.begin(); it != mProps.end(); ++it) {
            it.value().resetBaseValue();
        }
    }

    /// Use this function to load old tool property keys before being migrated
    bool requireMigration(QSettings& settings, int version) {
        settings.beginGroup(mIdentifier);

        if (settings.childKeys().isEmpty()) {
            return true;
        }
        return settings.value(mVersionKey).isNull() || (version > settings.value(mVersionKey).toInt());
    }

protected:

    virtual QString identifier(int) const {
        return "Invalid";
    }

    QString mIdentifier = "unidentified";
    QHash<int, PropertyInfo> mProps;

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

    int mVersion = 1;
    QString mVersionKey = "Version";
};

struct StrokeSettings: public ToolSettings
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

    QString identifier(int typeRaw) const override {
        auto type = static_cast<StrokeSettings::Type>(typeRaw);
        QString propertyID = ToolSettings::identifier(typeRaw);
        switch (type)
        {
        case WIDTH_VALUE:
            propertyID = "Width";
            break;
        case FEATHER_VALUE:
            propertyID = "Feather";
            break;
        case STABILIZATION_VALUE:
            propertyID = "LineStabilization";
            break;
        case PRESSURE_ENABLED:
            propertyID = "Pressure";
            break;
        case INVISIBILITY_ENABLED:
            propertyID = "Invisibility";
            break;
        case FEATHER_ENABLED:
            propertyID = "FeatherEnabled";
            break;
        case ANTI_ALIASING_ENABLED:
            propertyID = "AntiAliasingEnabled";
            break;
        case FILLCONTOUR_ENABLED:
            propertyID = "FillContourEnabled";
            break;
        case END:
            break;
        }

        return propertyID;
    }

    qreal width() const { return mProps[WIDTH_VALUE].getReal(); }
    qreal feather() const { return mProps[FEATHER_VALUE].getReal(); }
    int stabilizerLevel() const { return mProps[STABILIZATION_VALUE].getInt(); }
    bool pressureEnabled() const { return mProps[PRESSURE_ENABLED].getBool(); }
    bool invisibilityEnabled() const { return mProps[INVISIBILITY_ENABLED].getBool(); }
    bool featherEnabled() const { return mProps[FEATHER_ENABLED].getBool(); }
    bool AntiAliasingEnabled() const { return mProps[ANTI_ALIASING_ENABLED].getBool(); }
    bool fillContourEnabled() const { return mProps[FILLCONTOUR_ENABLED].getBool(); }
};

/// This struct is an example of how we can
/// share settings among tools rather than duplicating logic, eg. polyline uses settings from StrokeSettings.
/// The same could be done for PencilTool, BrushTool, Eraser etc...
struct PolylineSettings: public StrokeSettings
{
    enum Type {
        START               = 200,

        CLOSEDPATH_ENABLED  = START,
        BEZIERPATH_ENABLED  = 201,

        END                 = 299,
    };

    QString identifier(int typeRaw) const override {
        auto type = static_cast<PolylineSettings::Type>(typeRaw);
        QString propertyID = ToolSettings::identifier(typeRaw);
        switch (type)
        {
        case CLOSEDPATH_ENABLED:
            propertyID = "ClosedPathEnabled";
            break;
        case BEZIERPATH_ENABLED:
            propertyID = "BezierPathEnabled";
            break;
        default:
            propertyID = StrokeSettings::identifier(typeRaw);
        }

        return propertyID;
    }

    qreal width() const { return mProps[StrokeSettings::WIDTH_VALUE].getReal(); }
    bool closedPathEnabled() const { return mProps[CLOSEDPATH_ENABLED].getBool(); }
    bool bezierPathEnabled() const { return mProps[BEZIERPATH_ENABLED].getBool(); }
    bool AntiAliasingEnabled() const { return mProps[StrokeSettings::ANTI_ALIASING_ENABLED].getBool(); }
};

struct BucketSettings: public ToolSettings
{
    enum Type {
        START                           = 300,
        FILLTHICKNESS_VALUE             = START,

        COLORTOLERANCE_VALUE            = 301,
        FILLEXPAND_VALUE                = 302,
        FILLLAYERREFERENCEMODE_VALUE    = 304,
        FILLMODE_VALUE                  = 305,
        COLORTOLERANCE_ENABLED          = 307,
        FILLEXPAND_ENABLED              = 308,

        END                             = 399,
    };

    QString identifier(int typeRaw) const override {
        auto type = static_cast<BucketSettings::Type>(typeRaw);
        QString propertyID = ToolSettings::identifier(typeRaw);
        switch (type)
        {
        case FILLTHICKNESS_VALUE:
            propertyID = "FillThickness";
            break;
        case COLORTOLERANCE_VALUE:
            propertyID = "ColorTolerance";
            break;
        case COLORTOLERANCE_ENABLED:
            propertyID = "ColorToleranceEnabled";
            break;
        case FILLEXPAND_ENABLED:
            propertyID = "FillExpandEnabled";
            break;
        case FILLEXPAND_VALUE:
            propertyID = "FillExpand";
            break;
        case FILLLAYERREFERENCEMODE_VALUE:
            propertyID = "FillReferenceMode";
            break;
        case FILLMODE_VALUE:
            propertyID = "FillMode";
            break;
        case END:
            break;
        }

        return  propertyID;
    }

    qreal fillThickness() const { return mProps[FILLTHICKNESS_VALUE].getReal(); }
    int tolerance() const { return mProps[COLORTOLERANCE_VALUE].getInt(); }
    int fillExpandAmount() const { return mProps[FILLEXPAND_VALUE].getInt(); }
    int fillReferenceMode() const { return mProps[FILLLAYERREFERENCEMODE_VALUE].getInt(); }
    int fillMode() const { return mProps[FILLMODE_VALUE].getInt(); }
    bool colorToleranceEnabled() const { return mProps[COLORTOLERANCE_ENABLED].getBool(); }
    bool fillExpandEnabled() const { return mProps[FILLEXPAND_ENABLED].getBool(); }
};

struct CameraSettings: public ToolSettings
{
    enum Type {
        START               = 400,
        SHOWPATH_ENABLED    = START,

        PATH_DOTCOLOR_TYPE  = 401,

        END                 = 499,
    };

    QString identifier(int typeRaw) const override {
        auto type = static_cast<CameraSettings::Type>(typeRaw);
        QString propertyID = ToolSettings::identifier(typeRaw);

        switch (type)
        {
        case SHOWPATH_ENABLED:
            propertyID = "ShowPathEnabled";
            break;
        case PATH_DOTCOLOR_TYPE:
            propertyID = "PathDotColorType";
        case END:
            break;
        }

        return propertyID;
    }

    bool showPathEnabled() const { return mProps[SHOWPATH_ENABLED].getBool(); }
    DotColorType dotColorType() const { return static_cast<DotColorType>(mProps[PATH_DOTCOLOR_TYPE].getInt()); }
};

// Used by both select and move tool
struct TransformSettings: public ToolSettings
{
    enum Type {
        START                   = 500,
        SHOWSELECTIONINFO_ENABLED    = START,

        END                     = 599,
    };

    QString identifier(int typeRaw) const override {
        auto type = static_cast<TransformSettings::Type>(typeRaw);
        QString propertyID = ToolSettings::identifier(typeRaw);

        switch (type)
        {
            case SHOWSELECTIONINFO_ENABLED:
                propertyID = "ShowSelectionInfoEnabled";
                break;
            case END:
                break;
        }

        return propertyID;
    }

    bool showSelectionInfoEnabled() const { return mProps[SHOWSELECTIONINFO_ENABLED].getBool(); }
};

#endif // TOOLSETTINGS_H
