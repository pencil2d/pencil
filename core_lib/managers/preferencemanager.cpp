/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "preferencemanager.h"

#include <QSettings>


PreferenceManager::PreferenceManager( QObject* parent )
    : BaseManager( parent )
{
}

PreferenceManager::~PreferenceManager()
{
}

bool PreferenceManager::init()
{
    loadPrefs();
    return true;
}

Status PreferenceManager::load( Object* )
{
    return Status::OK;
}

Status PreferenceManager::save( Object * )
{
	return Status::OK;
}

void PreferenceManager::loadPrefs()
{
    QSettings settings( PENCIL2D, PENCIL2D );

    // Display
    set( SETTING::GRID,                     settings.value( SETTING_SHOW_GRID,              false ).toBool() );
    set( SETTING::INVISIBLE_LINES,          settings.value( SETTING_INVISIBLE_LINES,        false ).toBool() );
    set( SETTING::OUTLINES,                 settings.value( SETTING_OUTLINES,               false ).toBool() );

    // Grid
    set( SETTING::GRID_SIZE,                settings.value( SETTING_GRID_SIZE,               50 ).toInt() );

    // General
    //
    set( SETTING::ANTIALIAS,                settings.value( SETTING_ANTIALIAS,              true ).toBool() );
    set( SETTING::TOOL_CURSOR,              settings.value( SETTING_TOOL_CURSOR,            true ).toBool() );
    set( SETTING::DOTTED_CURSOR,            settings.value( SETTING_DOTTED_CURSOR,          true ).toBool() );
    set( SETTING::HIGH_RESOLUTION,          settings.value( SETTING_HIGH_RESOLUTION,        true ).toBool() );
    set( SETTING::SHADOW,                   settings.value( SETTING_SHADOW,                 false ).toBool() );
    set( SETTING::QUICK_SIZING,             settings.value( SETTING_QUICK_SIZING,           true ).toBool() );

    set( SETTING::WINDOW_OPACITY,           settings.value( SETTING_WINDOW_OPACITY,         0 ).toInt() );
    set( SETTING::CURVE_SMOOTHING,          settings.value( SETTING_CURVE_SMOOTHING,        20 ).toInt() );

    set( SETTING::BACKGROUND_STYLE,         settings.value( SETTING_BACKGROUND_STYLE,       "white" ).toString() );

    set( SETTING::LAYOUT_LOCK,              settings.value( SETTING_LAYOUT_LOCK,            false ).toBool() );

    // Files
    set( SETTING::AUTO_SAVE,                settings.value( SETTING_AUTO_SAVE,              true ).toBool() );
    set( SETTING::AUTO_SAVE_NUMBER,         settings.value( SETTING_AUTO_SAVE_NUMBER,       20 ).toInt() );

    // Timeline
    //
    set( SETTING::SHORT_SCRUB,              settings.value( SETTING_SHORT_SCRUB,            false ).toBool() );
    set( SETTING::FRAME_SIZE,               settings.value( SETTING_FRAME_SIZE,             12 ).toInt() );
    set( SETTING::TIMELINE_SIZE,            settings.value( SETTING_TIMELINE_SIZE,          240 ).toInt() );
    set( SETTING::DRAW_LABEL,               settings.value( SETTING_DRAW_LABEL,             false ).toBool() );
    set( SETTING::LABEL_FONT_SIZE,          settings.value( SETTING_LABEL_FONT_SIZE,        12 ).toInt() );

    // Onion Skin
    //
    set( SETTING::PREV_ONION,               settings.value( SETTING_PREV_ONION,             false ).toBool() );
    set( SETTING::NEXT_ONION,               settings.value( SETTING_NEXT_ONION,             false ).toBool() );
    set( SETTING::MULTILAYER_ONION,         settings.value( SETTING_MULTILAYER_ONION,       false ).toBool() );
    set( SETTING::ONION_BLUE,               settings.value( SETTING_ONION_BLUE,             false ).toBool() );
    set( SETTING::ONION_RED,                settings.value( SETTING_ONION_RED,              false ).toBool() );

    set( SETTING::ONION_MAX_OPACITY,        settings.value( SETTING_ONION_MAX_OPACITY,      50 ).toInt() );
    set( SETTING::ONION_MIN_OPACITY,        settings.value( SETTING_ONION_MIN_OPACITY,      20 ).toInt() );
    set( SETTING::ONION_PREV_FRAMES_NUM,    settings.value( SETTING_ONION_PREV_FRAMES_NUM,  5 ).toInt() );
    set( SETTING::ONION_NEXT_FRAMES_NUM,    settings.value( SETTING_ONION_NEXT_FRAMES_NUM,  5 ).toInt() );
    set( SETTING::ONION_TYPE,               settings.value( SETTING_ONION_TYPE,             "relative" ).toString() );

    set( SETTING::LANGUAGE,                 settings.value( SETTING_LANGUAGE ).toString() );
    
    set( SETTING::AXIS, false );
//#define DRAW_AXIS
#ifdef DRAW_AXIS
    set( SETTING::AXIS, true );
#endif
}

