/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <QtWidgets>
#include <QSpinBox>
#include <QGridLayout>
#include <QGroupBox>

#include "aboutdialog.h"


AboutDialog::AboutDialog(QWidget* parent ) : QDialog(parent)
{
    setWindowTitle(tr("About"));
    setFixedSize(450,450);
}

void AboutDialog::init()
{
    QVBoxLayout* lay = new QVBoxLayout;
    QPushButton *okButton = new QPushButton(tr("OK"));

    QHBoxLayout* buttonsLayout = new QHBoxLayout;

    devInfoText = new QLabel(this);
    aboutText = new QLabel(this);
    logo = new QLabel(this);
    logoText = new QLabel(this);

    logo->setPixmap(QPixmap(":icons/logo.png"));
    logo->setAlignment(Qt::AlignCenter);
    logoText->setAlignment(Qt::AlignCenter);

    logoText->setText(tr("<b>Pencil2D</b>"));
    aboutText->setTextInteractionFlags(Qt::TextBrowserInteraction); //FIXME: links clickable but doesn't open browser.
    aboutText->setWordWrap(true);
    aboutText->setText(tr("Official site: <a href=\"http://pencil2d.org\">pencil2d.org</a>"
                                                        "<br>Developed by: <b>Pascal Naidon, Patrick Corrieri, Matt Chang, Cirus</b></>"
                                                        "<br>Thanks to: Qt Framework <a href=\"http://qt-project.org\">qt-project.org</a></>"
                                                        "<br>Distributed under the <a href=\"http://www.gnu.org/copyleft/gpl.html\">GPL License</a></>"));

    connect(okButton, &QAbstractButton::clicked, this, &QDialog::accept);

    buttonsLayout->addWidget(okButton);

    lay->addWidget(logo);
    lay->addWidget(logoText);
    lay->addWidget(aboutText);

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define S__GIT_TIMESTAMP__ TOSTRING(GIT_TIMESTAMP)
#define S__GIT_COMMIT_HASH__ TOSTRING(GIT_CURRENT_SHA1)

#ifdef GIT_EXISTS
    devInfoText->setStyleSheet("QLabel { background-color: #ffffff;"
                                        "border-style: solid; border-width: 1px; border-color: gray;}");
    devInfoText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    devInfoText->setFixedHeight(60);
    devInfoText->setAlignment(Qt::AlignCenter);
    devInfoText->setText(tr("commit: " S__GIT_COMMIT_HASH__ /*"<br> version: " APP_VERSION*/ "<br> date: " S__GIT_TIMESTAMP__)) ;
    lay->addWidget(devInfoText);
#endif


    lay->addLayout(buttonsLayout);

    setLayout(lay);
}
