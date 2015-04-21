#ifndef PREFERENCEMANAGER_H
#define PREFERENCEMANAGER_H

#include <bitset>
#include "basemanager.h"


enum class EFFECT
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

    void set( EFFECT item, bool bOnOff );
    bool get( EFFECT item );
    
    void setOnionPrevFramesCount( int n );
    int  getOnionPrevFramesCount();
    void setOnionNextFramesCount( int n );
    int  getOnionNextFramesCount();

Q_SIGNALS:
    void preferenceChanged( EFFECT item );

private:
    std::bitset< static_cast< size_t >( EFFECT::COUNT ) > mPreferenceSet;
};

#endif // PREFERENCEMANAGER_H
