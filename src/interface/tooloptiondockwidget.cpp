
#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QSettings>
#include <QDebug>
#include "spinslider.h"
#include "tooloptiondockwidget.h"
#include "editor.h"

ToolOptionDockWidget::ToolOptionDockWidget(QWidget *parent) :
    QDockWidget(parent)
{
    createUI();
}

void ToolOptionDockWidget::createUI()
{
    setMinimumWidth(110);
    QFrame* optionGroup = new QFrame();
    QGridLayout* optionLay = new QGridLayout();
    optionLay->setMargin(8);
    optionLay->setSpacing(8);

    QSettings settings("Pencil","Pencil");

    QLabel* colourLabel = new QLabel();
    colourLabel->setText(tr("Color:"));
    colourLabel->setFont( QFont("Helvetica", 10) );

    QPixmap colourSwatch(30,30);
    colourSwatch.fill(Qt::black);
    colorChooseButton = new QToolButton(this);
    colorChooseButton->setIcon(colourSwatch);
    colorChooseButton->setToolTip(tr("Display Colors"));

    sizeSlider = new SpinSlider(tr("Size"), "log", "real", 0.2, 200.0, this);
    sizeSlider->setValue(settings.value("pencilWidth").toDouble());
    sizeSlider->setToolTip(tr("Set Pen Width <br><b>[SHIFT]+drag</b><br>for quick adjustment"));

    featherSlider = new SpinSlider(tr("Feather"), "log", "real", 0.2, 200.0, this);
    featherSlider->setValue(settings.value("pencilFeather").toDouble());
    featherSlider->setToolTip(tr("Set Pen Feather <br><b>[CTRL]+drag</b><br>for quick adjustment"));

    //opacitySlider = new SpinSlider("Opacity", "linear", "real", 0.0, 1.0, this);
    //opacitySlider->setValue(settings.value("pencilOpacity").toDouble());

    usePressureBox = new QCheckBox(tr("Pressure"));
    usePressureBox->setToolTip(tr("Size with pressure"));
    usePressureBox->setFont( QFont("Helvetica", 10) );
    usePressureBox->setChecked(true);

    makeInvisibleBox = new QCheckBox(tr("Invisible"));
    makeInvisibleBox->setToolTip(tr("Make invisible"));
    makeInvisibleBox->setFont( QFont("Helvetica", 10) );
    makeInvisibleBox->setChecked(false);

    preserveAlphaBox = new QCheckBox(tr("Alpha"));
    preserveAlphaBox->setToolTip(tr("Preserve Alpha"));
    preserveAlphaBox->setFont( QFont("Helvetica", 10) );
    preserveAlphaBox->setChecked(false);

    followContourBox = new QCheckBox(tr("Contours"));
    followContourBox->setToolTip(tr("Stop at contours"));
    followContourBox->setFont( QFont("Helvetica", 10) );
    followContourBox->setChecked(false);

    optionLay->addWidget(colourLabel,6,0);
    optionLay->addWidget(colorChooseButton,6,1);

    optionLay->addWidget(sizeSlider,8,0,1,2);
    optionLay->addWidget(featherSlider,9,0,1,2);
    optionLay->addWidget(usePressureBox,11,0,1,2);
    optionLay->addWidget(preserveAlphaBox,12,0,1,2);
    optionLay->addWidget(followContourBox,13,0,1,2);
    optionLay->addWidget(makeInvisibleBox,14,0,1,2);

    optionLay->setRowStretch(15,1);

    optionGroup->setLayout(optionLay);

    setWindowTitle(tr("Options"));
    setWidget(optionGroup);
}

void ToolOptionDockWidget::makeConnectionToEditor(Editor *editor)
{
    connect(usePressureBox,   SIGNAL(clicked(bool)), editor, SLOT(applyPressure(bool)));
    connect(makeInvisibleBox, SIGNAL(clicked(bool)), editor, SLOT(applyInvisibility(bool)));
    connect(preserveAlphaBox, SIGNAL(clicked(bool)), editor, SLOT(applyPreserveAlpha(bool)));
    connect(followContourBox, SIGNAL(clicked(bool)), editor, SLOT(applyFollowContour(bool)));

    connect(sizeSlider, SIGNAL(valueChanged(qreal)), editor, SLOT(applyWidth(qreal)));
    connect(featherSlider, SIGNAL(valueChanged(qreal)), editor, SLOT(applyFeather(qreal)));
    connect(colorChooseButton, SIGNAL(clicked()), this, SLOT(clickColorChooseButton()));

    connect(editor, SIGNAL(penWidthValueChange(qreal)), this, SLOT(setPenWidth(qreal)));
    connect(editor, SIGNAL(penFeatherValueChange(qreal)), this, SLOT(setPenFeather(qreal)));
    connect(editor, SIGNAL(penInvisiblityValueChange(int)), this, SLOT(setPenInvisibility(int)));
    connect(editor, SIGNAL(penPreserveAlphaValueChange(int)), this, SLOT(setPreserveAlpha(int)));
    connect(editor, SIGNAL(penPressureValueChange(int)), this, SLOT(setPressure(int)));
    connect(editor, SIGNAL(penFollowContourValueChange(int)), this, SLOT(setFollowContour(int)));
    connect(editor, SIGNAL(penColorValueChange(QColor)), this, SLOT(setColour(QColor)));

    qDebug() << "Tool Option Widget connect to Editor";
}

// SLOTS
// ================
void ToolOptionDockWidget::setPenWidth(qreal width)
{

    if (width < 0)
    {
        // disable tool if -1
        sizeSlider->setEnabled(false);
        return;
    }

    sizeSlider->setEnabled(true);
    sizeSlider->setValue(width);
}

void ToolOptionDockWidget::setPenFeather(qreal featherValue)
{
    //qDebug("Set Feather Value %lf", featherValue);
    if (featherValue < 0)
    {
        //disabled
        featherSlider->setEnabled(false);
        return;
    }

    featherSlider->setEnabled(true);
    featherSlider->setValue(featherValue);
}

void ToolOptionDockWidget::setPenInvisibility(int x)   // x = -1, 0, 1
{
    if (x < 0)
    {
        makeInvisibleBox->setEnabled(false);
        return;
    }

    makeInvisibleBox->setEnabled(true);
    makeInvisibleBox->setChecked( x > 0 );
}

void ToolOptionDockWidget::setPressure(int x)   // x = -1, 0, 1
{
    if (x < 0)
    {
        usePressureBox->setEnabled(false);
        return;
    }

    usePressureBox->setEnabled(true);
    usePressureBox->setChecked(x>0);
}

void ToolOptionDockWidget::setPreserveAlpha(int x)   // x = -1, 0, 1
{
    qDebug() << "Setting - Preserve Alpha=" << x;

    if (x < 0)
    {
        preserveAlphaBox->setEnabled(false);
        return;
    }

    preserveAlphaBox->setEnabled(true);
    preserveAlphaBox->setChecked(x > 0);
}

void ToolOptionDockWidget::setFollowContour(int x)   // x = -1, 0, 1
{
    if (x < 0)
    {
        followContourBox->setEnabled(false);
        return;
    }

    followContourBox->setEnabled(true);
    followContourBox->setChecked(x>0);
}

void ToolOptionDockWidget::setColour(QColor x)
{
    QPixmap colourSwatch(30, 30);
    colourSwatch.fill(x);
    colorChooseButton->setIcon(colourSwatch);
}

void ToolOptionDockWidget::clickColorChooseButton()
{
    // TODO:
    qDebug("Clock Colour choose button.");
}
