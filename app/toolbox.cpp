/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2017 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <cmath>

#include <QToolButton>
#include <QGridLayout>
#include <QKeySequence>

#include "spinslider.h"
#include "toolbox.h"
#include "editor.h"
#include "toolmanager.h"
#include "pencilsettings.h"

// ----------------------------------------------------------------------------------
QString GetToolTips(QString strCommandName)
{
    strCommandName = QString("shortcuts/") + strCommandName;
    QKeySequence keySequence(pencilSettings().value(strCommandName).toString());
    return QString("<b>%1</b>").arg(keySequence.toString()); // don't tr() this string.
}

ToolBoxWidget::ToolBoxWidget(QWidget* parent) : BaseDockWidget(parent)
{
    setWindowTitle(tr("Tools", "Window title of tool box"));
    setWindowIcon(QIcon());
}

void ToolBoxWidget::initUI()
{
    mGridLayout = new QGridLayout(this);

    mPencilButton = newToolButton(QIcon(":icons/new/svg/pencil_detailed.svg"),
                                  tr("Pencil Tool (%1): Sketch with pencil")
                                  .arg(GetToolTips(CMD_TOOL_PENCIL)));
    mSelectButton = newToolButton(QIcon(":icons/new/svg/selection.svg"),
                                  tr("Select Tool (%1): Select an object")
                                  .arg(GetToolTips(CMD_TOOL_SELECT)));
    mMoveButton = newToolButton(QIcon(":icons/new/svg/arrow.svg"),
                                tr("Move Tool (%1): Move an object")
                                .arg(GetToolTips(CMD_TOOL_MOVE)));
    mHandButton = newToolButton(QIcon(":icons/new/svg/hand_detailed.svg"),
                                tr("Hand Tool (%1): Move the canvas")
                                .arg(GetToolTips(CMD_TOOL_HAND)));
    mPenButton = newToolButton(QIcon(":icons/new/svg/pen_detailed.svg"),
                               tr("Pen Tool (%1): Sketch with pen")
                               .arg(GetToolTips(CMD_TOOL_PEN)));
    mEraserButton = newToolButton(QIcon(":icons/new/svg/eraser_detailed.svg"),
                                  tr("Eraser Tool (%1): Erase")
                                  .arg(GetToolTips(CMD_TOOL_ERASER)));
    mPolylineButton = newToolButton(QIcon(":icons/new/svg/line.svg"),
                                    tr("Polyline Tool (%1): Create line/curves")
                                    .arg(GetToolTips(CMD_TOOL_POLYLINE)));
    mBucketButton = newToolButton(QIcon(":icons/new/svg/bucket_detailed.svg"),
                                  tr("Paint Bucket Tool (%1): Fill selected area with a color")
                                  .arg(GetToolTips(CMD_TOOL_BUCKET)));
    mBrushButton = newToolButton(QIcon(":icons/new/svg/brush_detailed.svg"),
                                 tr("Brush Tool (%1): Paint smooth stroke with a brush")
                                 .arg(GetToolTips(CMD_TOOL_BRUSH)));
    mEyedropperButton = newToolButton(QIcon(":icons/new/svg/eyedropper_detailed.svg"),
                                      tr("Eyedropper Tool (%1): "
                                         "Set color from the stage<br>[ALT] for instant access")
                                      .arg(GetToolTips(CMD_TOOL_EYEDROPPER)));
    mClearButton = newToolButton(QIcon(":icons/new/svg/trash_detailed.svg"),
                                 tr("Clear Frame (%1): Erases content of selected frame")
                                 .arg(GetToolTips(CMD_CLEAR_FRAME)));
    mSmudgeButton = newToolButton(QIcon(":icons/new/svg/smudge_detailed.svg"),
                                  tr("Smudge Tool (%1):<br>Edit polyline/curves<br>"
                                     "Liquify bitmap pixels<br> (%1)+[Alt]: Smooth")
                                  .arg(GetToolTips(CMD_TOOL_SMUDGE)));

    mPencilButton->setWhatsThis(tr("Pencil Tool (%1)")
                                .arg(GetToolTips(CMD_TOOL_PENCIL)));
    mSelectButton->setWhatsThis(tr("Select Tool (%1)")
                                .arg(GetToolTips(CMD_TOOL_SELECT)));
    mMoveButton->setWhatsThis(tr("Move Tool (%1)")
                              .arg(GetToolTips(CMD_TOOL_MOVE)));
    mHandButton->setWhatsThis(tr("Hand Tool (%1)")
                              .arg(GetToolTips(CMD_TOOL_HAND)));
    mPenButton->setWhatsThis(tr("Pen Tool (%1)")
                             .arg(GetToolTips(CMD_TOOL_PEN)));
    mEraserButton->setWhatsThis(tr("Eraser Tool (%1)")
                                .arg(GetToolTips(CMD_TOOL_ERASER)));
    mPolylineButton->setWhatsThis(tr("Polyline Tool (%1)")
                                  .arg(GetToolTips(CMD_TOOL_POLYLINE)));
    mBucketButton->setWhatsThis(tr("Paint Bucket Tool (%1)")
                                .arg(GetToolTips(CMD_TOOL_BUCKET)));
    mBrushButton->setWhatsThis(tr("Brush Tool (%1)")
                               .arg(GetToolTips(CMD_TOOL_BRUSH)));
    mEyedropperButton->setWhatsThis(tr("Eyedropper Tool (%1)")
                                    .arg(GetToolTips(CMD_TOOL_EYEDROPPER)));
    mClearButton->setWhatsThis(tr("Clear Tool (%1)")
                               .arg(GetToolTips(CMD_CLEAR_FRAME)));
    mSmudgeButton->setWhatsThis(tr("Smudge Tool (%1)")
                                .arg(GetToolTips(CMD_TOOL_SMUDGE)));

    mPencilButton->setCheckable(true);
    mPenButton->setCheckable(true);
    mPolylineButton->setCheckable(true);
    mBucketButton->setCheckable(true);
    mBrushButton->setCheckable(true);
    mSmudgeButton->setCheckable(true);
    mEyedropperButton->setCheckable(true);
    mSelectButton->setCheckable(true);
    mMoveButton->setCheckable(true);
    mHandButton->setCheckable(true);
    mEraserButton->setCheckable(true);
    mPencilButton->setChecked(true);

    mGridLayout->setMargin(0);
    mGridLayout->setSpacing(0);

    QWidget* toolGroup = new QWidget(this);

    setWidget(toolGroup);
    toolGroup->setLayout(mGridLayout);

    connect(mPencilButton, &QToolButton::clicked, this, &ToolBoxWidget::pencilOn);
    connect(mEraserButton, &QToolButton::clicked, this, &ToolBoxWidget::eraserOn);
    connect(mSelectButton, &QToolButton::clicked, this, &ToolBoxWidget::selectOn);
    connect(mMoveButton, &QToolButton::clicked, this, &ToolBoxWidget::moveOn);
    connect(mPenButton, &QToolButton::clicked, this, &ToolBoxWidget::penOn);
    connect(mHandButton, &QToolButton::clicked, this, &ToolBoxWidget::handOn);
    connect(mPolylineButton, &QToolButton::clicked, this, &ToolBoxWidget::polylineOn);
    connect(mBucketButton, &QToolButton::clicked, this, &ToolBoxWidget::bucketOn);
    connect(mEyedropperButton, &QToolButton::clicked, this, &ToolBoxWidget::eyedropperOn);
    connect(mBrushButton, &QToolButton::clicked, this, &ToolBoxWidget::brushOn);
    connect(mSmudgeButton, &QToolButton::clicked, this, &ToolBoxWidget::smudgeOn);
    connect(mClearButton, &QToolButton::clicked, this, &ToolBoxWidget::clearButtonClicked);

    QSettings settings(PENCIL2D, PENCIL2D);
    this->restoreGeometry(settings.value("ToolBoxGeom").toByteArray());
}

