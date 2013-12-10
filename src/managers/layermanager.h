#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include <QObject>

class Object;
class Layer;

class LayerManager : public QObject
{
    Q_OBJECT

public:
    LayerManager( QObject* pParant );
    ~LayerManager();

    bool setObject( Object* );

    Layer* currentLayer();
    Layer* currentLayer( int incr );
    int currentLayerIndex();
    void setCurrentLayerIndex( int );

    void gotoNextLayer();
    void gotoPreviouslayer();

    int  currentFrameIndex();
    void setCurrentFrameIndex( int );

private:
    Object* m_pObject;

    int m_currentLayerIndex; // the current layer to be edited/displayed
    int m_currentFrameIndex; // the current key frame to be edited/displayed
};

#endif