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
    
    set( EFFECT::ANTIALIAS, settings.value( SETTING_ANTIALIAS, false ).toBool() );

    return true;
}

void PreferenceManager::set( EFFECT effect, bool bOnOff )
{
    int index = static_cast< size_t >( effect );

    if ( bOnOff != mEffectSet[ index ] )
    {
        mEffectSet[ index ] = bOnOff;
        emit preferenceChanged( effect );
    }
}

bool PreferenceManager::get( EFFECT effect )
{
    int index = static_cast< size_t >( effect );
    return mEffectSet[ index ];
}
