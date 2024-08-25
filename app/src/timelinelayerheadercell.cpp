#include "timelinelayerheadercell.h"

#include <QPalette>

TimeLineLayerHeaderCell::TimeLineLayerHeaderCell(TimeLine* timeLine, Editor* editor, QPoint origin, QSize size)
{
    mEditor = editor;
    mTimeLine = timeLine;

    mOrigin = origin;
    mSize = size;
}

TimeLineLayerHeaderCell::TimeLineLayerHeaderCell()
{

}

void TimeLineLayerHeaderCell::paintGlobalDotVisibility(QPainter& painter, const QPalette& palette)
{
    // --- draw circle
    painter.setPen(palette.color(QPalette::Text));
    if (mEditor->layerVisibility() == LayerVisibility::CURRENTONLY)
    {
        painter.setBrush(palette.color(QPalette::Base));
    }
    else if (mEditor->layerVisibility() == LayerVisibility::RELATED)
    {
        QColor color = palette.color(QPalette::Text);
        color.setAlpha(128);
        painter.setBrush(color);
    }
    else if (mEditor->layerVisibility() == LayerVisibility::ALL)
    {
        painter.setBrush(palette.brush(QPalette::Text));
    }
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(mOrigin.x() + 6, mOrigin.y() + 4, 9, 9);
    painter.setRenderHint(QPainter::Antialiasing, false);
}
