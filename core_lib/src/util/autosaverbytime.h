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
    AutosaverByTime(PreferenceManager*);

private:
    PreferenceManager* pref;
    QTimer autoSaveTimer;

    void resetTimer();

public slots:
    void configChanged(SETTING);
    void timerTimeout();

signals:
    void timeout();
};

#endif // AUTOSAVERBYTIME_H
