#ifndef STROKEOPTIONSWIDGET_H
#define STROKEOPTIONSWIDGET_H

#include "basewidget.h"
#include "pencildef.h"

class Editor;
class BaseTool;
class QCheckBox;
class SpinSlider;
class QSpinBox;

class StrokeTool;

namespace Ui {
class StrokeOptionsWidget;
}

class StrokeOptionsWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit StrokeOptionsWidget(Editor* editor, QWidget *parent = nullptr);
    ~StrokeOptionsWidget();

    void updateUI() override;
    void initUI() override;

private:
    void makeConnectionFromModelToUI(StrokeTool* strokeTool);
    void makeConnectionFromUIToModel();
    void updateToolConnections(BaseTool* tool);

    void setWidthValue(qreal);
    void setFeatherValue(qreal);
    void setFeatherEnabled(bool);
    void setPenInvisibilityEnabled(bool);
    void setPressureEnabled(bool);
    void setAntiAliasingEnabled(bool);
    void setStabilizerLevel(int);
    void setFillContourEnabled(bool);
    void setBezierPathEnabled(bool);
    void setClosedPathEnabled(bool);

    void setVisibility(BaseTool*);

private:
    Ui::StrokeOptionsWidget *ui;

    StrokeTool* mCurrentTool = nullptr;

    Editor* mEditor = nullptr;

};

#endif // STROKEOPTIONSWIDGET_H
