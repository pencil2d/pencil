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

    bool init( Editor* );
    Layer* currentLayer();
    Layer* currentLayer( int incr );

    int  currentFrameIndex();
    void setCurrentFrameIndex( int );

private:
    Editor* m_pEditor;

    int m_currentFrameIndex;
};

#endif