#ifndef AUTOSAVERBYTIME_H
#define AUTOSAVERBYTIME_H

#include <QObject>
#include <QTimer>

#include "preferencemanager.h"

class autosaverbytime : public QObject
{
    Q_OBJECT

protected:
    PreferenceManager* pref;

public:
    QTimer* autoSaveTimer;

    autosaverbytime(PreferenceManager*);

public slots:
    void config_changed(SETTING);
};

#endif // AUTOSAVERBYTIME_H
