#include "doubleprogressdialog.h"
#include "ui_doubleprogressdialog.h"

#include <QtMath>

DoubleProgressDialog::DoubleProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoubleProgressDialog)
{
    ui->setupUi(this);

    major = new ProgressBarControl(ui->majorProgressBar);
    minor = new ProgressBarControl(ui->minorProgressBar);

    connect(ui->cancelButton, &QPushButton::pressed, this, &DoubleProgressDialog::canceled);
}

DoubleProgressDialog::~DoubleProgressDialog()
{
    delete ui;
}

QString DoubleProgressDialog::getStatus() {
    return ui->progressStatus->text();
}

void DoubleProgressDialog::setStatus(QString msg)
{
    ui->progressStatus->setText(msg);
}

DoubleProgressDialog::ProgressBarControl::ProgressBarControl(QProgressBar *b)
{
    bar = b;
}

void DoubleProgressDialog::ProgressBarControl::setMin(float minimum)
{
    min = minimum;
    if(max < min) setMax(min);
    bar->setMinimum(convertUnits(min));
}

void DoubleProgressDialog::ProgressBarControl::setMax(float maximum)
{
    max = maximum;
    if(min > max) setMin(max);
    bar->setMaximum(convertUnits(max));
}

void DoubleProgressDialog::ProgressBarControl::setValue(float value)
{
    val = qBound(min, value, max);
    bar->setValue(convertUnits(val));
}

int DoubleProgressDialog::ProgressBarControl::getPrecision()
{
    return qLn(unitFactor) / qLn(10);
}

void DoubleProgressDialog::ProgressBarControl::setPrecision(int e)
{
    int oldFactor = unitFactor;
    unitFactor = qPow(10, e);

    min *= unitFactor / (float)oldFactor;
    max *= unitFactor / (float)oldFactor;
    val *= unitFactor / (float)oldFactor;
}

int DoubleProgressDialog::ProgressBarControl::convertUnits(float value)
{
    return qRound(value * unitFactor);
}
