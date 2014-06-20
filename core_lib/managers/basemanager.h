#ifndef BASEMANAGER_H
#define BASEMANAGER_H

#include <QObject>

class Editor;


class BaseManager : public QObject
{
    Q_OBJECT
public:
    explicit BaseManager(QObject *parent = 0);

    Editor* editor();
    void    setEditor( Editor* pEditor );
    
    virtual bool init() = 0;

private:
    Editor* mEditor;
};

#endif // BASEMANAGER_H
