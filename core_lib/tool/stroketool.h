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
    bool mFirstDraw;

    QPointF lastPixel;
    QList<QPointF> mStrokePoints;
    QList<qreal> mStrokePressures;

    qreal mCurrentWidth;
    qreal mCurrentPressure;
};

#endif // STROKETOOL_H
