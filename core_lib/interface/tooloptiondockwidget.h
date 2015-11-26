#ifndef TOOLOPTIONDOCKWIDGET_H
#define TOOLOPTIONDOCKWIDGET_H

#include "basedockwidget.h"
#include "pencildef.h"
class QToolButton;
class SpinSlider;
class QCheckBox;
class Editor;
class BaseTool;



class ToolOptionWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit ToolOptionWidget( QWidget* parent );
    virtual ~ToolOptionWidget();

    void initUI() override;
    void updateUI() override;

    void makeConnectionToEditor(Editor* editor);

public slots:
    void onToolPropertyChanged( ToolType, ToolPropertyType );
    void onToolChanged( ToolType );

private:
    void setPenWidth( qreal );
    void setPenFeather( qreal );
    void setPenInvisibility( int );
    void setPressure( int );
    void setPreserveAlpha( int );

    void disableAllOptions();
    void createUI();

    QCheckBox* usePressureBox   = nullptr;
    QCheckBox* makeInvisibleBox = nullptr;
    QCheckBox* preserveAlphaBox = nullptr;
    SpinSlider* sizeSlider      = nullptr;
    SpinSlider* featherSlider   = nullptr;
};

#endif // TOOLOPTIONDOCKWIDGET_H