void ToolBoxWidget::updateUI()
{
}

void ToolBoxWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    QRect geom = geometry();
    QSize buttonSize = mClearButton->size(); // all buttons share same size

    // Vertical layout
    if (geom.width() < geom.height())
    {
        if (geom.width() > buttonSize.width() * 5)
        {
            mGridLayout->addWidget(mClearButton, 0, 0);
            mGridLayout->addWidget(mMoveButton, 0, 1);

            mGridLayout->addWidget(mSelectButton, 0, 2);
            mGridLayout->addWidget(mBrushButton, 1, 0);

            mGridLayout->addWidget(mPolylineButton, 1, 1);
            mGridLayout->addWidget(mSmudgeButton, 1, 2);

            mGridLayout->addWidget(mPenButton, 2, 0);
            mGridLayout->addWidget(mHandButton, 2, 1);

            mGridLayout->addWidget(mPencilButton, 2, 2);
            mGridLayout->addWidget(mBucketButton, 3, 0);

            mGridLayout->addWidget(mEyedropperButton, 3, 1);
            mGridLayout->addWidget(mEraserButton, 3, 2);
        }
        else if (geom.width() > buttonSize.width() * 3)
        {
            mGridLayout->addWidget(mClearButton, 0, 0);
            mGridLayout->addWidget(mMoveButton, 0, 1);

            mGridLayout->addWidget(mSelectButton, 1, 0);
            mGridLayout->addWidget(mBrushButton, 1, 1);

            mGridLayout->addWidget(mPolylineButton, 2, 0);
            mGridLayout->addWidget(mSmudgeButton, 2, 1);

            mGridLayout->addWidget(mPenButton, 3, 0);
            mGridLayout->addWidget(mHandButton, 3, 1);

            mGridLayout->addWidget(mPencilButton, 4, 0);
            mGridLayout->addWidget(mBucketButton, 4, 1);

            mGridLayout->addWidget(mEyedropperButton, 5, 0);
            mGridLayout->addWidget(mEraserButton, 5, 1);
        }
        else
        {
            mGridLayout->addWidget(mClearButton, 0, 0);
            mGridLayout->addWidget(mMoveButton, 1, 0);
            mGridLayout->addWidget(mSelectButton, 2, 0);
            mGridLayout->addWidget(mBrushButton, 3, 0);
            mGridLayout->addWidget(mPolylineButton, 4, 0);
            mGridLayout->addWidget(mSmudgeButton, 5, 0);
            mGridLayout->addWidget(mPenButton, 6, 0);
            mGridLayout->addWidget(mHandButton, 7, 0);
            mGridLayout->addWidget(mPencilButton, 8, 0);
            mGridLayout->addWidget(mBucketButton, 9, 0);
            mGridLayout->addWidget(mEyedropperButton, 10, 0);
            mGridLayout->addWidget(mEraserButton, 11, 0);
        }
    }
    else
    { // Horizontal
        if (geom.height() > buttonSize.height() * 5)
        {
            mGridLayout->addWidget(mClearButton, 0, 0);
            mGridLayout->addWidget(mMoveButton, 1, 0);

            mGridLayout->addWidget(mSelectButton, 2, 0);
            mGridLayout->addWidget(mBrushButton, 0, 1);

            mGridLayout->addWidget(mPolylineButton, 1, 1);
            mGridLayout->addWidget(mSmudgeButton, 2, 1);

            mGridLayout->addWidget(mPenButton, 0, 2);
            mGridLayout->addWidget(mHandButton, 1, 2);

            mGridLayout->addWidget(mPencilButton, 2, 2);
            mGridLayout->addWidget(mBucketButton, 0, 3);

            mGridLayout->addWidget(mEyedropperButton, 1, 3);
            mGridLayout->addWidget(mEraserButton, 2, 3);
        }
        else if (geom.height() > buttonSize.height() * 3)
        {
            mGridLayout->addWidget(mClearButton, 0, 0);
            mGridLayout->addWidget(mMoveButton, 1, 0);

            mGridLayout->addWidget(mSelectButton, 0, 1);
            mGridLayout->addWidget(mBrushButton, 1, 1);

            mGridLayout->addWidget(mPolylineButton, 0, 2);
            mGridLayout->addWidget(mSmudgeButton, 1, 2);

            mGridLayout->addWidget(mPenButton, 0, 3);
            mGridLayout->addWidget(mHandButton, 1, 3);

            mGridLayout->addWidget(mPencilButton, 0, 4);
            mGridLayout->addWidget(mBucketButton, 1, 4);

            mGridLayout->addWidget(mEyedropperButton, 0, 5);
            mGridLayout->addWidget(mEraserButton, 1, 5);
        }
        else
        {
            mGridLayout->addWidget(mClearButton, 0, 0);
            mGridLayout->addWidget(mMoveButton, 0, 1);
            mGridLayout->addWidget(mSelectButton, 0, 2);
            mGridLayout->addWidget(mBrushButton, 0, 3);
            mGridLayout->addWidget(mPolylineButton, 0, 4);
            mGridLayout->addWidget(mSmudgeButton, 0, 5);
            mGridLayout->addWidget(mPenButton, 0, 6);
            mGridLayout->addWidget(mHandButton, 0, 7);
            mGridLayout->addWidget(mPencilButton, 0, 8);
            mGridLayout->addWidget(mBucketButton, 0, 9);
            mGridLayout->addWidget(mEyedropperButton, 0, 10);
            mGridLayout->addWidget(mEraserButton, 0, 11);
        }
    }
    
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("ToolBoxGeom", this->saveGeometry());
}

