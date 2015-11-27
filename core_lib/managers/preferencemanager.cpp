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

    set( EFFECT::ANTIALIAS,         settings.value( SETTING_ANTIALIAS,        true ).toBool() );
    set( EFFECT::BLURRYZOOM,        settings.value( SETTING_BLURRYZOOM,       false ).toBool() );
    set( EFFECT::GRID,              settings.value( SETTING_SHOW_GRID,        false ).toBool() );
    set( EFFECT::SHADOW,            settings.value( SETTING_SHADOW,           false ).toBool() );
    set( EFFECT::PREV_ONION,        settings.value( SETTING_PREV_ONION,       false ).toBool() );
    set( EFFECT::NEXT_ONION,        settings.value( SETTING_NEXT_ONION,       false ).toBool() );
    set( EFFECT::CAMERABORDER,      settings.value( SETTING_CAMERABORDER,     false ).toBool() );
    set( EFFECT::INVISIBLE_LINES,   settings.value( SETTING_INVISIBLE_LINES,  false ).toBool() );
    set( EFFECT::OUTLINES,          settings.value( SETTING_OUTLINES,         false ).toBool() );
    set( EFFECT::ONION_BLUE,        settings.value( SETTING_ONION_BLUE,       false ).toBool() );
    set( EFFECT::ONION_RED,         settings.value( SETTING_ONION_RED,        false ).toBool() );
    set( EFFECT::MIRROR_H,          false ); // Always off by default
    set( EFFECT::MIRROR_V,          false ); // Always off by default

    set( EFFECT::AXIS, false );
//#define DRAW_AXIS
#ifdef DRAW_AXIS
    set( EFFECT::AXIS, true );
#endif

    emit prefsLoaded();
}

void PreferenceManager::turnOn( EFFECT effect )
{
    set( effect, true );
}

void PreferenceManager::turnOff( EFFECT effect )
{
    set( effect, false );
}

bool PreferenceManager::isOn( EFFECT effect )
{
    int effectId = static_cast< int >( effect );
    return mEffectSet.value(effectId, false);
}

void PreferenceManager::set( EFFECT effect, bool value )
{
    int effectId = static_cast< int >( effect );
    if ( mEffectSet[ effectId ] != value )
    {
        mEffectSet[ effectId ] = value;
        emit effectChanged( effect, value );
    }

    QSettings settings( PENCIL2D, PENCIL2D );
    switch ( effect )
    {
    case EFFECT::ANTIALIAS:
        settings.setValue( SETTING_ANTIALIAS, value );
        break;
    case EFFECT::BLURRYZOOM:
        settings.setValue ( SETTING_BLURRYZOOM, value );
        break;
    case EFFECT::GRID:
        settings.setValue( SETTING_SHOW_GRID, value );
        break;
    case EFFECT::SHADOW:
        settings.setValue ( SETTING_SHADOW, value );
        break;
    case EFFECT::PREV_ONION:
        settings.setValue ( SETTING_PREV_ONION, value );
        break;
    case EFFECT::NEXT_ONION:
        settings.setValue ( SETTING_NEXT_ONION, value );
        break;
    case EFFECT::AXIS:
        settings.setValue ( SETTING_AXIS, value );
        break;
    case EFFECT::CAMERABORDER:
        settings.setValue ( SETTING_CAMERABORDER, value );
        break;
    case EFFECT::INVISIBLE_LINES:
        settings.setValue ( SETTING_INVISIBLE_LINES, value );
        break;
    case EFFECT::OUTLINES:
        settings.setValue ( SETTING_OUTLINES, value );
        break;
    case EFFECT::ONION_BLUE:
        settings.setValue ( SETTING_ONION_BLUE, value );
        break;
    case EFFECT::ONION_RED:
        settings.setValue ( SETTING_ONION_RED, value );
        break;
    case EFFECT::MIRROR_H:
        settings.setValue ( SETTING_MIRROR_H, value );
        break;
    case EFFECT::MIRROR_V:
        settings.setValue ( SETTING_MIRROR_V, value );
        break;
    default:
        break;
    }
}
