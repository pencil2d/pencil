/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2017 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <QLabel>
#include <QToolButton>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QSettings>
#include <QDebug>
#include "spinslider.h"
#include "toolmanager.h"
#include "tooloptionwidget.h"
#include "editor.h"
#include "util.h"
#include "layer.h"
#include "layermanager.h"

ToolOptionWidget::ToolOptionWidget( QWidget* parent ) : BaseDockWidget( parent )
{
    setWindowTitle( tr( "Options", "Window title of tool options like pen width, feather etc.." ) );
}

ToolOptionWidget::~ToolOptionWidget()
{
}

void ToolOptionWidget::initUI()
{
    createUI();
}

void ToolOptionWidget::updateUI()
{
    BaseTool* currentTool = editor()->tools()->currentTool();
    Q_ASSERT( currentTool );

    disableAllOptions();

    mSizeSlider->setVisible( currentTool->isPropertyEnabled( WIDTH ) );
    mBrushSpinBox->setVisible( currentTool->isPropertyEnabled( WIDTH) );
    mFeatherSlider->setVisible( currentTool->isPropertyEnabled( FEATHER ) );
    mUseFeatherBox->setVisible( currentTool->isPropertyEnabled( FEATHER ) );
    mFeatherSpinBox->setVisible( currentTool->isPropertyEnabled( FEATHER) );
    mUseBezierBox->setVisible( currentTool->isPropertyEnabled( BEZIER ) );
    mUsePressureBox->setVisible( currentTool->isPropertyEnabled( PRESSURE ) );
    mMakeInvisibleBox->setVisible( currentTool->isPropertyEnabled( INVISIBILITY ) );
    mPreserveAlphaBox->setVisible( currentTool->isPropertyEnabled( PRESERVEALPHA ) );
    mUseAABox->setVisible(currentTool->isPropertyEnabled( ANTI_ALIASING ) );
    mInpolLevelsBox->setVisible(currentTool->isPropertyEnabled( INTERPOLATION ) );
    mToleranceSlider->setVisible(currentTool->isPropertyEnabled( TOLERANCE ) );
    mToleranceSpinBox->setVisible(currentTool->isPropertyEnabled( TOLERANCE ) );
    mFillContour->setVisible( currentTool->isPropertyEnabled( FILLCONTOUR ) );

    visibilityOnLayer();

    const Properties& p = currentTool->properties;

    setPenWidth( p.width );
    setPenFeather( p.feather );
    setUseFeather( p.useFeather );
    setPressure( p.pressure );
    setPenInvisibility( p.invisibility );
    setPreserveAlpha( p.preserveAlpha );
    setVectorMergeEnabled( p.vectorMergeEnabled );
    setAA(p.useAA);
    setInpolLevel(p.inpolLevel);
    setTolerance(p.tolerance);
    setFillContour(p.useFillContour);
}

