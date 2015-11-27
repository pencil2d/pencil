#ifndef PREFERENCEMANAGER_H
#define PREFERENCEMANAGER_H

#include <bitset>
#include "basemanager.h"
#include "pencildef.h"


enum class EFFECT
{
    ANTIALIAS,
    BLURRYZOOM,
    GRID,
    COUNT,
    SHADOW,
    PREV_ONION,
    NEXT_ONION,
    AXIS,
    CAMERABORDER,
    INVISIBLE_LINES,
    OUTLINES,
    ONION_BLUE,
    ONION_RED,
    MIRROR_H,
    MIRROR_V
};

class PreferenceManager : public BaseManager
{
    Q_OBJECT

public:
    PreferenceManager( QObject* parent );
    ~PreferenceManager();

    virtual bool init() override;
    Status onObjectLoaded( Object* ) override;

    void loadPrefs();
    void set(EFFECT e, bool value );

    void turnOn( EFFECT e );
    void turnOff( EFFECT e );
    bool isOn( EFFECT e );
    
    void setOnionPrevFramesCount( int n );
    int  getOnionPrevFramesCount();
    void setOnionNextFramesCount( int n );
    int  getOnionNextFramesCount();

Q_SIGNALS:
    void prefsLoaded();
    void effectChanged( EFFECT e, bool value );

private:
    QHash< int, bool > mEffectSet;
};

#endif // PREFERENCEMANAGER_H
