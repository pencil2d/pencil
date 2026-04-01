#include "autosaverbytime.h"

autosaverbytime::autosaverbytime(PreferenceManager* manager)
    : pref(manager)
{
    autoSaveTimer = new QTimer();
    autoSaveTimer->setTimerType(Qt::CoarseTimer);
    // Interval in minutes
    autoSaveTimer->setInterval(pref->getInt(SETTING::AUTO_SAVE_BY_TIME_TIMER) * 1000 * 60);
    if(pref->isOn(SETTING::AUTO_SAVE_BY_TIME))
        autoSaveTimer->start();

    connect(pref, &PreferenceManager::optionChanged, this, &autosaverbytime::config_changed);
}

void autosaverbytime::config_changed(SETTING param){
    switch(param){

        case SETTING::AUTO_SAVE_BY_TIME:
            if(pref->isOn(SETTING::AUTO_SAVE_BY_TIME)) autoSaveTimer->start();
            else autoSaveTimer->stop();
            break;

        case SETTING::AUTO_SAVE_BY_TIME_TIMER:
            autoSaveTimer->stop();
            autoSaveTimer->setInterval(pref->getInt(SETTING::AUTO_SAVE_BY_TIME_TIMER) * 1000 * 60);
            if(pref->isOn(SETTING::AUTO_SAVE_BY_TIME))
                autoSaveTimer->start();

            break;

        default:
            break;
    }
}
