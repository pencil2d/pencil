#ifndef STROKEOPTIONSWIDGET_H
#define STROKEOPTIONSWIDGET_H

#include <QWidget>

#include "pencildef.h"

class Editor;
class BaseTool;
class QCheckBox;
class SpinSlider;
class QSpinBox;
// class QComboBox;

class StrokeTool;

namespace Ui {
class StrokeOptionsWidget;
}

class StrokeOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StrokeOptionsWidget(Editor* editor, QWidget *parent = nullptr);
    ~StrokeOptionsWidget();

    void updateUI();
    void initUI();

    void makeConnectionToEditor(Editor* editor);

public slots:
    void onToolPropertyChanged(ToolType, ToolPropertyType);
    void onToolChanged(ToolType);

private:
    void setPenWidth(qreal);
    void setPenFeather(qreal);
    void setUseFeather(bool);
    void setPenInvisibility(int);
    void setPressure(int);
    void setAA(int);
    void setStabilizerLevel(int);
    void setFillContour(int);
    void setBezier(bool);
    void setClosedPath(bool);

    void disableAllOptions();
    void setVisibility(BaseTool*);

    StrokeTool* strokeTool();

private:
    Ui::StrokeOptionsWidget *ui;

    Editor* mEditor = nullptr;

};

#endif // STROKEOPTIONSWIDGET_H
