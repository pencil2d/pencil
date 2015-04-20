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

void PreferenceManager::set( PREFERENCEITEM item, bool bOnOff )
{
    int index = static_cast< size_t >( item );

    if ( bOnOff != mPreferenceSet[ index ] )
    {
        mPreferenceSet[ index ] = bOnOff;
        emit preferenceChanged( item );
    }
}

bool PreferenceManager::get( PREFERENCEITEM item )
{
    int index = static_cast< size_t >( item );
    return mPreferenceSet[ index ];
}
