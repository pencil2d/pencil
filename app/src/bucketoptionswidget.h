/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef BUCKETOPTIONSWIDGET_H
#define BUCKETOPTIONSWIDGET_H

#include <QWidget>
#include "pencildef.h"

class Editor;
class Layer;

namespace Ui {
class BucketOptionsWidget;
}

class BucketOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BucketOptionsWidget(Editor* editor, QWidget* parent);
    ~BucketOptionsWidget();

    void setStrokeWidth(qreal value);
    void setColorToleranceEnabled(bool enabled);
    void setFillExpandEnabled(bool enabled);
    void setFillExpand(int value);
    void setColorTolerance(int tolerance);
    void setFillReferenceMode(int referenceMode);
    void setFillMode(int mode);

    void onPropertyChanged(ToolType, const ToolPropertyType propertyType);
    void onLayerChanged(int);

private:
    void updatePropertyVisibility();

    Ui::BucketOptionsWidget *ui;
    Editor* mEditor = nullptr;

    const static int MAX_EXPAND = 25;
    const static int MAX_COLOR_TOLERANCE = 100;
    const static int MAX_STROKE_THICKNESS = 200;
};

#endif // BUCKETOPTIONSWIDGET_H
