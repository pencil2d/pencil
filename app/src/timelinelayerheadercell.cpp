#include "timelinelayerheadercell.h"

#include <QPalette>
#include <QMouseEvent>

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

void TimeLineLayerHeaderCell::paintSplitter(QPainter& painter, const QPalette& palette)
{
    painter.setPen(palette.color(QPalette::Mid));
    int yPos = mOrigin.y() + mSize.height() - painter.pen().width();
    painter.drawLine(mOrigin.x(), yPos, mSize.width(), yPos);
}

void TimeLineLayerHeaderCell::mousePressEvent(QMouseEvent* event, int layerNumber)
{
    if (event->pos().x() < 15)
    {
        if (event->button() == Qt::LeftButton) {
            mEditor->increaseLayerVisibilityIndex();
        } else if (event->button() == Qt::RightButton) {
            mEditor->decreaseLayerVisibilityIndex();
        }
    }
}
