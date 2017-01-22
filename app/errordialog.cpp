#include "errordialog.h"
#include "ui_errordialog.h"

ErrorDialog::ErrorDialog( QString title, QString description, QString details, QWidget *parent ) :
    QDialog( parent ),
    ui(new Ui::ErrorDialog)
{
    ui->setupUi( this );
    ui->horizontalLayout_2->setSpacing(0);

    this->setWindowTitle( title );
    ui->title->setText( QString( "<h3>%1</h3>" ).arg( title ) );
    ui->description->setText( description );
    if ( details.isEmpty() )
    {
        ui->details->setVisible( false );
    }
    else
    {
        ui->details->setText( QString( "<pre>%1</pre>" ).arg( details ) );
    }
}

ErrorDialog::~ErrorDialog()
{
    delete ui;
}
