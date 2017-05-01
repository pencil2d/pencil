#ifndef MOVETOOL_H
#define MOVETOOL_H

#include "basetool.h"

class Layer;

class MoveTool : public BaseTool
{
    Q_OBJECT
public:
    explicit MoveTool( QObject* parent = 0 );
    ToolType type() override;
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;

    bool keyPressEvent(QKeyEvent *event) override;

    void leavingThisTool() override;
    void switchingLayers() override;

private:
    void cancelChanges();
    void applyChanges();
    void whichTransformationPoint();
    void transformSelection(qreal offsetX, qreal offsetY);
    void pressOperation(QMouseEvent* event, Layer *layer);
    void actionOnVector(QMouseEvent *event, Layer *layer);
    void onHoverOutlineStroke(Layer* layer);
    QPointF maintainAspectRatio(qreal offsetX, qreal offsetY);
};

#endif
