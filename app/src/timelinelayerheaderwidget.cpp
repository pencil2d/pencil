#include "timelinelayerheaderwidget.h"

#include <QPalette>
#include <QApplication>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QDebug>
#include <QCheckBox>
#include <QSpacerItem>

#include "editor.h"
#include "timeline.h"
#include "layervisibilitybutton.h"

TimeLineLayerHeaderWidget::TimeLineLayerHeaderWidget(TimeLine* timeLine,
                                                Editor* editor)
    : QWidget(timeLine),
      mEditor(editor)
{
    mHLayout = new QHBoxLayout(this);
    mVisibilityButton = new LayerVisibilityButton(this, LayerVisibilityContext::GLOBAL, nullptr, editor);

    mHLayout->setContentsMargins(0,0,0,0);
    mHLayout->addWidget(mVisibilityButton);
    mHLayout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Expanding));
}

TimeLineLayerHeaderWidget::~TimeLineLayerHeaderWidget()
{
}
