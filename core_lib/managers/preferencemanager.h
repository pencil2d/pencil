#ifndef PREFERENCEMANAGER_H
#define PREFERENCEMANAGER_H

#include <bitset>
#include "basemanager.h"


enum class PREFERENCEITEM
{
    ANTIALIAS,
    COUNT
};

class PreferenceManager : public BaseManager
{
    Q_OBJECT

public:
    PreferenceManager( QObject* parent );
    ~PreferenceManager();

    virtual bool init() override;

    void set( PREFERENCEITEM item, bool bOnOff );
    bool get( PREFERENCEITEM item );

Q_SIGNALS:
    void preferenceChanged( PREFERENCEITEM item );

private:
    std::bitset< static_cast< size_t >( PREFERENCEITEM::COUNT ) > mPreferenceSet;
};

#endif // PREFERENCEMANAGER_H