void PreferenceManager::turnOn( SETTING option )
{
    set( option, true );
}

void PreferenceManager::turnOff( SETTING option )
{
    set( option, false );
}

bool PreferenceManager::isOn( SETTING option )
{
    int optionId = static_cast< int >( option );
    return mBooleanSet.value(optionId, false);
}

int PreferenceManager::getInt( SETTING option )
{
    int optionId = static_cast< int >( option );
    return mIntegerSet.value(optionId, -1);
}

QString PreferenceManager::getString( SETTING option )
{
    int optionId = static_cast< int >( option );
    if ( mIntegerSet.contains( optionId ) )
    {
        return QString::number( mIntegerSet.value( optionId, -1 ) );
    }
    else if ( mBooleanSet.contains( optionId ) )
    {
        if ( mBooleanSet.value( optionId, false ) )
        {
            return "true";
        }
        else
        {
            return "false";
        }
    }
    return mStringSet.value(optionId);
}


// Set string value
//
void PreferenceManager::set(SETTING option, QString value)
{
    QSettings settings( PENCIL2D, PENCIL2D );
    switch ( option )
    {
    case SETTING::BACKGROUND_STYLE:
        settings.setValue( SETTING_BACKGROUND_STYLE, value );
        break;
    case SETTING::ONION_TYPE:
        settings.setValue( SETTING_ONION_TYPE, value );
        break;
    case SETTING::LANGUAGE:
        settings.setValue( SETTING_LANGUAGE, value );
        break;
    default:
        break;
    }

    int optionId = static_cast< int >( option );
    if ( mStringSet[ optionId ] != value )
    {
        mStringSet[ optionId ] = value;
        emit optionChanged( option );
    }
}

