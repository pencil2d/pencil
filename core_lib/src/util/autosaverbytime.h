#ifndef AUTOSAVERBYTIME_H
#define AUTOSAVERBYTIME_H

#include <QObject>
#include <QTimer>
#include <QGuiApplication>

#include "preferencemanager.h"

class AutosaverByTime : public QObject
{
    Q_OBJECT

public:
    explicit AutosaverByTime(PreferenceManager*);

public slots:
    void configChanged(SETTING);
    void timerTimeout();

signals:
    void timeout();

private:
    void resetTimer();
    
    PreferenceManager* mPref = nullptr;
    QTimer mAutoSaveTimer;

};

#endif // AUTOSAVERBYTIME_H