void ToolOptionWidget::createUI()
{
    setMinimumWidth( 115 );
    setMaximumWidth(300);

    QFrame* optionGroup = new QFrame();
    QGridLayout* pLayout = new QGridLayout();
    pLayout->setMargin( 8 );
    pLayout->setSpacing( 8 );

    QSettings settings( PENCIL2D, PENCIL2D );

    mSizeSlider = new SpinSlider( tr( "Brush" ), SpinSlider::EXPONENT, SpinSlider::INTEGER, 1, 200, this );
    mSizeSlider->setValue( settings.value( "brushWidth" ).toDouble() );
    mSizeSlider->setToolTip( tr( "Set Pen Width <br><b>[SHIFT]+drag</b><br>for quick adjustment" ) );

    mBrushSpinBox = new QSpinBox(this);
    mBrushSpinBox->setRange(1,200);
    mBrushSpinBox->setValue(settings.value( "brushWidth" ).toDouble() );

    mFeatherSlider = new SpinSlider( tr( "Feather" ), SpinSlider::LOG, SpinSlider::INTEGER, 2, 64, this );
    mFeatherSlider->setValue( settings.value( "brushFeather" ).toDouble() );
    mFeatherSlider->setToolTip( tr( "Set Pen Feather <br><b>[CTRL]+drag</b><br>for quick adjustment" ) );

    mFeatherSpinBox = new QSpinBox(this);
    mFeatherSpinBox->setRange(2,64);
    mFeatherSpinBox->setValue(settings.value( "brushFeather" ).toDouble() );

    mUseFeatherBox = new QCheckBox( tr( "Use Feather" ) );
    mUseFeatherBox->setToolTip( tr( "Enable or disable feathering" ) );
    mUseFeatherBox->setFont( QFont( "Helvetica", 10 ) );
    mUseFeatherBox->setChecked( settings.value( "brushUseFeather" ).toBool() );

    mUseBezierBox = new QCheckBox( tr( "Bezier" ) );
    mUseBezierBox->setToolTip( tr( "Bezier curve fitting" ) );
    mUseBezierBox->setFont( QFont( "Helvetica", 10 ) );
    mUseBezierBox->setChecked( false );

    mUsePressureBox = new QCheckBox( tr( "Pressure" ) );
    mUsePressureBox->setToolTip( tr( "Size with pressure" ) );
    mUsePressureBox->setFont( QFont( "Helvetica", 10 ) );
    mUsePressureBox->setChecked( true );

    mUseAABox = new QCheckBox( tr( "Anti-Aliasing" ) );
    mUseAABox->setToolTip( tr( "Enable Anti-Aliasing" ) );
    mUseAABox->setFont( QFont( "Helvetica", 10 ) );
    mUseAABox->setChecked( true );

    mFillContour = new QCheckBox( tr( "Fill Contour", "ToolOptions" ) );
    mFillContour->setToolTip( tr( "Contour will be filled" ) );
    mFillContour->setFont( QFont( "Helvetica", 10 ) );
    mFillContour->setChecked( true );

    mInpolLevelsBox = new QGroupBox ( tr( "Stabilization" ) );
    mInpolLevelsBox->setFlat(true);
    mInpolLevelsBox->setFont(QFont( "Helvetica", 10 ) );
    //mInpolLevelsBox->setStyleSheet();

    mInpol = new QComboBox();
    mInpol->addItems(QStringList() << tr("No interpolation") << tr("Simple line interpolation") << tr("Strong line interpolation"));
    mInpol->setFont(QFont( "Helvetica", 10));

    QVBoxLayout *inpolLayout = new QVBoxLayout();
    inpolLayout->addWidget(mInpol);
    inpolLayout->setSpacing(2);
    mInpolLevelsBox->setLayout(inpolLayout);

    mToleranceSlider = new SpinSlider( tr( "Color Tolerance" ), SpinSlider::LINEAR, SpinSlider::INTEGER, 1, 100, this );
    mToleranceSlider->setValue( settings.value( "Tolerance" ).toInt() );
    mToleranceSlider->setToolTip( tr( "The extend to which the color variation will be treated as being equal" ) );

    mToleranceSpinBox = new QSpinBox(this);
    mToleranceSpinBox->setRange(1,100);
    mToleranceSpinBox->setValue(settings.value( "Tolerance" ).toInt() );

    mMakeInvisibleBox = new QCheckBox( tr( "Invisible" ) );
    mMakeInvisibleBox->setToolTip( tr( "Make invisible" ) );
    mMakeInvisibleBox->setFont( QFont( "Helvetica", 10 ) );
    mMakeInvisibleBox->setChecked( false );

    mPreserveAlphaBox = new QCheckBox( tr( "Alpha" ) );
    mPreserveAlphaBox->setToolTip( tr( "Preserve Alpha" ) );
    mPreserveAlphaBox->setFont( QFont( "Helvetica", 10 ) );
    mPreserveAlphaBox->setChecked( false );

    mVectorMergeBox = new QCheckBox( tr( "Merge" ) );
    mVectorMergeBox->setToolTip( tr( "Merge vector lines when they are close together" ) );
    mVectorMergeBox->setFont( QFont( "Helvetica", 10 ) );
    mVectorMergeBox->setChecked( false );

    pLayout->addWidget( mSizeSlider, 1, 0, 1, 2 );
    pLayout->addWidget( mBrushSpinBox, 1, 2, 1, 2);
    pLayout->addWidget( mFeatherSlider, 2, 0, 1, 2 );
    pLayout->addWidget( mFeatherSpinBox, 2, 2, 1, 2 );
    pLayout->addWidget( mUseFeatherBox, 3, 0, 1, 2 );
    pLayout->addWidget( mUseBezierBox, 4, 0, 1, 2 );
    pLayout->addWidget( mUsePressureBox, 5, 0, 1, 2 );
    pLayout->addWidget( mUseAABox, 6, 0, 1, 2);
    pLayout->addWidget( mPreserveAlphaBox, 7, 0, 1, 2 );
    pLayout->addWidget( mMakeInvisibleBox, 8, 0, 1, 2 );
    pLayout->addWidget( mVectorMergeBox, 9, 0, 1, 2 );
    pLayout->addWidget( mInpolLevelsBox, 10, 0, 1, 4);
    pLayout->addWidget( mToleranceSlider, 2, 0, 1, 2);
    pLayout->addWidget( mToleranceSpinBox, 2, 2, 1, 2);
    pLayout->addWidget( mFillContour, 1, 0, 1, 2);

    pLayout->setRowStretch( 17, 1 );

    optionGroup->setLayout( pLayout );

    setWidget( optionGroup );
}

