/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "errordialog.h"
#include "ui_errordialog.h"

#include <QPushButton>
#include <QClipboard>

ErrorDialog::ErrorDialog( QString title, QString description, QString details, QWidget *parent ) :
    QDialog( parent ),
    ui(new Ui::ErrorDialog)
{
    ui->setupUi( this );

    setWindowTitle( title );
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

    QPushButton* copyToClipboard = new QPushButton(tr("Copy to Clipboard"));
    ui->buttonBox->addButton(copyToClipboard, QDialogButtonBox::ActionRole);

    connect(copyToClipboard, &QPushButton::clicked, this, &ErrorDialog::onCopyToClipboard);
}

ErrorDialog::~ErrorDialog()
{
    delete ui;
}

void ErrorDialog::onCopyToClipboard() {
    QGuiApplication::clipboard()->setText(ui->details->toPlainText());
}