QToolButton* ToolBoxWidget::newToolButton(const QIcon& icon, QString strToolTip)
{
    QToolButton* toolButton = new QToolButton(this);
    toolButton->setAutoRaise(true);
    toolButton->setIconSize(QSize(24, 24));
    toolButton->setFixedSize(32, 32);
#ifdef __APPLE__
    // Only Mac needs this. ToolButton is naturally borderless on Win/Linux.
    QString sStyle =
        "QToolButton { border: 0px; }"
        "QToolButton:pressed { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }"
        "QToolButton:checked { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }";
    toolButton->setStyleSheet(sStyle);
#endif
    toolButton->setIcon(icon);
    toolButton->setToolTip(strToolTip);

    return toolButton;
}

void ToolBoxWidget::pencilOn()
{
    editor()->tools()->setCurrentTool(PENCIL);

    deselectAllTools();
    mPencilButton->setChecked(true);
}

void ToolBoxWidget::eraserOn()
{
    editor()->tools()->setCurrentTool(ERASER);

    deselectAllTools();
    mEraserButton->setChecked(true);
}

void ToolBoxWidget::selectOn()
{
    editor()->tools()->setCurrentTool(SELECT);

    deselectAllTools();
    mSelectButton->setChecked(true);
}

void ToolBoxWidget::moveOn()
{
    editor()->tools()->setCurrentTool(MOVE);

    deselectAllTools();
    mMoveButton->setChecked(true);
}

