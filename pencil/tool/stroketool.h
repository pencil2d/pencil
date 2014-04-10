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
    virtual void drawStroke();
    virtual void endStroke();

signals:
    
public slots:

protected:
    bool m_firstDraw;

    QPointF lastPixel;
    QList<QPointF> strokePoints;
    QList<qreal> strokePressures;

    qreal currentWidth;
    qreal currentPressure;
};

#endif // STROKETOOL_H
