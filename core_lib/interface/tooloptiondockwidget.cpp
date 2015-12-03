#include <QLabel>
#include <QToolButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QSettings>
#include <QDebug>
#include "spinslider.h"
#include "toolmanager.h"
#include "tooloptiondockwidget.h"
#include "editor.h"

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
    mFeatherSlider->setVisible( currentTool->isPropertyEnabled( FEATHER ) );
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
    setMinimumWidth( 110 );

    QFrame* optionGroup = new QFrame();
    QGridLayout* pLayout = new QGridLayout();
    pLayout->setMargin( 8 );
    pLayout->setSpacing( 8 );

    QSettings settings( "Pencil", "Pencil" );

    mSizeSlider = new SpinSlider( tr( "Size" ), "log", "real", 0.1, 200.0, this );
    mSizeSlider->setValue( settings.value( "pencilWidth" ).toDouble() );
    mSizeSlider->setToolTip( tr( "Set Pen Width <br><b>[SHIFT]+drag</b><br>for quick adjustment" ) );

    mFeatherSlider = new SpinSlider( tr( "Feather" ), "log", "real", 0.0, 100.0, this );
    mFeatherSlider->setValue( settings.value( "pencilFeather" ).toDouble() );
    mFeatherSlider->setToolTip( tr( "Set Pen Feather <br><b>[CTRL]+drag</b><br>for quick adjustment" ) );

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
    pLayout->addWidget( mFeatherSlider, 9, 0, 1, 2 );
    pLayout->addWidget( mUsePressureBox, 11, 0, 1, 2 );
    pLayout->addWidget( mPreserveAlphaBox, 12, 0, 1, 2 );
    pLayout->addWidget( mMakeInvisibleBox, 14, 0, 1, 2 );

    pLayout->setRowStretch( 15, 1 );

    optionGroup->setLayout( pLayout );

    setWidget( optionGroup );
}

void ToolOptionWidget::makeConnectionToEditor( Editor* editor )
{
    auto toolManager = editor->tools();

    connect( mUsePressureBox, &QCheckBox::clicked, toolManager, &ToolManager::setPressure );
    connect( mMakeInvisibleBox, &QCheckBox::clicked, toolManager, &ToolManager::setInvisibility );
    connect( mPreserveAlphaBox, &QCheckBox::clicked, toolManager, &ToolManager::setPreserveAlpha );

    connect( mSizeSlider, &SpinSlider::valueChanged, toolManager, &ToolManager::setWidth );
    connect( mFeatherSlider, &SpinSlider::valueChanged, toolManager, &ToolManager::setFeather );

    connect( toolManager, &ToolManager::toolChanged, this, &ToolOptionWidget::onToolChanged );
    connect( toolManager, &ToolManager::toolPropertyChanged, this, &ToolOptionWidget::onToolPropertyChanged );
}

void ToolOptionWidget::onToolPropertyChanged( ToolType, ToolPropertyType ePropertyType )
{
    const Properties& p = editor()->tools()->currentTool()->properties;
    qDebug() << p.feather;
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
    QSignalBlocker b( mSizeSlider );
    mSizeSlider->setEnabled( true );
    mSizeSlider->setValue( width );
}

void ToolOptionWidget::setPenFeather( qreal featherValue )
{
    QSignalBlocker b( mFeatherSlider );
    mFeatherSlider->setEnabled( true );
    mFeatherSlider->setValue( featherValue );
}

void ToolOptionWidget::setPenInvisibility( int x )
{
    QSignalBlocker b( mMakeInvisibleBox );
    mMakeInvisibleBox->setEnabled( true );
    mMakeInvisibleBox->setChecked( x > 0 );
}

void ToolOptionWidget::setPressure( int x )
{
    QSignalBlocker b( mUsePressureBox );
    mUsePressureBox->setEnabled( true );
    mUsePressureBox->setChecked( x > 0 );
}

void ToolOptionWidget::setPreserveAlpha( int x )
{
    qDebug() << "Setting - Preserve Alpha=" << x;

    QSignalBlocker b( mPreserveAlphaBox );
    mPreserveAlphaBox->setEnabled( true );
    mPreserveAlphaBox->setChecked( x > 0 );
}

void ToolOptionWidget::disableAllOptions()
{
    mSizeSlider->hide();
    mFeatherSlider->hide();
    mUsePressureBox->hide();
    mMakeInvisibleBox->hide();
    mPreserveAlphaBox->hide();
}
