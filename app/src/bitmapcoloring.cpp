/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "bitmapcoloring.h"
#include "ui_bitmapcoloringwidget.h"


BitmapColoring::BitmapColoring(Editor* editor, QWidget *parent) :
    BaseDockWidget(parent)

{
    QWidget* innerWidget = new QWidget;
    setWindowTitle(tr("Bitmap Coloring"));

    ui = new Ui::BitmapColoringWidget;
    ui->setupUi(innerWidget);
    setWidget(innerWidget);
    mEditor = editor;
    connect(ui->btn1Select, &QPushButton::clicked, mEditor, &Editor::copyFromScan);
    connect(ui->btn1Next, &QPushButton::clicked, mEditor, &Editor::scrubNextKeyFrame);
}

BitmapColoring::~BitmapColoring()
{
    delete ui;
}

void BitmapColoring::initUI()
{

}

void BitmapColoring::updateUI()
{

}
