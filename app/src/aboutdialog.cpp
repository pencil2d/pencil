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
#include "ui_aboutdialog.h"

#include <QPushButton>
#include <QSysInfo>
#include <QClipboard>

#include "pencildef.h"

AboutDialog::AboutDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::init()
{
	QStringList devText;
	devText << tr("Version: %1", "Version Number in About Dialog").arg(APP_VERSION);
#if defined(GIT_EXISTS) && defined(NIGHTLY_BUILD)
    devText << "commit: " S__GIT_COMMIT_HASH ;
	devText << "date: " S__GIT_TIMESTAMP ;
#endif
#if !defined(PENCIL2D_RELEASE)
    devText << "Development build";
#endif
	devText << QString("Operating System: %1").arg(QSysInfo::prettyProductName());
	devText << QString("CPU Architecture: %1").arg(QSysInfo::buildCpuArchitecture());
    ui->devInfoText->setText(devText.join("<br>"));

	QPushButton* copyToClipboardButton = new QPushButton(tr("Copy to clipboard", "Copy system info from About Dialog"));
	connect(copyToClipboardButton, &QPushButton::clicked, this, [devText] 
	{
		QApplication::clipboard()->setText(devText.join("\n"));
	});
    ui->buttonBox->addButton(copyToClipboardButton, QDialogButtonBox::ActionRole);
}
