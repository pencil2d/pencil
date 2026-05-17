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

#ifndef TIMELINELAYERCELLEDITORWIDGET_H
#define TIMELINELAYERCELLEDITORWIDGET_H

#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QIcon>

#include "pencildef.h"

class TimeLine;
class Editor;
class Layer;
class LayerCamera;
class PreferenceManager;
class QHBoxLayout;
class QIcon;
class QLabel;
class LineEditWidget;

class TimeLineLayerCellEditorWidget : public QWidget
{
    Q_OBJECT
public:
    TimeLineLayerCellEditorWidget(QWidget* parent,
                                  Editor* editor,
                                  Layer* layer);

    void setGeometry(const QRect& rect);

    void editLayerProperties() const;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    bool didDetach() const { return mDidDetach; }
    const Layer* layer() const { return mLayer; }

signals:
    void drag(const DragEvent& dragEvent, TimeLineLayerCellEditorWidget* cell, int x, int y);
    void layerVisibilityChanged();

private:
    void handleDragStarted(QMouseEvent* event);
    void handleDragging(QMouseEvent* event);
    void handleDragEnded(QMouseEvent* event);

    bool isInsideLayerVisibilityArea(QMouseEvent* event) const;

    bool hasDetached(int yOffset) const { return abs(yOffset) > mDetachThreshold; }

    void paintBackground(QPainter& painter, const QPalette& palette, bool isSelected) const;

    void editLayerProperties(LayerCamera* cameraLayer) const;

    void onFinishedEditingName();

    QSize mLabelIconSize = QSize(20,20);
    bool mDidDetach = false;
    bool mIsDraggable = false;
    bool mWarningShown = false;

    LineEditWidget* mLayerNameEditWidget = nullptr;
    QIcon mIcon;

    int mDetachThreshold = 5;

    Editor* mEditor = nullptr;
    Layer* mLayer = nullptr;

    PreferenceManager* mPrefs = nullptr;
    QHBoxLayout* mHBoxLayout = nullptr;

    int mDragFromY = 0;
};

#endif // TIMELINELAYERCELLEDITORWIDGET_H