void ToolOptionWidget::makeConnectionToEditor( Editor* editor )
{
    auto toolManager = editor->tools();

    connect( mUseBezierBox, &QCheckBox::clicked, toolManager, &ToolManager::setBezier );
    connect( mUsePressureBox, &QCheckBox::clicked, toolManager, &ToolManager::setPressure );
    connect( mMakeInvisibleBox, &QCheckBox::clicked, toolManager, &ToolManager::setInvisibility );
    connect( mPreserveAlphaBox, &QCheckBox::clicked, toolManager, &ToolManager::setPreserveAlpha );

    connect( mSizeSlider, &SpinSlider::valueChanged, toolManager, &ToolManager::setWidth );
    connect( mFeatherSlider, &SpinSlider::valueChanged, toolManager, &ToolManager::setFeather );

    connect( mBrushSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), toolManager, &ToolManager::setWidth );
    connect( mFeatherSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), toolManager, &ToolManager::setFeather );

    connect( mUseFeatherBox, &QCheckBox::clicked, toolManager, &ToolManager::setUseFeather );

    connect( mVectorMergeBox, &QCheckBox::clicked, toolManager, &ToolManager::setVectorMergeEnabled );
    connect( mUseAABox, &QCheckBox::clicked, toolManager, &ToolManager::setAA );

    connect( mInpol, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), toolManager, &ToolManager::setInpolLevel);

    connect( mToleranceSlider, &SpinSlider::valueChanged, toolManager, &ToolManager::setTolerance);
    connect( mToleranceSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), toolManager, &ToolManager::setTolerance);

    connect( mFillContour, &QCheckBox::clicked, toolManager, &ToolManager::setUseFillContour );

    connect( toolManager, &ToolManager::toolChanged, this, &ToolOptionWidget::onToolChanged );
    connect( toolManager, &ToolManager::toolPropertyChanged, this, &ToolOptionWidget::onToolPropertyChanged );
}

void ToolOptionWidget::onToolPropertyChanged( ToolType, ToolPropertyType ePropertyType )
{
    const Properties& p = editor()->tools()->currentTool()->properties;

    switch ( ePropertyType )
    {
        case WIDTH:
            setPenWidth( p.width );
            break;
        case FEATHER:
            setPenFeather( p.feather );
            break;
        case USEFEATHER:
            setUseFeather( p.useFeather );
            break;
        case PRESSURE:
            setPressure( p.pressure );
            break;
        case INVISIBILITY:
            setPenInvisibility( p.invisibility );
            break;
        case PRESERVEALPHA:
            setPreserveAlpha( p.preserveAlpha );
            break;
        case VECTORMERGE:
            setVectorMergeEnabled(p.vectorMergeEnabled);
            break;
        case ANTI_ALIASING:
            setAA(p.useAA);
            break;
        case INTERPOLATION:
            setInpolLevel(p.inpolLevel);
            break;
        case TOLERANCE:
            setTolerance(p.tolerance);
            break;
        case FILLCONTOUR:
            setFillContour(p.useFillContour);
            break;
        default:
            break;
    }
}

void ToolOptionWidget::visibilityOnLayer()
{
    auto currentLayerType = editor()->layers()->currentLayer()->type();
    auto propertyType = editor()->tools()->currentTool()->type();

    if (currentLayerType == Layer::VECTOR)
    {
        switch (propertyType)
        {
            case SMUDGE:
                mSizeSlider->setVisible(false);
                mBrushSpinBox->setVisible(false);
                mUsePressureBox->setVisible(false);
                mFeatherSlider->setVisible(false);
                mFeatherSpinBox->setVisible(false);
                mUseFeatherBox->setVisible(false);
                break;
            case PENCIL:
                mSizeSlider->setVisible(false);
                mBrushSpinBox->setVisible(false);
                mUsePressureBox->setVisible(false);
                break;
            case BUCKET:
                mSizeSlider->setLabel(tr("Stroke Thickness"));
                mToleranceSlider->setVisible(false);
                mToleranceSpinBox->setVisible(false);
                break;
            default:
                mSizeSlider->setLabel(tr("Width"));
                mToleranceSlider->setVisible(false);
                mToleranceSpinBox->setVisible(false);
                mUseAABox->setVisible(false);
                break;
        }
    }
    else
    {
        switch (propertyType)
        {
            case PENCIL:
                mFillContour->setVisible(false);
                break;
            case BUCKET:
                mBrushSpinBox->setVisible(false);
                mSizeSlider->setVisible(false);
                break;
            default:
                mMakeInvisibleBox->setVisible(false);
                break;

        }
    }
}

