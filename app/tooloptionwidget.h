/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef TOOLOPTIONDOCKWIDGET_H
#define TOOLOPTIONDOCKWIDGET_H

#include "basedockwidget.h"
#include "pencildef.h"

namespace Ui
{
    class ToolOptions;
}
class QToolButton;
class SpinSlider;
class QCheckBox;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QGroupBox;
class Editor;
class BaseTool;


class ToolOptionWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit ToolOptionWidget(QWidget* parent);
    virtual ~ToolOptionWidget();

    void initUI() override;
    void updateUI() override;

    void makeConnectionToEditor(Editor* editor);

public slots:
    void onToolPropertyChanged(ToolType, ToolPropertyType);
    void onToolChanged(ToolType);
    void setVisibility(BaseTool*);

private:
    void setPenWidth(qreal);
    void setPenFeather(qreal);
    void setUseFeather(bool);
    void setPenInvisibility(int);
    void setPressure(int);
    void setPreserveAlpha(int);
    void setVectorMergeEnabled(int);
    void setAA(int);
    void setInpolLevel(int);
    void setTolerance(int);
    void setFillContour(int);

    void disableAllOptions();
    void createUI();

private:
    Ui::ToolOptions* ui = nullptr;
};

#endif // TOOLOPTIONDOCKWIDGET_H
