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


#include "basewidget.h"
#include "pencildef.h"

class Editor;
class Layer;
class BucketTool;

#include "toolproperties.h"

namespace Ui {
class BucketOptionsWidget;
}

class BucketOptionsWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit BucketOptionsWidget(Editor* editor, QWidget* parent);
    ~BucketOptionsWidget();

    void initUI() override;
    void updateUI() override;

    void setStrokeWidth(qreal value);
    void setColorToleranceEnabled(bool enabled);
    void setFillExpandEnabled(bool enabled);
    void setFillExpand(int value);
    void setColorTolerance(int tolerance);
    void setFillReferenceMode(int referenceMode);
    void setFillMode(int mode);
    void onLayerChanged(int);

private:
    void makeConnectionsFromUIToModel();
    void makeConnectionsFromModelToUI();
    void updatePropertyVisibility();

    BucketTool* mBucketTool = nullptr;
    BucketToolProperties mSettings;
    Ui::BucketOptionsWidget *ui;
    Editor* mEditor = nullptr;
};

#endif // BUCKETOPTIONSWIDGET_H
