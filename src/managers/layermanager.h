#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include <QObject>

class Editor;
class Layer;

class LayerManager : public QObject
{
    Q_OBJECT

public:
    LayerManager( QObject* pParant );
    ~LayerManager();

    bool init( Editor* pEditor );
    Layer* currentLayer();
    Layer* currentLayer( int incr );

private:
    Editor* m_pEditor;
};

#endif