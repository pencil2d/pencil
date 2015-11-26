#include <QFrame>
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

}

void ToolOptionWidget::createUI()
{
    setMinimumWidth( 110 );

    QFrame* optionGroup = new QFrame();
    QGridLayout* pLayout = new QGridLayout();
    pLayout->setMargin( 8 );
    pLayout->setSpacing( 8 );

    QSettings settings( "Pencil", "Pencil" );

    sizeSlider = new SpinSlider( tr( "Size" ), "linear", "real", 0.1, 200.0, this );
    sizeSlider->setValue( settings.value( "pencilWidth" ).toDouble() );
    sizeSlider->setToolTip( tr( "Set Pen Width <br><b>[SHIFT]+drag</b><br>for quick adjustment" ) );

    featherSlider = new SpinSlider( tr( "Feather" ), "linear", "real", 0.0, 100.0, this );
    featherSlider->setValue( settings.value( "pencilFeather" ).toDouble() );
    featherSlider->setToolTip( tr( "Set Pen Feather <br><b>[CTRL]+drag</b><br>for quick adjustment" ) );

    usePressureBox = new QCheckBox( tr( "Pressure" ) );
    usePressureBox->setToolTip( tr( "Size with pressure" ) );
    usePressureBox->setFont( QFont( "Helvetica", 10 ) );
    usePressureBox->setChecked( true );

    makeInvisibleBox = new QCheckBox( tr( "Invisible" ) );
    makeInvisibleBox->setToolTip( tr( "Make invisible" ) );
    makeInvisibleBox->setFont( QFont( "Helvetica", 10 ) );
    makeInvisibleBox->setChecked( false );

    preserveAlphaBox = new QCheckBox( tr( "Alpha" ) );
    preserveAlphaBox->setToolTip( tr( "Preserve Alpha" ) );
    preserveAlphaBox->setFont( QFont( "Helvetica", 10 ) );
    preserveAlphaBox->setChecked( false );

    pLayout->addWidget( sizeSlider, 8, 0, 1, 2 );
    pLayout->addWidget( featherSlider, 9, 0, 1, 2 );
    pLayout->addWidget( usePressureBox, 11, 0, 1, 2 );
    pLayout->addWidget( preserveAlphaBox, 12, 0, 1, 2 );
    pLayout->addWidget( makeInvisibleBox, 14, 0, 1, 2 );

    pLayout->setRowStretch( 15, 1 );

    optionGroup->setLayout( pLayout );

    setWidget( optionGroup );
}

void ToolOptionWidget::makeConnectionToEditor( Editor* editor )
{
    auto pToolManager = editor->tools();

    connect( usePressureBox, &QCheckBox::clicked, pToolManager, &ToolManager::setPressure );
    connect( makeInvisibleBox, &QCheckBox::clicked, pToolManager, &ToolManager::setInvisibility );
    connect( preserveAlphaBox, &QCheckBox::clicked, pToolManager, &ToolManager::setPreserveAlpha );

    connect( sizeSlider, &SpinSlider::valueChanged, pToolManager, &ToolManager::setWidth );
    connect( featherSlider, &SpinSlider::valueChanged, pToolManager, &ToolManager::setFeather );

    connect( pToolManager, &ToolManager::penWidthValueChange, this, &ToolOptionWidget::setPenWidth );
    connect( pToolManager, &ToolManager::penFeatherValueChange, this, &ToolOptionWidget::setPenFeather );
    connect( pToolManager, &ToolManager::penInvisiblityValueChange, this, &ToolOptionWidget::setPenInvisibility );
    connect( pToolManager, &ToolManager::penPreserveAlphaValueChange, this, &ToolOptionWidget::setPreserveAlpha );
    connect( pToolManager, &ToolManager::penPressureValueChange, this, &ToolOptionWidget::setPressure );
}

// SLOTS
// ================
void ToolOptionWidget::setPenWidth( qreal width )
{
    sizeSlider->setEnabled( true );
    sizeSlider->setValue( width );
}

void ToolOptionWidget::setPenFeather( qreal featherValue )
{
    featherSlider->setEnabled( true );
    featherSlider->setValue( featherValue );
}

void ToolOptionWidget::setPenInvisibility( int x )   // x = -1, 0, 1
{
    makeInvisibleBox->setEnabled( true );
    makeInvisibleBox->setChecked( x > 0 );
}

void ToolOptionWidget::setPressure( int x )   // x = -1, 0, 1
{
    usePressureBox->setEnabled( true );
    usePressureBox->setChecked( x>0 );
}

void ToolOptionWidget::setPreserveAlpha( int x )   // x = -1, 0, 1
{
    qDebug() << "Setting - Preserve Alpha=" << x;

    preserveAlphaBox->setEnabled( true );
    preserveAlphaBox->setChecked( x > 0 );
}

void ToolOptionWidget::disableAllOptions()
{
    sizeSlider->hide();
    featherSlider->hide();
    usePressureBox->hide();
    makeInvisibleBox->hide();
    preserveAlphaBox->hide();
}

void ToolOptionWidget::displayToolOptions(QHash<ToolPropertyType, bool> options)
{
    disableAllOptions();
    QHash<ToolPropertyType, bool>::iterator i;
    for (i = options.begin(); i != options.end(); ++i) {
        if (i.value()) {

            switch ( i.key() ) {
            case WIDTH:
              sizeSlider->show();
              break;
            case FEATHER:
              featherSlider->show();
              break;
            case PRESSURE:
              usePressureBox->show();
              break;
            case INVISIBILITY:
              makeInvisibleBox->show();
              break;
            case PRESERVEALPHA:
              preserveAlphaBox->show();
              break;
            default:
              break;
            }
        }
    }

}
