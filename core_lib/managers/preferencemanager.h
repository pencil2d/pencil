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
    
    void turnOn( EFFECT e );
    void turnOff( EFFECT e );
    bool isOn( EFFECT e );
    
    void setOnionPrevFramesCount( int n );
    int  getOnionPrevFramesCount();
    void setOnionNextFramesCount( int n );
    int  getOnionNextFramesCount();

Q_SIGNALS:
    void preferenceChanged( EFFECT e );

private:
    void set( EFFECT e, bool b );
    std::bitset< static_cast< size_t >( EFFECT::COUNT ) > mEffectSet;
};

#endif // PREFERENCEMANAGER_H
