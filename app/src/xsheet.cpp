#include "xsheet.h"
#include "ui_xsheet.h"

Xsheet::Xsheet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Xsheet)
{
    ui->setupUi(this);
}

Xsheet::~Xsheet()
{
    delete ui;
}
