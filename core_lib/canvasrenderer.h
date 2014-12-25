#ifndef CANVASRENDERER_H
#define CANVASRENDERER_H

#include <QWidget>

class CanvasRenderer : public QWidget
{
    Q_OBJECT
public:
    explicit CanvasRenderer(QWidget *parent = 0);
    virtual ~CanvasRenderer();


};

#endif // CANVASRENDERER_H
