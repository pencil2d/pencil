#ifndef TOOLOPTIONDOCKWIDGET_H
#define TOOLOPTIONDOCKWIDGET_H

#include <QDockWidget>


class QToolButton;
class SpinSlider;
class QCheckBox;

class ToolOptionDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    ToolOptionDockWidget(QWidget *parent = 0);

    QToolButton* choseColour;
    QPixmap* colourSwatch;
    SpinSlider* sizeSlider;
    SpinSlider* featherSlider;
    //SpinSlider* opacitySlider;
    QCheckBox* usePressureBox;
    QCheckBox* makeInvisibleBox;
    QCheckBox* preserveAlphaBox;
    QCheckBox* followContourBox;

signals:
    
public slots:
    
private:
    void createUI();
};

#endif // TOOLOPTIONDOCKWIDGET_H
