
#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include <QObject>

class Editor;


class LayerManager : public QObject
{
    Q_OBJECT

public:
    LayerManager( QObject* pParant );
    ~LayerManager();

    bool Initialize( Editor* pEditor );

private:
    Editor* m_pEditor;
};

#endif