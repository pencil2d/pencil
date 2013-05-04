#ifndef TOOLOPTIONDOCKWIDGET_H
#define TOOLOPTIONDOCKWIDGET_H

#include <QDockWidget>


class QToolButton;
class SpinSlider;
class QCheckBox;
class Editor;

class ToolOptionDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    ToolOptionDockWidget(QWidget *parent = 0);
    void makeConnectionToEditor(Editor* editor);

    QToolButton* choseColour;
    QPixmap* colourSwatch;
    QCheckBox* usePressureBox;
    QCheckBox* makeInvisibleBox;
    QCheckBox* preserveAlphaBox;
    QCheckBox* followContourBox;
    SpinSlider* sizeSlider;
    SpinSlider* featherSlider;
    //SpinSlider* opacitySlider;

signals:
    
public slots:
    
private:
    void createUI();
};

#endif // TOOLOPTIONDOCKWIDGET_H
