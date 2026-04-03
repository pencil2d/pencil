#include "autosaverbytime.h"

autosaverbytime::autosaverbytime(PreferenceManager* manager)
    : pref(manager)
{
    autoSaveTimer = new QTimer();
    autoSaveTimer->setSingleShot(false);
    resetTimer();
    if(pref->isOn(SETTING::AUTO_SAVE_BY_TIME))
        autoSaveTimer->start();

    connect(pref, &PreferenceManager::optionChanged, this, &autosaverbytime::config_changed);
    connect(autoSaveTimer, &QTimer::timeout, this, &autosaverbytime::timer_timeout);
}

void autosaverbytime::config_changed(SETTING param){
    switch(param){

        case SETTING::AUTO_SAVE_BY_TIME:
            if(pref->isOn(SETTING::AUTO_SAVE_BY_TIME)) autoSaveTimer->start();
            else autoSaveTimer->stop();
            break;

        case SETTING::AUTO_SAVE_BY_TIME_TIMER:
            autoSaveTimer->stop();
            resetTimer();
            if(pref->isOn(SETTING::AUTO_SAVE_BY_TIME))
                autoSaveTimer->start();

            break;

        default:
            break;
    }
}

void autosaverbytime::timer_timeout(){
    if(QGuiApplication::mouseButtons() == Qt::NoButton &&
       QGuiApplication::keyboardModifiers() == Qt::NoModifier){
        emit timeout();
        resetTimer();
    }
    else{
        autoSaveTimer->setInterval(1000);
    }
}

void autosaverbytime::resetTimer(){
    autoSaveTimer->setInterval(pref->getInt(SETTING::AUTO_SAVE_BY_TIME_TIMER) * 1000 * 60); // Interval in minutes
}
