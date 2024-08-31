#include "timelinelayerheadercell.h"

#include <QPalette>
#include <QMouseEvent>

TimeLineLayerHeaderCell::TimeLineLayerHeaderCell(TimeLine* timeLine,
                                                Editor* editor,
                                                QPoint origin,
                                                int width,
                                                int height) : TimeLineBaseCell(timeLine, editor, origin, width, height)
{
}

TimeLineLayerHeaderCell::~TimeLineLayerHeaderCell()
{
}

void TimeLineLayerHeaderCell::paintGlobalDotVisibility(QPainter& painter, const QPalette& palette) const
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
    painter.drawEllipse(mGlobalBounds.x() + 6, mGlobalBounds.y() + 4, 9, 9);
    painter.setRenderHint(QPainter::Antialiasing, false);
}

void TimeLineLayerHeaderCell::paint(QPainter &painter, const QPalette &palette) const
{
    paintSplitter(painter, palette);
    paintGlobalDotVisibility(painter, palette);
}

void TimeLineLayerHeaderCell::paintSplitter(QPainter& painter, const QPalette& palette) const
{
    painter.setPen(palette.color(QPalette::Mid));
    int yPos = mGlobalBounds.y() + mGlobalBounds.height() - painter.pen().width();
    painter.drawLine(mGlobalBounds.x(), yPos, mGlobalBounds.width(), yPos);
}

void TimeLineLayerHeaderCell::mousePressEvent(QMouseEvent* event)
{
    if (!mGlobalBounds.contains(event->pos())) { return; }

    if (event->pos().x() < 15)
    {
        if (event->button() == Qt::LeftButton) {
            mEditor->increaseLayerVisibilityIndex();
        } else if (event->button() == Qt::RightButton) {
            mEditor->decreaseLayerVisibilityIndex();
        }
    }
}
