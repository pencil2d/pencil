#ifndef TOOLOPTIONDOCKWIDGET_H
#define TOOLOPTIONDOCKWIDGET_H

#include <QDockWidget>
#include "pencildef.h"


class QToolButton;
class SpinSlider;
class QCheckBox;
class Editor;

class ToolOptionWidget : public QDockWidget
{
    Q_OBJECT
public:
    ToolOptionWidget(QWidget *parent = 0);
    void makeConnectionToEditor(Editor* editor);

    QCheckBox* usePressureBox;
    QCheckBox* makeInvisibleBox;
    QCheckBox* preserveAlphaBox;
    SpinSlider* sizeSlider;
    SpinSlider* featherSlider;

signals:

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
};

#endif // TOOLOPTIONDOCKWIDGET_H
