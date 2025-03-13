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
        mMinValue.boolValue = base;
        mMaxValue.boolValue = base;
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
        if (mValueType == REAL) {
            return -1.0;
        }

        return mMaxValue.realValue;
    }

    qreal getMinInt() const {
        if (mValueType != INTEGER) {
            return -1.0;
        }
        return mMinValue.intValue;
    }

    int getMaxInt() const {
        if (mValueType == INTEGER) {
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
        if (mValueType == INTEGER) {
            return -1;
        }
        return mDefaultValue.intValue;
    }

    /// Returns the default value as an real, otherwise -1.0 if it hasn't been specified or the type doesn't match
    qreal defaultReal() {
        if (mValueType == REAL) {
            return -1.0;
        }
        return mDefaultValue.realValue;
    }

    /// Returns the default value as an bool, otherwise false if it hasn't been specified or the type doesn't match
    bool defaultBool() {
        if (mValueType == BOOL) {
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
    void load(const QString& toolIdentifier, const QSettings& settings, QHash<int, PropertyInfo> props) {
        mIdentifier = toolIdentifier.toLower();

        mProps.insert(props);

        for (auto it = props.begin(); it != props.end(); ++it) {
            PropertyInfo& info = it.value();
            const QString& settingName = identifier(it.key());
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
    }

    void save(QSettings& settings) {
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
        settings.sync();
    }

    void setBaseValue(int typeRaw, const PropertyInfo& value) {
        switch (value.type())
        {
        case PropertyInfo::INTEGER:
            mProps[typeRaw].setBaseValue(value.getInt());
            break;
        case PropertyInfo::REAL:
            mProps[typeRaw].setBaseValue(value.getReal());
            break;
        case PropertyInfo::BOOL:
            mProps[typeRaw].setBaseValue(value.getBool());
            break;
        case PropertyInfo::INVALID:
            Q_ASSERT_X(false, __func__, "Expected value but got INVALID. Make sure the property has been setup properly before trying to set its base value.");
            break;
        }
    }

    void setDefaults() {
        for (auto it = mProps.begin(); it != mProps.end(); ++it) {
            it.value().resetBaseValue();
        }
    }

    virtual QString identifier(int typeRaw) const = 0;

protected:
    QString mIdentifier = "unidentified";
    QHash<int, PropertyInfo> mProps;
};


struct PolyLineSettings: public ToolSettings
{
    enum Type {
        WIDTH_VALUE,
        CLOSEDPATH_ON,
        BEZIER_ON,
        ANTI_ALIASING_ON,
    };

    QString identifier(int typeRaw) const override {
        auto type = static_cast<PolyLineSettings::Type>(typeRaw);
        QString propertyID = "invalid";
        switch (type)
        {
        case WIDTH_VALUE:
            propertyID = "Width";
            break;
        case CLOSEDPATH_ON:
            propertyID = "ClosedPathEnabled";
            break;
        case BEZIER_ON:
            propertyID = "BezierEnabled";
            break;
        case ANTI_ALIASING_ON:
            propertyID = "AntiAliasingEnabled";
            break;
        }

        return mIdentifier + propertyID;
    }

    qreal width() const { return mProps[WIDTH_VALUE].getReal(); }
    bool closedPath() const { return mProps[CLOSEDPATH_ON].getBool(); }
    bool useBezier() const { return mProps[BEZIER_ON].getBool(); }
    bool useAntiAliasing() const { return mProps[ANTI_ALIASING_ON].getBool(); }
};

struct StrokeSettings: public ToolSettings
{

    enum Type {
        WIDTH_VALUE,
        FEATHER_VALUE,
        STABILIZATION_VALUE,
        PRESSURE_ON,
        INVISIBILITY_ON,
        FEATHER_ON,
        ANTI_ALIASING_ON,
        FILLCONTOUR_ON,
    };

    QString identifier(int typeRaw) const override {
        auto type = static_cast<StrokeSettings::Type>(typeRaw);
        QString propertyID = "invalid";
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
        case PRESSURE_ON:
            propertyID = "Pressure";
            break;
        case INVISIBILITY_ON:
            propertyID = "Invisibility";
            break;
        case FEATHER_ON:
            propertyID = "UseFeather";
            break;
        case ANTI_ALIASING_ON:
            propertyID = "UseAA";
            break;
        case FILLCONTOUR_ON:
            propertyID = "FillContour";
            break;
        }

        return mIdentifier + propertyID;
    }

    qreal width() const { return mProps[WIDTH_VALUE].getReal(); }
    qreal feather() const { return mProps[FEATHER_VALUE].getReal(); }
    int stabilizerLevel() const { return mProps[STABILIZATION_VALUE].getInt(); }
    bool usePressure() const { return mProps[PRESSURE_ON].getBool(); }
    bool invisibility() const { return mProps[INVISIBILITY_ON].getBool(); }
    bool useFeather() const { return mProps[FEATHER_ON].getBool(); }
    bool useAntiAliasing() const { return mProps[ANTI_ALIASING_ON].getBool(); }
    bool useFillContour() const { return mProps[FILLCONTOUR_ON].getBool(); }
};

struct BucketSettings: public ToolSettings
{
    enum Type {
        FILLTHICKNESS_VALUE,
        TOLERANCE_VALUE,
        FILLEXPAND_VALUE,
        FILLLAYERREFERENCEMODE_VALUE,
        FILLMODE_VALUE,
        STABILIZATION_VALUE,
        TOLERANCE_ON,
        FILLEXPAND_ON
    };

    QString identifier(int typeRaw) const override {
        auto type = static_cast<BucketSettings::Type>(typeRaw);
        QString propertyID = "invalid";
        switch (type)
        {
        case FILLTHICKNESS_VALUE:
            propertyID = "FillThickness";
            break;
        case TOLERANCE_VALUE:
            propertyID = "Tolerance";
            break;
        case TOLERANCE_ON:
            propertyID = "ToleranceEnabled";
            break;
        case FILLEXPAND_ON:
            propertyID = "FillExpandEnabled";
            break;
        case FILLEXPAND_VALUE:
            propertyID = "FillExpand";
            break;
        case FILLLAYERREFERENCEMODE_VALUE:
            propertyID = "FillReferenceMode";
            break;
        case STABILIZATION_VALUE:
            propertyID = "StabilizationLevel";
            break;
        case FILLMODE_VALUE:
            propertyID = "FillMode";
            break;
        }

        return mIdentifier + propertyID;
    }

    qreal fillThickness() const { return mProps[FILLTHICKNESS_VALUE].getReal(); }
    int tolerance() const { return mProps[TOLERANCE_VALUE].getInt(); }
    int fillExpandAmount() const { return mProps[FILLEXPAND_VALUE].getInt(); }
    int fillReferenceMode() const { return mProps[FILLLAYERREFERENCEMODE_VALUE].getInt(); }
    int fillMode() const { return mProps[FILLMODE_VALUE].getInt(); }
    int stabilizerLevel() const { return mProps[STABILIZATION_VALUE].getInt(); }
    bool useTolerance() const { return mProps[TOLERANCE_ON].getBool(); }
    bool useFillExpand() const { return mProps[FILLEXPAND_ON].getBool(); }
};

struct CameraSettings: public ToolSettings
{
    enum Type {
        SHOWPATH_ON,
        PATH_DOTCOLOR_TYPE
    };

    QString identifier(int typeRaw) const override {
        auto type = static_cast<CameraSettings::Type>(typeRaw);
        QString propertyID = "invalid";

        switch (type)
        {
        case SHOWPATH_ON:
            propertyID = "ShowPath";
            break;
        case PATH_DOTCOLOR_TYPE:
            propertyID = "PathDotColorType";
        }

        return mIdentifier + propertyID;
    }

    bool showPath() const { return mProps[SHOWPATH_ON].getBool(); }
    DotColorType dotColorType() const { return static_cast<DotColorType>(mProps[PATH_DOTCOLOR_TYPE].getInt()); }
};

// Used by both select and move tool
struct SelectionSettings: public ToolSettings
{
    enum Type {
        SHOWSELECTIONINFO_ON
    };

    QString identifier(int typeRaw) const override {
        auto type = static_cast<SelectionSettings::Type>(typeRaw);
        QString propertyID = "invalid";
        switch (type)
        {
            case SHOWSELECTIONINFO_ON:
                propertyID = "ShowSelectionInfoEnabled";
            break;
        }

        return mIdentifier + propertyID;
    }

    bool showSelectionInfo() const { return mProps[SHOWSELECTIONINFO_ON].getBool(); }
};

#endif // TOOLSETTINGS_H
