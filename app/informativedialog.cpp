#include "informativedialog.h"
#include "ui_informativedialog.h"

InformativeDialog::InformativeDialog( QString title, QString description, QWidget *parent ) :
    QDialog( parent ),
    ui(new Ui::InformativeDialog)
{
    ui->setupUi( this );
    ui->horizontalLayout->setSpacing(0);

    this->setWindowTitle( title );
    ui->title->setText( QString( "<h3>%1</h3>" ).arg( title ) );
    ui->description->setText( description );
}

InformativeDialog::~InformativeDialog()
{
    delete ui;
}
