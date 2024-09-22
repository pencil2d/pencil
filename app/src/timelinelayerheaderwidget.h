#ifndef TIMELINELAYERHEADERWIDGET_H
#define TIMELINELAYERHEADERWIDGET_H

#include "timelinebasecell.h"

#include <QWidget>
#include <QPainter>


class TimeLine;
class Editor;
class LayerVisibilityButton;
class QHBoxLayout;

class TimeLineLayerHeaderWidget : public QWidget
{
public:
    TimeLineLayerHeaderWidget(TimeLine* timeLine,
                            Editor* editor);
                            
    ~TimeLineLayerHeaderWidget() override;

private:

    Editor* mEditor = nullptr;
    LayerVisibilityButton* mVisibilityButton = nullptr;
    QHBoxLayout* mHLayout = nullptr;
};

#endif // TIMELINELAYERHEADERWIDGET_H
