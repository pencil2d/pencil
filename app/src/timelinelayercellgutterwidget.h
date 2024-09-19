#ifndef TIMELINELAYERCELLGUTTERWIDGET_H
#define TIMELINELAYERCELLGUTTERWIDGET_H

#include <QWidget>

class TimeLineLayerCellGutterWidget : public QWidget
{
public:
    TimeLineLayerCellGutterWidget(int width, QWidget* parent = nullptr);

    void paintEvent(QPaintEvent* event) override;
    void updateWidth(int width);
};

#endif // TIMELINELAYERCELLGUTTERWIDGET_H
