
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
    choseColour = new QToolButton(this);
    choseColour->setIcon(colourSwatch);
    choseColour->setToolTip("Display Colors");

    sizeSlider = new SpinSlider("Size", "log", "real", 0.2, 200.0, this);
    sizeSlider->setValue(settings.value("pencilWidth").toDouble());
    sizeSlider->setToolTip("Set Pen Width");

    featherSlider = new SpinSlider("Feather", "log", "real", 0.2, 200.0, this);
    featherSlider->setValue(settings.value("pencilFeather").toDouble());

    //opacitySlider = new SpinSlider("Opacity", "linear", "real", 0.0, 1.0, this);
    //opacitySlider->setValue(settings.value("pencilOpacity").toDouble());

    usePressureBox = new QCheckBox("Pressure");
    usePressureBox->setToolTip("Size with pressure");
    usePressureBox->setFont( QFont("Helvetica", 10) );
    usePressureBox->setChecked(true);

    makeInvisibleBox = new QCheckBox("Invisible");
    makeInvisibleBox->setToolTip("Make invisible");
    makeInvisibleBox->setFont( QFont("Helvetica", 10) );
    makeInvisibleBox->setChecked(false);

    preserveAlphaBox = new QCheckBox("Alpha");
    preserveAlphaBox->setToolTip("Preserve Alpha");
    preserveAlphaBox->setFont( QFont("Helvetica", 10) );
    preserveAlphaBox->setChecked(false);

    followContourBox = new QCheckBox("Contours");
    followContourBox->setToolTip("Stop at contours");
    followContourBox->setFont( QFont("Helvetica", 10) );
    followContourBox->setChecked(false);

    optionLay->addWidget(colourLabel,6,0);
    optionLay->addWidget(choseColour,6,1);

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
    connect(choseColour, SIGNAL(clicked()), editor, SLOT(showPalette()));
    //connect(toolSet, SIGNAL(opacityClick(qreal)), editor, SLOT(applyOpacity(qreal)));

    qDebug() << "Tool Option Widget connect to Editor";
}
