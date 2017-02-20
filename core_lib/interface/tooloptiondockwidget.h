#ifndef TOOLOPTIONDOCKWIDGET_H
#define TOOLOPTIONDOCKWIDGET_H

#include "basedockwidget.h"
#include "pencildef.h"
class QToolButton;
class SpinSlider;
class QCheckBox;
class QSpinBox;
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
    void setVectorMergeEnabled( int );
    void setAA( int );

    void disableAllOptions();
    void createUI();

    QCheckBox* mUseBezierBox     = nullptr;
    QCheckBox* mUsePressureBox   = nullptr;
    QCheckBox* mUseFeatherBox    = nullptr;
    QCheckBox* mMakeInvisibleBox = nullptr;
    QCheckBox* mPreserveAlphaBox = nullptr;
    QCheckBox* mVectorMergeBox   = nullptr;
    QSpinBox* mBrushSpinBox      = nullptr;
    QSpinBox* mFeatherSpinBox    = nullptr;
    SpinSlider* mSizeSlider      = nullptr;
    SpinSlider* mFeatherSlider   = nullptr;
    QCheckBox* mUseAABox         = nullptr;
};

#endif // TOOLOPTIONDOCKWIDGET_H
