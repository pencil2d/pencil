#ifndef KEYBOARDMANAGER_H
#define KEYBOARDMANAGER_H

#include <QObject>


class QSettings;


#define COMMAND_NEW "NewFile"
#define COMMAND_OPEN "OpenFile"
#define COMMAND_SAVE "Save"
#define COMMAND_SAVE_AS "SaveAs"




class KeyboardManager : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardManager(QObject *parent = 0);
    //void setShortcut(ACTION eAction, QString strKeys);
    //QString getShortcut(ACTION eAction);
signals:
    
public slots:
    
private:
    QSettings* m_pKeySettings;
};

#endif // KEYBOARDMANAGER_H
