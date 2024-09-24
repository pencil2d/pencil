#include "timelinelayercellgutterwidget.h"

#include <QPainter>
#include <QApplication>

TimeLineLayerCellGutterWidget::TimeLineLayerCellGutterWidget(int width, QWidget* parent)
    : QWidget(parent)
{

    setGeometry(0, 0, width, 8);
}

void TimeLineLayerCellGutterWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPalette palette = QApplication::palette();

    QPen pen = palette.color(QPalette::HighlightedText);
    pen.setWidth(1);
    painter.setPen(pen);
    QColor brushColor = palette.color(QPalette::Highlight);
    brushColor.setAlphaF(0.5);
    painter.setBrush(brushColor);
    painter.drawRect(rect().x()+1, rect().top()+1, rect().width()-1, rect().bottom()-1);
}

void TimeLineLayerCellGutterWidget::updateWidth(int width)
{
    resize(width, height());
}
