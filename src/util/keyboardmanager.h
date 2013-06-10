#ifndef KEYBOARDMANAGER_H
#define KEYBOARDMANAGER_H

#include <QObject>


class QSettings;


enum ACTION
{
    UNKOWN_ACTION = 0,
    OPEN_FILE_ATION
};


class KeyboardManager : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardManager(QObject *parent = 0);
    void setShortcut(ACTION eAction, QString strKeys);
    QString getShortcut(ACTION eAction);
signals:
    
public slots:
    
private:
    QSettings* m_pKeySettings;
};

#endif // KEYBOARDMANAGER_H
