#ifndef STROKETOOL_H
#define STROKETOOL_H

#include "basetool.h"

#include <QList>
#include <QPointF>
#include <QPair>

typedef QPair<QPointF, QPointF> QSegment;

class StrokeTool : public BaseTool
{
    Q_OBJECT
public:
    explicit StrokeTool(QObject *parent = 0);
    
    virtual void startStroke();
    virtual void drawStroke() = 0;
    virtual void endStroke();
    QList<QSegment> calculateStroke(float width);

signals:
    
public slots:

protected:
    bool m_firstDraw;

    QPointF lastPixel;
    QList<QPointF> strokePoints;
    QList<qreal> strokePressures;

    qreal currentWidth;
};

#endif // STROKETOOL_H
