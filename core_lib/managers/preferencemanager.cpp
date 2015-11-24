#include "preferencemanager.h"

#include <QSettings>
#include "pencildef.h"


PreferenceManager::PreferenceManager( QObject* parent )
    : BaseManager( parent )
{
}

PreferenceManager::~PreferenceManager()
{
}

bool PreferenceManager::init()
{
    QSettings settings( PENCIL2D, PENCIL2D );
    
    set( EFFECT::ANTIALIAS,   settings.value( SETTING_ANTIALIAS, false ).toBool() );
    set( EFFECT::BLURRYZOOM,  settings.value( SETTING_BLURRYZOOM, false ).toBool() );
    set( EFFECT::GRID,        settings.value( SETTING_SHOW_GRID, false ).toBool() );

    return true;
}

Status PreferenceManager::onObjectLoaded( Object* )
{
    return Status::OK;
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
    size_t index = static_cast< size_t >( effect );
    return mEffectSet[ index ];
}

void PreferenceManager::set( EFFECT effect, bool b )
{
    size_t index = static_cast< size_t >( effect );

    if ( mEffectSet[ index ] != b )
    {
        mEffectSet[ index ] = b;
        emit preferenceChanged( effect );
    }

    QSettings settings( PENCIL2D, PENCIL2D );
    switch ( effect )
    {
        case EFFECT::ANTIALIAS:
            settings.setValue( SETTING_ANTIALIAS, b );
            break;
        case EFFECT::BLURRYZOOM:
            settings.setValue ( SETTING_BLURRYZOOM, b );
            break;
        case EFFECT::GRID:
            settings.setValue( SETTING_SHOW_GRID, b );
        default:
            break;
    }
}
