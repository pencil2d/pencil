#ifndef TOOLOPTIONDOCKWIDGET_H
#define TOOLOPTIONDOCKWIDGET_H

#include "basedockwidget.h"
#include "pencildef.h"


class QToolButton;
class SpinSlider;
class QCheckBox;
class Editor;

class ToolOptionWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit ToolOptionWidget( QWidget* parent );
    ~ToolOptionWidget();

    void initUI() override;
    void updateUI() override;

    void makeConnectionToEditor(Editor* editor);

public slots:
    void setPenWidth(qreal);
    void setPenFeather(qreal);
    void setPenInvisibility(int);
    void setPressure(int);
    void setPreserveAlpha(int);

    void displayToolOptions(QHash<ToolPropertyType, bool> options);

private:
    void disableAllOptions();
    void createUI();

    QCheckBox* usePressureBox   = nullptr;
    QCheckBox* makeInvisibleBox = nullptr;
    QCheckBox* preserveAlphaBox = nullptr;
    SpinSlider* sizeSlider      = nullptr;
    SpinSlider* featherSlider   = nullptr;

};

#endif // TOOLOPTIONDOCKWIDGET_H
