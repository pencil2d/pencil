#include "timelinelayerheaderwidget.h"

#include <QPalette>
#include <QApplication>
#include <QMouseEvent>

#include "editor.h"

TimeLineLayerHeaderWidget::TimeLineLayerHeaderWidget(TimeLine* timeLine,
                                                Editor* editor)
    : QWidget(timeLine),
      mEditor(editor)
{
}

TimeLineLayerHeaderWidget::~TimeLineLayerHeaderWidget()
{
}

void TimeLineLayerHeaderWidget::paintGlobalDotVisibility(QPainter& painter, const QPalette& palette) const
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
    painter.drawEllipse(mLeftPadding, rect().center().y() - (mVisibilityCircleSize.height() * 0.5), mVisibilityCircleSize.width(), mVisibilityCircleSize.height());
    painter.setRenderHint(QPainter::Antialiasing, false);
}

void TimeLineLayerHeaderWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    const QPalette palette = QApplication::palette();
    paintGlobalDotVisibility(painter, palette);
}

void TimeLineLayerHeaderWidget::mousePressEvent(QMouseEvent* event)
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