void ToolBoxWidget::penOn()
{
    editor()->tools()->setCurrentTool(PEN);

    deselectAllTools();
    mPenButton->setChecked(true);
}

void ToolBoxWidget::handOn()
{
    editor()->tools()->setCurrentTool(HAND);

    deselectAllTools();
    mHandButton->setChecked(true);
}

void ToolBoxWidget::polylineOn()
{
    editor()->tools()->setCurrentTool(POLYLINE);

    deselectAllTools();
    mPolylineButton->setChecked(true);
}

void ToolBoxWidget::bucketOn()
{
    editor()->tools()->setCurrentTool(BUCKET);

    deselectAllTools();
    mBucketButton->setChecked(true);
}

void ToolBoxWidget::eyedropperOn()
{
    editor()->tools()->setCurrentTool(EYEDROPPER);

    deselectAllTools();
    mEyedropperButton->setChecked(true);
}

void ToolBoxWidget::brushOn()
{
    editor()->tools()->setCurrentTool(BRUSH);

    deselectAllTools();
    mBrushButton->setChecked(true);
}

void ToolBoxWidget::smudgeOn()
{
    editor()->tools()->setCurrentTool(SMUDGE);

    deselectAllTools();
    mSmudgeButton->setChecked(true);
}

void ToolBoxWidget::deselectAllTools()
{
    mPencilButton->setChecked(false);
    mEraserButton->setChecked(false);
    mSelectButton->setChecked(false);
    mMoveButton->setChecked(false);
    mHandButton->setChecked(false);
    mPenButton->setChecked(false);
    mPolylineButton->setChecked(false);
    mBucketButton->setChecked(false);
    mEyedropperButton->setChecked(false);
    mBrushButton->setChecked(false);
    mSmudgeButton->setChecked(false);
}
