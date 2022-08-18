/*

Pencil2D - Traditional Animation Software
Copyright (C) 2020 Jakob Gahde

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <cmath>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>

#include "editor.h"
#include "elidedlabel.h"
#include "layermanager.h"
#include "scribblearea.h"
#include "toolmanager.h"
#include "viewmanager.h"

#include "statusbar.h"

StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent)
{
    setContentsMargins(3, 0, 3, 0);

    mToolIcon = new QLabel(this);
    addWidget(mToolIcon);
    mToolLabel = new ElidedLabel(this);
    mToolLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    addWidget(mToolLabel, 1);

    mModifiedLabel = new QLabel(this);
    mModifiedLabel->setPixmap(QPixmap(":/icons/save.png"));
    updateModifiedStatus(false);
    addPermanentWidget(mModifiedLabel);

    QLocale locale;
    mZoomBox = new QComboBox(this);
    mZoomBox->addItems(QStringList()
                           << locale.toString(10000., 'f', 1) + locale.percent()
                           << locale.toString(6400., 'f', 1) + locale.percent()
                           << locale.toString(1600., 'f', 1) + locale.percent()
                           << locale.toString(800., 'f', 1) + locale.percent()
                           << locale.toString(400., 'f', 1) + locale.percent()
                           << locale.toString(200., 'f', 1) + locale.percent()
                           << locale.toString(100., 'f', 1) + locale.percent()
                           << locale.toString(75., 'f', 1) + locale.percent()
                           << locale.toString(50., 'f', 1) + locale.percent()
                           << locale.toString(33., 'f', 1) + locale.percent()
                           << locale.toString(25., 'f', 1) + locale.percent()
                           << locale.toString(12., 'f', 1) + locale.percent()
                           << locale.toString(1., 'f', 1) + locale.percent());
    mZoomBox->setMaxCount(mZoomBox->count() + 1);
    mZoomBox->setEditable(true);
    mZoomBox->lineEdit()->setAlignment(Qt::AlignRight);
    connect(mZoomBox, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated), [=](const QString &currentText)
    {
        if (mZoomBox->count() == mZoomBox->maxCount())
        {
            // Keep the size of the list reasonable by preventing user entries
            // insertPolicy is unsuitable as it prevents entering custom values at all
            mZoomBox->removeItem(mZoomBox->maxCount() - 1);
        }
        emit zoomChanged(locale.toDouble(QString(currentText).remove(locale.percent())) / 100);
    });
    addPermanentWidget(mZoomBox);

    mZoomSlider = new QSlider(Qt::Horizontal, this);
    mZoomSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mZoomSlider->setRange(-20, 20);
    mZoomSlider->setTickPosition(QSlider::TicksBelow);
    mZoomSlider->setTickInterval(20);
    connect(mZoomSlider, &QSlider::valueChanged, [this](int value)
    {
        emit zoomChanged(std::pow(10, value / 10.));
    });
    addPermanentWidget(mZoomSlider);
}

void StatusBar::updateToolStatus(ToolType tool)
{
    Q_ASSERT(mEditor);
    switch (tool) {
        case PENCIL:
            mToolLabel->setText(tr("Click to draw. Hold Ctrl and Shift to erase or Alt to select a color from the canvas."));
            break;
        case ERASER:
            mToolLabel->setText(tr("Click to erase."));
            break;
        case SELECT:
            mToolLabel->setText(tr("Click and drag to create or modify a selection. Hold Alt to modify its contents or press Backspace to clear them."));
            break;
        case MOVE:
            mToolLabel->setText(tr("Click and drag to move an object. Hold Ctrl to rotate."));
            break;
        case HAND:
            mToolLabel->setText(tr("Click and drag to pan. Hold Ctrl to zoom or Alt to rotate."));
            break;
        case SMUDGE:
            mToolLabel->setText(tr("Click to liquefy pixels or modify a vector line. Hold Alt to smooth."));
            break;
        case PEN:
            mToolLabel->setText(tr("Click to draw. Hold Ctrl and Shift to erase or Alt to select a color from the canvas."));
            break;
        case POLYLINE:
            if (mEditor->tools()->getTool(tool)->isActive())
            {
                mToolLabel->setText(tr("Click to continue the polyline. Double-click or press enter to complete the line or press Escape to discard it."));
            }
            else
            {
                mToolLabel->setText(tr("Click to create a new polyline. Hold Ctrl and Shift to erase."));
            }
            break;
        case BUCKET:
            mToolLabel->setText(tr("Click to fill an area with the current color. Hold Alt to select a color from the canvas."));
            break;
        case EYEDROPPER:
            mToolLabel->setText(tr("Click to select a color from the canvas."));
            break;
        case BRUSH:
            mToolLabel->setText(tr("Click to paint. Hold Ctrl and Shift to erase or Alt to select a color from the canvas."));
            break;
        default:
            Q_ASSERT(false);
    }

    static QPixmap toolIcons[TOOL_TYPE_COUNT]{
        {":icons/new/svg/pencil_detailed.svg"},
        {":icons/new/svg/eraser_detailed.svg"},
        {":icons/new/svg/selection.svg"},
        {":icons/new/svg/arrow.svg"},
        {":icons/new/svg/hand_detailed.svg"},
        {":icons/new/svg/smudge_detailed.svg"},
        {":icons/new/svg/pen_detailed.svg"},
        {":icons/new/svg/line.svg"},
        {":icons/new/svg/bucket_detailed.svg"},
        {":icons/new/svg/eyedropper_detailed.svg"},
        {":icons/new/svg/brush_detailed.svg"}
    };
    mToolIcon->setPixmap(toolIcons[tool]);
    mToolIcon->setToolTip(BaseTool::TypeName(tool));
}

void StatusBar::updateModifiedStatus(bool modified)
{
    mModifiedLabel->setDisabled(!modified);
    if (modified)
    {
        mModifiedLabel->setToolTip(tr("This file has unsaved changes"));
    }
    else
    {
        mModifiedLabel->setToolTip(tr("This file has no unsaved changes"));
    }
}

void StatusBar::updateZoomStatus()
{
    Q_ASSERT(mEditor);

    QLocale locale;
    QSignalBlocker b1(mZoomBox);
    mZoomBox->setCurrentText(locale.toString(mEditor->view()->scaling() * 100, 'f', 1) + locale.percent());

    QSignalBlocker b2(mZoomSlider);
    mZoomSlider->setValue(static_cast<int>(std::round(std::log10(mEditor->view()->scaling()) * 10)));
}
