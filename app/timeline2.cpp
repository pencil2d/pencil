#include "timeline2.h"
#include "ui_timeline2.h"

Timeline2::Timeline2(QWidget *parent) : BaseDockWidget(parent)
{
    ui = new Ui::Timeline2;
    ui->setupUi(this);
}

Timeline2::~Timeline2()
{
    delete ui;
}

void Timeline2::initUI()
{

}

void Timeline2::updateUI()
{

}
