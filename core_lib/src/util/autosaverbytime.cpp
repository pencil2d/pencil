#include "autosaverbytime.h"

AutosaverByTime::AutosaverByTime(PreferenceManager* manager)
    : mPref(manager)
{
    mAutoSaveTimer.setSingleShot(false);
    resetTimer();
    if(mPref->isOn(SETTING::AUTO_SAVE_BY_TIME))
        mAutoSaveTimer.start();

    connect(mPref, &PreferenceManager::optionChanged, this, &AutosaverByTime::configChanged);
    connect(&mAutoSaveTimer, &QTimer::timeout, this, &AutosaverByTime::timerTimeout);
}

void AutosaverByTime::configChanged(SETTING setting){
    switch(setting)
    {
        case SETTING::AUTO_SAVE_BY_TIME:
            if(mPref->isOn(SETTING::AUTO_SAVE_BY_TIME)) mAutoSaveTimer.start();
            else mAutoSaveTimer.stop();
            break;

        case SETTING::AUTO_SAVE_BY_TIME_TIMER:
            mAutoSaveTimer.stop();
            resetTimer();
            if(mPref->isOn(SETTING::AUTO_SAVE_BY_TIME))
                mAutoSaveTimer.start();

            break;

        default:
            break;
    }
}

void AutosaverByTime::timerTimeout()
{
    if(QGuiApplication::mouseButtons() == Qt::NoButton &&
       QGuiApplication::keyboardModifiers() == Qt::NoModifier)
    {
        emit timeout();
        resetTimer();
    }
    else
    {
        mAutoSaveTimer.setInterval(1000);
    }
}

void AutosaverByTime::resetTimer()
{
    mAutoSaveTimer.setInterval(mPref->getInt(SETTING::AUTO_SAVE_BY_TIME_TIMER) * 1000 * 60); // Interval in minutes
}
