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

#include "aboutdialog.h"

#include <QtWidgets>
#include <QSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QSysInfo>
#include <QApplication>
#include <QClipboard>

AboutDialog::AboutDialog(QWidget* parent ) : QDialog(parent)
{
    setWindowTitle(tr("About", "About Dialog Window Title"));
    setFixedSize(450, 450);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
}

void AboutDialog::init()
{
    QLabel* logo = new QLabel(this);
    logo->setFont(QFont("Helvetica", 10));
    logo->setPixmap(QPixmap(":icons/logo.png"));
    logo->setAlignment(Qt::AlignCenter);

    QLabel* logoText = new QLabel(this);
    logoText->setFont(QFont("Helvetica", 10));
    logoText->setAlignment(Qt::AlignCenter);
    logoText->setText("<b>Pencil2D</b>");

    QLabel* aboutText = new QLabel(this);
    aboutText->setFont(QFont("Helvetica", 10));
    aboutText->setTextInteractionFlags(Qt::TextBrowserInteraction);
    aboutText->setWordWrap(true);
    aboutText->setOpenExternalLinks(true);
    QString sText;
    sText += tr("Official site: <a href=\"http://pencil2d.github.io\">pencil2d.github.io</a>");
    sText += tr("<br>Developed by: <b>Pascal Naidon, Patrick Corrieri, Matt Chang</b>");
    sText += tr("<br>Thanks to Qt Framework <a href=\"https://www.qt.io/download\">https://www.qt.io/</a>");
    sText += tr("<br>miniz: <a href=\"https://github.com/richgel999/miniz\">https://github.com/richgel999/miniz</a>");
    sText += tr("<br>Distributed under the <a href=\"http://www.gnu.org/licenses/gpl-2.0.html\">GNU General Public License, version 2</a>");
    aboutText->setText(sText);

    QVBoxLayout* lay = new QVBoxLayout;
    lay->addWidget(logo);
    lay->addWidget(logoText);
    lay->addWidget(aboutText);

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define S__GIT_TIMESTAMP__ TOSTRING(GIT_TIMESTAMP)
#define S__GIT_COMMIT_HASH__ TOSTRING(GIT_CURRENT_SHA1)

    QLabel* devInfoText = new QLabel(this);
    devInfoText->setFont(QFont("Helvetica", 10));
    devInfoText->setStyleSheet("QLabel { background-color: #ffffff;"
                                        "border-style: solid; border-width: 1px; border-color: gray;}");
    devInfoText->setTextInteractionFlags(Qt::TextSelectableByMouse);

    devInfoText->setAlignment(Qt::AlignCenter);
	QStringList devText;
	devText << tr("Version: %1", "Version Number in About Dialog").arg(APP_VERSION);
#if defined(GIT_EXISTS) && defined(NIGHTLY_BUILD)
    devText << "commit: " S__GIT_COMMIT_HASH__ ;
	devText << "date: " S__GIT_TIMESTAMP__ ;
#endif
#if !defined(PENCIL2D_RELEASE)
    devText << "Development build";
#endif
	devText << QString("Operating System: %1").arg(QSysInfo::prettyProductName());
	devText << QString("CPU Architecture: %1").arg(QSysInfo::buildCpuArchitecture());
    devInfoText->setText(devText.join("<br>"));
    lay->addWidget(devInfoText);

	QPushButton* copyToClipboardButton = new QPushButton(tr("Copy to clipboard", "Copy system info from About Dialog"));
	connect(copyToClipboardButton, &QPushButton::clicked, this, [devText] 
	{
		QApplication::clipboard()->setText(devText.join("\n"));
	});

    QPushButton* okButton = new QPushButton(tr("OK"));
    connect(okButton, &QAbstractButton::clicked, this, &QDialog::accept);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(copyToClipboardButton);
    buttonsLayout->addWidget(okButton);

    lay->addLayout(buttonsLayout);
    setLayout(lay);
}
