#ifndef TIMELINELAYERHEADERWIDGET_H
#define TIMELINELAYERHEADERWIDGET_H

#include "timelinebasecell.h"

#include <QWidget>
#include <QPainter>


class TimeLine;
class Editor;

class TimeLineLayerHeaderWidget : public QWidget
{
public:
    TimeLineLayerHeaderWidget(TimeLine* timeLine,
                            Editor* editor);
                            
    ~TimeLineLayerHeaderWidget() override;

    void paintGlobalDotVisibility(QPainter& painter, const QPalette& palette) const;
    void paintSplitter(QPainter& painter, const QPalette& palette) const;

    void mousePressEvent(QMouseEvent* event) override;
    // void mouseMoveEvent(QMouseEvent* event) override;
    // void mouseReleaseEvent(QMouseEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

private:

    Editor* mEditor = nullptr;
    QSize mVisibilityCircleSize = QSize(9,9);
    int mLeftPadding = 7;
};

#endif // TIMELINELAYERHEADERWIDGET_H
