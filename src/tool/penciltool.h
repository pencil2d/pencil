#ifndef PENCILTOOL_H
#define PENCILTOOL_H

#include <QList>
#include <QPointF>

#include "basetool.h"

class PencilTool : public BaseTool
{
    Q_OBJECT
public:
    explicit PencilTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);

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
    
};

#endif // PENCILTOOL_H