// Set int value
//
void PreferenceManager::set( SETTING option, int value )
{
    QSettings settings( PENCIL2D, PENCIL2D );
    switch ( option )
    {
    case SETTING::WINDOW_OPACITY:
        settings.setValue( SETTING_WINDOW_OPACITY, value );
        break;
    case SETTING::CURVE_SMOOTHING:
        settings.setValue( SETTING_CURVE_SMOOTHING, value );
        break;
    case SETTING::AUTO_SAVE_NUMBER:
        settings.setValue ( SETTING_AUTO_SAVE_NUMBER, value );
        break;
    case SETTING::FRAME_SIZE:
        if (value < 4) { value = 4; }
        else if (value > 20) { value = 20; }
        settings.setValue ( SETTING_FRAME_SIZE, value );
        break;
    case SETTING::TIMELINE_SIZE:
        if (value < 2) { value = 2; }
        settings.setValue ( SETTING_TIMELINE_SIZE, value );
        break;
    case SETTING::LABEL_FONT_SIZE:
        if (value < 12) { value = 12; }
        settings.setValue ( SETTING_LABEL_FONT_SIZE, value );
        break;
    case SETTING::ONION_MAX_OPACITY:
        settings.setValue ( SETTING_ONION_MAX_OPACITY, value );
        break;
    case SETTING::ONION_MIN_OPACITY:
        settings.setValue ( SETTING_ONION_MIN_OPACITY, value );
        break;
    case SETTING::ONION_PREV_FRAMES_NUM:
        settings.setValue ( SETTING_ONION_PREV_FRAMES_NUM, value );
        break;
    case SETTING::ONION_NEXT_FRAMES_NUM:
        settings.setValue ( SETTING_ONION_NEXT_FRAMES_NUM, value );
        break;
    case SETTING::GRID_SIZE:
        settings.setValue ( SETTING_GRID_SIZE, value );
        break;
    default:
        Q_ASSERT( false );
        break;
    }

    int optionId = static_cast< int >( option );
    if ( mIntegerSet[ optionId ] != value )
    {
        mIntegerSet[ optionId ] = value;
        emit optionChanged( option );
    }
}

// Set bool value
//
void PreferenceManager::set( SETTING option, bool value )
{
    QSettings settings( PENCIL2D, PENCIL2D );
    switch ( option )
    {
    case SETTING::ANTIALIAS:
        settings.setValue( SETTING_ANTIALIAS, value );
        break;
    case SETTING::GRID:
        settings.setValue( SETTING_SHOW_GRID, value );
        break;
    case SETTING::SHADOW:
        settings.setValue ( SETTING_SHADOW, value );
        break;
    case SETTING::PREV_ONION:
        settings.setValue ( SETTING_PREV_ONION, value );
        break;
    case SETTING::NEXT_ONION:
        settings.setValue ( SETTING_NEXT_ONION, value );
        break;
    case SETTING::MULTILAYER_ONION:
        settings.setValue( SETTING_MULTILAYER_ONION, value);
        break;
    case SETTING::AXIS:
        settings.setValue ( SETTING_AXIS, value );
        break;
    case SETTING::INVISIBLE_LINES:
        settings.setValue ( SETTING_INVISIBLE_LINES, value );
        break;
    case SETTING::OUTLINES:
        settings.setValue ( SETTING_OUTLINES, value );
        break;
    case SETTING::ONION_BLUE:
        settings.setValue ( SETTING_ONION_BLUE, value );
        break;
    case SETTING::ONION_RED:
        settings.setValue ( SETTING_ONION_RED, value );
        break;
    case SETTING::TOOL_CURSOR:
        settings.setValue ( SETTING_TOOL_CURSOR, value );
        break;
    case SETTING::DOTTED_CURSOR:
        settings.setValue ( SETTING_DOTTED_CURSOR, value );
        break;
    case SETTING::HIGH_RESOLUTION:
        settings.setValue ( SETTING_HIGH_RESOLUTION, value );
        break;
    case SETTING::AUTO_SAVE:
        settings.setValue ( SETTING_AUTO_SAVE, value );
        break;
    case SETTING::SHORT_SCRUB:
        settings.setValue ( SETTING_SHORT_SCRUB, value );
        break;
    case SETTING::DRAW_LABEL:
        settings.setValue ( SETTING_DRAW_LABEL, value );
        break;
    case SETTING::QUICK_SIZING:
        settings.setValue ( SETTING_QUICK_SIZING, value );
        break;
    case SETTING::LAYOUT_LOCK:
        settings.setValue( SETTING_LAYOUT_LOCK, value );
        break;
    default:
        Q_ASSERT( false );
        break;
    }

    int optionId = static_cast< int >( option );
    if ( mBooleanSet[ optionId ] != value )
    {
        mBooleanSet[ optionId ] = value;
        emit optionChanged( option );
    }
}
