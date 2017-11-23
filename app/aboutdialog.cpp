/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

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
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
}

void AboutDialog::init()
{
    QLabel* logo = new QLabel(this);
    QLabel* logoText = new QLabel(this);
    QLabel* aboutText = new QLabel(this);

    logo->setPixmap(QPixmap(":icons/logo.png"));
    logo->setAlignment(Qt::AlignCenter);
    logoText->setAlignment(Qt::AlignCenter);
    logoText->setText(tr("<b>Pencil2D</b>"));
    aboutText->setTextInteractionFlags(Qt::TextBrowserInteraction);
    aboutText->setWordWrap(true);
    aboutText->setOpenExternalLinks(true);
    aboutText->setText(tr("Official site: <a href=\"http://pencil2d.github.io\">pencil2d.github.io</a>"
                          "<br>Developed by: <b>Pascal Naidon, Patrick Corrieri, Matt Chang</b>"
                          "<br>Thanks to: Qt Framework <a href=\"https://www.qt.io/\">https://www.qt.io/</a>"
                          "<br>Distributed under the <a href=\"http://www.gnu.org/licenses/gpl-2.0.html\">GNU General Public License, version 2</a>"));
    QVBoxLayout* lay = new QVBoxLayout;
    lay->addWidget(logo);
    lay->addWidget(logoText);
    lay->addWidget(aboutText);

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define S__GIT_TIMESTAMP__ TOSTRING(GIT_TIMESTAMP)
#define S__GIT_COMMIT_HASH__ TOSTRING(GIT_CURRENT_SHA1)


    QLabel* devInfoText = new QLabel(this);
    devInfoText->setStyleSheet("QLabel { background-color: #ffffff;"
                                        "border-style: solid; border-width: 1px; border-color: gray;}");
    devInfoText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    devInfoText->setFixedHeight(60);
    devInfoText->setAlignment(Qt::AlignCenter);
    QString devText = QString("Version: %1").arg(APP_VERSION);
#ifdef GIT_EXISTS
    devText += "<br>commit: " S__GIT_COMMIT_HASH__ "<br> date: " S__GIT_TIMESTAMP__ "<br>";
#endif
    devInfoText->setText(devText);
    lay->addWidget(devInfoText);


    QPushButton* okButton = new QPushButton(tr("OK"));
    connect(okButton, &QAbstractButton::clicked, this, &QDialog::accept);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(okButton);

    lay->addLayout(buttonsLayout);
    setLayout(lay);
}
