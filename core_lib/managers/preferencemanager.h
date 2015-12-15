#ifndef PREFERENCEMANAGER_H
#define PREFERENCEMANAGER_H

#include <bitset>
#include "basemanager.h"
#include "pencildef.h"


enum class SETTING
{
    ANTIALIAS,
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
    MIRROR_V,
    TOOL_CURSOR,
    HIGH_RESOLUTION,
    WINDOW_OPACITY,
    CURVE_SMOOTHING,
    BACKGROUND_STYLE,
    AUTO_SAVE,
    AUTO_SAVE_NUMBER,
    SHORT_SCRUB,
    FRAME_SIZE,
    TIMELINE_SIZE,
    LABEL_FONT_SIZE,
    DRAW_LABEL,
    ONION_MAX_OPACITY,
    ONION_MIN_OPACITY,
    ONION_PREV_FRAMES_NUM,
    ONION_NEXT_FRAMES_NUM
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
    void set(SETTING option, QString value );
    void set(SETTING option, int value );
    void set(SETTING option, bool value );

    void turnOn(SETTING option );
    void turnOff(SETTING option );
    bool isOn(SETTING option );
    
    void setOnionPrevFramesCount( int n );
    int  getOnionPrevFramesCount();
    void setOnionNextFramesCount( int n );
    int  getOnionNextFramesCount();

    QString getString(SETTING option);
    int     getInt(SETTING option);

Q_SIGNALS:
    void optionChanged( SETTING e );


private:
    QHash< int, QString > mStringSet;
    QHash< int, int > mIntegerSet;
    QHash< int, bool > mBooleanSet;
};

#endif // PREFERENCEMANAGER_H
