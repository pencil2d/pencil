#ifndef AUTOSAVERBYTIME_H
#define AUTOSAVERBYTIME_H

#include <QObject>
#include <QTimer>
#include <QGuiApplication>

#include "preferencemanager.h"

class autosaverbytime : public QObject
{
    Q_OBJECT

protected:
    PreferenceManager* pref;
    void resetTimer();

public:
    QTimer* autoSaveTimer;

    autosaverbytime(PreferenceManager*);

public slots:
    void config_changed(SETTING);
    void timer_timeout();

signals:
    void timeout();
};

#endif // AUTOSAVERBYTIME_H
