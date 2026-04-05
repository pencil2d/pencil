#include "autosaverbytime.h"

AutosaverByTime::AutosaverByTime(PreferenceManager* manager)
    : pref(manager)
{
    autoSaveTimer.setSingleShot(false);
    resetTimer();
    if(pref->isOn(SETTING::AUTO_SAVE_BY_TIME))
        autoSaveTimer.start();

    connect(pref, &PreferenceManager::optionChanged, this, &AutosaverByTime::configChanged);
    connect(&autoSaveTimer, &QTimer::timeout, this, &AutosaverByTime::timerTimeout);
}

void AutosaverByTime::configChanged(SETTING param){
    switch(param){

        case SETTING::AUTO_SAVE_BY_TIME:
            if(pref->isOn(SETTING::AUTO_SAVE_BY_TIME)) autoSaveTimer.start();
            else autoSaveTimer.stop();
            break;

        case SETTING::AUTO_SAVE_BY_TIME_TIMER:
            autoSaveTimer.stop();
            resetTimer();
            if(pref->isOn(SETTING::AUTO_SAVE_BY_TIME))
                autoSaveTimer.start();

            break;

        default:
            break;
    }
}

void AutosaverByTime::timerTimeout(){
    if(QGuiApplication::mouseButtons() == Qt::NoButton &&
       QGuiApplication::keyboardModifiers() == Qt::NoModifier){
        emit timeout();
        resetTimer();
    }
    else{
        autoSaveTimer.setInterval(1000);
    }
}

void AutosaverByTime::resetTimer(){
    autoSaveTimer.setInterval(pref->getInt(SETTING::AUTO_SAVE_BY_TIME_TIMER) * 1000 * 60); // Interval in minutes
}