void ToolOptionWidget::onToolChanged( ToolType )
{
    updateUI();
}

void ToolOptionWidget::setPenWidth( qreal width )
{
    SignalBlocker b( mSizeSlider );
    mSizeSlider->setEnabled( true );
    mSizeSlider->setValue( width );

    SignalBlocker b2( mBrushSpinBox );
    mBrushSpinBox->setEnabled( true );
    mBrushSpinBox->setValue( width );
}

void ToolOptionWidget::setPenFeather( qreal featherValue )
{
    SignalBlocker b( mFeatherSlider );
    mFeatherSlider->setEnabled( true );
    mFeatherSlider->setValue( featherValue );
    
    SignalBlocker b2( mFeatherSpinBox );
    mFeatherSpinBox->setEnabled( true );
    mFeatherSpinBox->setValue( featherValue );
}

void ToolOptionWidget::setUseFeather( bool useFeather)
{
    SignalBlocker b( mUseFeatherBox );
    mUseFeatherBox->setEnabled(true);
    mUseFeatherBox->setChecked(useFeather);
}

void ToolOptionWidget::setPenInvisibility( int x )
{
    SignalBlocker b( mMakeInvisibleBox );
    mMakeInvisibleBox->setEnabled( true );
    mMakeInvisibleBox->setChecked( x > 0 );
}

void ToolOptionWidget::setPressure( int x )
{
    SignalBlocker b( mUsePressureBox );
    mUsePressureBox->setEnabled( true );
    mUsePressureBox->setChecked( x > 0 );
}

void ToolOptionWidget::setPreserveAlpha( int x )
{
    qDebug() << "Setting - Preserve Alpha=" << x;

    SignalBlocker b( mPreserveAlphaBox );
    mPreserveAlphaBox->setEnabled( true );
    mPreserveAlphaBox->setChecked( x > 0 );
}

void ToolOptionWidget::setVectorMergeEnabled(int x)
{
    qDebug() << "Setting - Vector Merge Enabled=" << x;

    SignalBlocker b( mVectorMergeBox );
    mVectorMergeBox->setEnabled( true );
    mVectorMergeBox->setChecked( x > 0 );
}

void ToolOptionWidget::setAA(int x)
{
    qDebug() << "Setting - Pen AA Enabled=" << x;

    SignalBlocker b( mUseAABox );
    mUseAABox->setEnabled( true );
    mUseAABox->setVisible( false );

    auto layerType = editor()->layers()->currentLayer()->type();

    if (layerType == Layer::BITMAP)
    {
        if (x == -1) {
            mUseAABox->setEnabled(false);
            mUseAABox->setVisible(false);
        } else {
            mUseAABox->setVisible(true);
        }
        mUseAABox->setChecked( x > 0 );
    }
}

void ToolOptionWidget::setInpolLevel(int x)
{
    qDebug() << "Setting - Interpolation level:" << x;

    mInpol->setCurrentIndex(qBound(0, x, mInpol->count()));
}

void ToolOptionWidget::setTolerance(int tolerance)
{
    SignalBlocker b( mToleranceSlider );
    mToleranceSlider->setEnabled( true );
    mToleranceSlider->setValue( tolerance );

    SignalBlocker b2( mToleranceSpinBox );
    mToleranceSpinBox->setEnabled( true );
    mToleranceSpinBox->setValue( tolerance );
}

void ToolOptionWidget::setFillContour(int useFill)
{
    SignalBlocker b( mFillContour );
    mFillContour->setEnabled(true);
    mFillContour->setChecked(useFill > 0);
}

void ToolOptionWidget::disableAllOptions()
{
    mSizeSlider->hide();
    mBrushSpinBox->hide();
    mFeatherSlider->hide();
    mFeatherSpinBox->hide();
    mUseFeatherBox->hide();
    mUseBezierBox->hide();
    mUsePressureBox->hide();
    mMakeInvisibleBox->hide();
    mPreserveAlphaBox->hide();
    mVectorMergeBox->hide();
    mUseAABox->hide();
    mInpolLevelsBox->hide();
    mToleranceSlider->hide();
    mToleranceSpinBox->hide();
    mFillContour->hide();
}
