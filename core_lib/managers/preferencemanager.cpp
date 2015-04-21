#include "preferencemanager.h"

PreferenceManager::PreferenceManager( QObject* parent )
    : BaseManager( parent )
{
}

PreferenceManager::~PreferenceManager()
{
}

bool PreferenceManager::init()
{
    return true;
}

void PreferenceManager::set( EFFECT effect, bool bOnOff )
{
    int index = static_cast< size_t >( effect );

    if ( bOnOff != mPreferenceSet[ index ] )
    {
        mPreferenceSet[ index ] = bOnOff;
        emit preferenceChanged( effect );
    }
}

bool PreferenceManager::get( EFFECT effect )
{
    int index = static_cast< size_t >( effect );
    return mPreferenceSet[ index ];
}
