#ifndef PENCILTOOL_H
#define PENCILTOOL_H

#include "stroketool.h"

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

    void adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice );

    void setWidth( const qreal width );
    void setFeather( const qreal feather );
    void setInvisibility( const qreal invisibility );
    void setPressure( const bool pressure );
    void setPreserveAlpha( const bool preserveAlpha );

private:
    QColor currentPressuredColor;
};

#endif // PENCILTOOL_H
