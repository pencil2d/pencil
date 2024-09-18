#include "timelinelayercell.h"

#include "preferencemanager.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "pencilsettings.h"

#include "layer.h"
#include "layercamera.h"
#include "camerapropertiesdialog.h"
#include "timelinelayercelleditorwidget.h"

#include <QPalette>
#include <QInputDialog>
#include <QRegularExpression>
#include <QMouseEvent>

#include <QDebug>

TimeLineLayerCell::TimeLineLayerCell(TimeLine* timeline,
                                     QWidget* parent,
                                     Editor* editor,
                                     Layer* layer,
                                     const QPoint& origin, int width, int height)
    : TimeLineBaseCell(timeline, parent, editor, origin, width, height)
{
    // mOldBounds = mGlobalBounds;

    mEditorWidget = new TimeLineLayerCellEditorWidget(parent, editor, layer);
    mEditorWidget->setGeometry(mGlobalBounds);
    mEditorWidget->show();
}

TimeLineLayerCell::~TimeLineLayerCell()
{
    mEditorWidget->deleteLater();
}

