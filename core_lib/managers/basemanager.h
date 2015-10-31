#ifndef BASEMANAGER_H
#define BASEMANAGER_H

#include "editor.h"

class Object;


class BaseManager : public QObject
{
    Q_OBJECT
public:
    explicit BaseManager(QObject *parent = 0);
    virtual ~BaseManager();

    void     setEditor( Editor* pEditor );

    Editor* editor() { return mEditor; }
    Object* object() { return mEditor->object(); }

    virtual bool init() = 0;

private:
    Editor* mEditor = nullptr;
};

#endif // BASEMANAGER_H
