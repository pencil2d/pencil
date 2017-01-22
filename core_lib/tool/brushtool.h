#ifndef BRUSHTOOL_H
#define BRUSHTOOL_H

#include "stroketool.h"
#include "bitmapimage.h"

class BrushTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit BrushTool( QObject *parent = 0 );
    ToolType type() override;
    void loadSettings() override;
    QCursor cursor() override;

    void mouseMoveEvent( QMouseEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice ) override;

    void drawStroke();
    void paintAt( QPointF point );

    void setWidth( const qreal width ) override;
    void setFeather( const qreal feather ) override;
    void setUseFeather( const bool usingFeather ) override;
    void setPressure( const bool pressure ) override;

protected:
    QPointF mLastBrushPoint;

    QPointF mMouseDownPoint;

    BitmapImage mImg;
    QColor mCurrentPressuredColor;
    qreal mOpacity;
};

#endif // BRUSHTOOL_H
