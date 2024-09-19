#include "timelinelayercellgutterwidget.h"

#include <QPainter>
#include <QApplication>

TimeLineLayerCellGutterWidget::TimeLineLayerCellGutterWidget(int width, QWidget* parent)
    : QWidget(parent)
{

    setGeometry(0, 0, width, 2);
}

void TimeLineLayerCellGutterWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPalette palette = QApplication::palette();

    painter.setPen(palette.color(QPalette::HighlightedText));
    painter.drawRect(0, rect().bottom(), rect().width(), 2);
}

void TimeLineLayerCellGutterWidget::updateWidth(int width)
{
    resize(width, height());
}
