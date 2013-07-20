#ifndef PENTOOL_H
#define PENTOOL_H

#include <QList>
#include <QPointF>

#include "basetool.h"
#include "penciltool.h"

class PenTool : public PencilTool
{
    Q_OBJECT
public:
    PenTool();
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);

};

#endif // PENTOOL_H
