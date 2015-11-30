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

Status PreferenceManager::onObjectLoaded( Object* )
{
    return Status::OK;
}

void PreferenceManager::loadPrefs()
{
    QSettings settings( PENCIL2D, PENCIL2D );

    set( SETTING::ANTIALIAS,        settings.value( SETTING_ANTIALIAS,          true ).toBool() );
    set( SETTING::BLURRYZOOM,       settings.value( SETTING_BLURRYZOOM,         false ).toBool() );
    set( SETTING::GRID,             settings.value( SETTING_SHOW_GRID,          false ).toBool() );
    set( SETTING::SHADOW,           settings.value( SETTING_SHADOW,             false ).toBool() );
    set( SETTING::PREV_ONION,       settings.value( SETTING_PREV_ONION,         false ).toBool() );
    set( SETTING::NEXT_ONION,       settings.value( SETTING_NEXT_ONION,         false ).toBool() );
    set( SETTING::CAMERABORDER,     settings.value( SETTING_CAMERABORDER,       false ).toBool() );
    set( SETTING::INVISIBLE_LINES,  settings.value( SETTING_INVISIBLE_LINES,    false ).toBool() );
    set( SETTING::OUTLINES,         settings.value( SETTING_OUTLINES,           false ).toBool() );
    set( SETTING::ONION_BLUE,       settings.value( SETTING_ONION_BLUE,         false ).toBool() );
    set( SETTING::ONION_RED,        settings.value( SETTING_ONION_RED,          false ).toBool() );
    set( SETTING::MIRROR_H,         false ); // Always off by default
    set( SETTING::MIRROR_V,         false ); // Always off by default
    set( SETTING::TOOL_CURSOR,      settings.value( SETTING_TOOL_CURSOR,        true ).toBool() );
    set( SETTING::HIGH_RESOLUTION,  settings.value( SETTING_HIGH_RESOLUTION,    true ).toBool() );

    set( SETTING::WINDOW_OPACITY,   settings.value( SETTING_WINDOW_OPACITY,     0 ).toInt() );
    set( SETTING::CURVE_SMOOTHING,  settings.value( SETTING_CURVE_SMOOTHING,    20 ).toInt() );

    set( SETTING::BACKGROUND_STYLE, settings.value( SETTING_BACKGROUND_STYLE,   "white" ).toString() );


    set( SETTING::AXIS, false );
//#define DRAW_AXIS
#ifdef DRAW_AXIS
    set( EFFECT::AXIS, true );
#endif

    emit prefsLoaded();
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
    return mStringSet.value(optionId);
}


// Set string value
//
void PreferenceManager::set(SETTING option, QString value)
{
    int optionId = static_cast< int >( option );
    if ( mStringSet[ optionId ] != value )
    {
        mStringSet[ optionId ] = value;
        emit optionChanged( option );
    }
    QSettings settings( PENCIL2D, PENCIL2D );
    switch ( option )
    {
    case SETTING::BACKGROUND_STYLE:
        settings.setValue( SETTING_BACKGROUND_STYLE, value );
        break;
    default:
        break;
    }
}

// Set int value
//
void PreferenceManager::set( SETTING option, int value )
{
    int optionId = static_cast< int >( option );
    if ( mIntegerSet[ optionId ] != value )
    {
        mIntegerSet[ optionId ] = value;
        emit optionChanged( option );
    }
    QSettings settings( PENCIL2D, PENCIL2D );
    switch ( option )
    {
    case SETTING::WINDOW_OPACITY:
        settings.setValue( SETTING_WINDOW_OPACITY, value );
        break;
    case SETTING::CURVE_SMOOTHING:
        settings.setValue( SETTING_CURVE_SMOOTHING, value );
        break;
    default:
        break;
    }
}

// Set bool value
//
void PreferenceManager::set( SETTING option, bool value )
{
    int optionId = static_cast< int >( option );
    if ( mBooleanSet[ optionId ] != value )
    {
        mBooleanSet[ optionId ] = value;
        emit optionChanged( option );
    }

    QSettings settings( PENCIL2D, PENCIL2D );
    switch ( option )
    {
    case SETTING::ANTIALIAS:
        settings.setValue( SETTING_ANTIALIAS, value );
        break;
    case SETTING::BLURRYZOOM:
        settings.setValue ( SETTING_BLURRYZOOM, value );
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
    case SETTING::AXIS:
        settings.setValue ( SETTING_AXIS, value );
        break;
    case SETTING::CAMERABORDER:
        settings.setValue ( SETTING_CAMERABORDER, value );
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
    case SETTING::MIRROR_H:
        settings.setValue ( SETTING_MIRROR_H, value );
        break;
    case SETTING::MIRROR_V:
        settings.setValue ( SETTING_MIRROR_V, value );
        break;
    case SETTING::TOOL_CURSOR:
        settings.setValue ( SETTING_TOOL_CURSOR, value );
        break;
    case SETTING::HIGH_RESOLUTION:
        settings.setValue ( SETTING_HIGH_RESOLUTION, value );
        break;
    default:
        break;
    }
}
