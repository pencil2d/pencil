#ifndef PENCILTOOL_H
#define PENCILTOOL_H

#include "stroketool.h"
#include <QColor>

class PencilTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit PencilTool( QObject *parent = 0 );
    ToolType type() override { return PENCIL; }
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void drawStroke();
    void paintAt( QPointF point );
    void paintVectorStroke();

    void adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice ) override;

    void setWidth( const qreal width ) override;
    void setFeather( const qreal feather ) override;
    void setInvisibility( const bool invisibility ) override;
    void setPressure( const bool pressure ) override;
    void setPreserveAlpha( const bool preserveAlpha ) override;
    void setInpolLevel(const int level) override;
    void setUseFillContour(const bool useFillContour) override;

private:
    QColor mCurrentPressuredColor { 0, 0, 0, 255 };
    QPointF mLastBrushPoint { 0, 0 };
    qreal mOpacity = 1.0f;
    QPointF mMouseDownPoint;
};

#endif // PENCILTOOL_H
