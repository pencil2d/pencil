#include <QLabel>
#include <QToolButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QGridLayout>
#include <QSettings>
#include <QDebug>
#include "spinslider.h"
#include "toolmanager.h"
#include "tooloptiondockwidget.h"
#include "editor.h"
#include "util.h"

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

    const Properties& p = currentTool->properties;

    setPenWidth( p.width );
    setPenFeather( p.feather );
    setPressure( p.pressure );
    setPenInvisibility( p.invisibility );
    setPreserveAlpha( p.preserveAlpha );
}

void ToolOptionWidget::createUI()
{
    setMinimumWidth( 115 );

    QFrame* optionGroup = new QFrame();
    QGridLayout* pLayout = new QGridLayout();
    pLayout->setMargin( 8 );
    pLayout->setSpacing( 8 );

    QSettings settings( "Pencil", "Pencil" );

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

    mUseFeatherBox = new QCheckBox( tr( "Use Feather?" ) );
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

    mMakeInvisibleBox = new QCheckBox( tr( "Invisible" ) );
    mMakeInvisibleBox->setToolTip( tr( "Make invisible" ) );
    mMakeInvisibleBox->setFont( QFont( "Helvetica", 10 ) );
    mMakeInvisibleBox->setChecked( false );

    mPreserveAlphaBox = new QCheckBox( tr( "Alpha" ) );
    mPreserveAlphaBox->setToolTip( tr( "Preserve Alpha" ) );
    mPreserveAlphaBox->setFont( QFont( "Helvetica", 10 ) );
    mPreserveAlphaBox->setChecked( false );

    pLayout->addWidget( mSizeSlider, 8, 0, 1, 2 );
    pLayout->addWidget( mBrushSpinBox, 8, 10, 1, 2);
    pLayout->addWidget( mFeatherSlider, 9, 0, 1, 2 );
    pLayout->addWidget( mFeatherSpinBox, 9, 10, 1, 2 );
    pLayout->addWidget( mUseBezierBox, 10, 0, 1, 2 );
    pLayout->addWidget( mUsePressureBox, 11, 0, 1, 2 );
    pLayout->addWidget( mPreserveAlphaBox, 12, 0, 1, 2 );
    pLayout->addWidget( mUseFeatherBox, 13, 0, 1, 2 );
    pLayout->addWidget( mMakeInvisibleBox, 14, 0, 1, 2 );

    pLayout->setRowStretch( 15, 1 );

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
        case PRESSURE:
            setPressure( p.pressure );
            break;
        case INVISIBILITY:
            setPenInvisibility( p.invisibility );
            break;
        case PRESERVEALPHA:
            setPreserveAlpha( p.preserveAlpha );
            break;
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
}
