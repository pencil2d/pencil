/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "timelinelayercelleditorwidget.h"

#include "editor.h"
#include "timeline.h"
#include "pencilsettings.h"

#include "layermanager.h"
#include "layercamera.h"
#include "preferencemanager.h"
#include "viewmanager.h"
#include "camerapropertiesdialog.h"
#include "layervisibilitybutton.h"
#include "lineeditwidget.h"

#include <QApplication>
#include <QInputDialog>
#include <QRegularExpression>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QIcon>
#include <QMessageBox>

TimeLineLayerCellEditorWidget::TimeLineLayerCellEditorWidget(QWidget* parent,
                                                             Editor* editor,
                                                             Layer* layer)
    : QWidget(parent),
      mEditor(editor),
      mLayer(layer)
{

    if (mLayer->type() == Layer::BITMAP) mIcon = QIcon(":icons/themes/playful/timeline/cell-bitmap.svg");
    if (mLayer->type() == Layer::VECTOR) mIcon = QIcon(":icons/themes/playful/timeline/cell-vector.svg");
    if (mLayer->type() == Layer::SOUND)  mIcon = QIcon(":icons/themes/playful/timeline/cell-sound.svg");
    if (mLayer->type() == Layer::CAMERA) mIcon = QIcon(":icons/themes/playful/timeline/cell-camera.svg");

    LayerVisibilityButton* layerVisibilityButton = new LayerVisibilityButton(this, LayerVisibilityContext::LOCAL, layer, editor);
    mHBoxLayout = new QHBoxLayout(this);

    mHBoxLayout->addWidget(layerVisibilityButton);
    QLabel* iconLabel = new QLabel(this);
    iconLabel->setPixmap(mIcon.pixmap(mLabelIconSize));
    mHBoxLayout->addWidget(iconLabel);
    mHBoxLayout->addSpacing(4);
    mLayerNameEditWidget = new LineEditWidget(this, mLayer->name());
    mLayerNameEditWidget->setFocusPolicy(Qt::NoFocus);
    mHBoxLayout->addWidget(mLayerNameEditWidget);
    mHBoxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored));
    mHBoxLayout->setContentsMargins(0,0,0,0);
    mHBoxLayout->setSpacing(0);

    connect(layerVisibilityButton, &LayerVisibilityButton::visibilityChanged, this, &TimeLineLayerCellEditorWidget::layerVisibilityChanged);
    connect(mLayerNameEditWidget, &LineEditWidget::editingFinished, this, &TimeLineLayerCellEditorWidget::onFinishedEditingName);
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, mLayerNameEditWidget, &LineEditWidget::deselect);
}

void TimeLineLayerCellEditorWidget::setGeometry(const QRect& rect)
{
    QWidget::setGeometry(rect);
    mHBoxLayout->setGeometry(rect);
}

void TimeLineLayerCellEditorWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPalette palette = QApplication::palette();

    bool isSelected = mEditor->layers()->currentLayer()->id() == mLayer->id();
    paintBackground(painter, palette, isSelected);
}

void TimeLineLayerCellEditorWidget::paintBackground(QPainter& painter, const QPalette& palette, bool isSelected) const
{
    int x = rect().topLeft().x();
    int y = rect().topLeft().y();
    if (isSelected)
    {
        painter.setBrush(palette.color(QPalette::Highlight));
    }
    else
    {
        painter.setBrush(palette.color(QPalette::Base));
    }
    painter.setPen(Qt::NoPen);
    painter.drawRect(x, y, size().width(), size().height());
}

void TimeLineLayerCellEditorWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    if (mEditor->layers()->currentLayer() != mLayer)
    {
        mEditor->layers()->setCurrentLayer(mLayer);
        mEditor->layers()->currentLayer()->deselectAll();
    }

    handleDragStarted(event);
}

void TimeLineLayerCellEditorWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    handleDragging(event);
}

void TimeLineLayerCellEditorWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    handleDragEnded(event);
}

void TimeLineLayerCellEditorWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
    QWidget::mouseDoubleClickEvent(event);
    if (event->buttons() & Qt::LeftButton)
    {
        if (!isInsideLayerVisibilityArea(event))
        {
            editLayerProperties();
        }
    }
}

bool TimeLineLayerCellEditorWidget::isInsideLayerVisibilityArea(QMouseEvent* event) const
{
    return event->pos().x() < 15;
}

void TimeLineLayerCellEditorWidget::handleDragStarted(QMouseEvent* event)
{
    if (isInsideLayerVisibilityArea(event)) {
        return;
    }

    if (event->buttons() & Qt::LeftButton) {
        if (mLayerNameEditWidget->hasFocus()) {
            mLayerNameEditWidget->deselect();
        }

        mIsDraggable = true;
        mDragFromY = y();
        emit drag(DragEvent::STARTED, this, 0, y());
    }
}

void TimeLineLayerCellEditorWidget::handleDragging(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton && mIsDraggable) {
        int delta = event->pos().y() - y();
        int mappedY = mapToParent(event->pos()).y();
        int centerY = mDragFromY + (size().height() * 0.5);
        if (hasDetached(mappedY - centerY)) {
            mDidDetach = true;
            int newCenter = mapToParent(pos()).y() - size().height() * 0.5;
            emit drag(DragEvent::DRAGGING, this, 0, newCenter + delta);
        } else {
            mDidDetach = false;
            emit drag(DragEvent::DRAGGING, this, 0, mDragFromY);
        }
    }
}

void TimeLineLayerCellEditorWidget::handleDragEnded(QMouseEvent*)
{
    if (mIsDraggable) {
        emit drag(DragEvent::ENDED, this, 0, y());

        mIsDraggable = false;
        mDragFromY = y();
        mDidDetach = false;
    }
}

void TimeLineLayerCellEditorWidget::editLayerProperties() const
{
    if (mLayer->type() == Layer::CAMERA) {
        editLayerProperties(static_cast<LayerCamera*>(mLayer));
    }
}

void TimeLineLayerCellEditorWidget::editLayerProperties(LayerCamera* cameraLayer) const
{
    QRegularExpression regex("([\\x{FFEF}-\\x{FFFF}])+");

    CameraPropertiesDialog dialog(cameraLayer->name(), cameraLayer->getViewRect().width(),
                                  cameraLayer->getViewRect().height());
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }
    QString name = dialog.getName().replace(regex, "");

    if (!name.isEmpty())
    {
        mLayerNameEditWidget->setText(name);
        mEditor->layers()->renameLayer(cameraLayer, name);
    }
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_FIELD_W, dialog.getWidth());
    settings.setValue(SETTING_FIELD_H, dialog.getHeight());
    cameraLayer->setViewRect(QRect(-dialog.getWidth() / 2, -dialog.getHeight() / 2, dialog.getWidth(), dialog.getHeight()));
    mEditor->view()->forceUpdateViewTransform();
}

void TimeLineLayerCellEditorWidget::onFinishedEditingName()
{
    QRegularExpression regex("([\\x{FFEF}-\\x{FFFF}])+");
    QString newName = mLayerNameEditWidget->text();

    newName.replace(regex, "");
    if (mWarningShown) {
        return;
    }

    if (newName.isEmpty())
    {
        mWarningShown = true;
        mLayerNameEditWidget->setText(mLayer->name());
        int result = QMessageBox::information(this, tr("Empty name"), tr("The name of the layer cannot be left empty"),
                                 QMessageBox::Ok);

        if (result == QMessageBox::Ok) {
            mWarningShown = false;
        }
        return;
    }

    mEditor->layers()->renameLayer(mLayer, newName);
}
