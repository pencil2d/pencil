#ifndef PENCILSETTINGS_H
#define PENCILSETTINGS_H

#include <QObject>

class QSettings;

class PencilSettings : public QObject
{
    Q_OBJECT
public:
    explicit PencilSettings(QObject *parent = 0);
    static PencilSettings* get();
signals:
    
public slots:
    
private:
    static QSettings* m_pSettings;
};

#endif // PENCILSETTINGS_H
