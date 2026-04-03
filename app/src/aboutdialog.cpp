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

    QString version(APP_VERSION);
    if (version == "0.0.0.0") {
        devText << "Development Build ";
        devText << "Build date: " __DATE__;
    } else if (version.startsWith("99.0.0")) {
        devText << QString("Nightly Build (%1)").arg(APP_VERSION);
        devText << "Build date: " __DATE__;
    } else {
        devText << tr("Version: %1", "Version Number in About Dialog").arg(APP_VERSION);
    }
    devText << ""; // An empty line

#if defined(GIT_EXISTS)
    devText << "commit: " S__GIT_COMMIT_HASH
            << "date: " S__GIT_TIMESTAMP
            << "";
#endif

    devText << QString("Operating System: %1").arg(QSysInfo::prettyProductName())
            << QString("CPU Architecture: %1").arg(QSysInfo::buildCpuArchitecture());
    if(QString(qVersion()) == QT_VERSION_STR)
    {
        devText << QString("Qt Version: %1").arg(QT_VERSION_STR);
    }
    else
    {
        devText << QString("Compile-Time Qt Version: %1").arg(QT_VERSION_STR)
                << QString("Runtime Qt Version: %1").arg(qVersion());
    }
    ui->devInfoText->setText(devText.join("<br>"));

    QPushButton* copyToClipboardButton = new QPushButton(tr("Copy to clipboard", "Copy system info from About Dialog"));
    connect(copyToClipboardButton, &QPushButton::clicked, this, [devText]
    {
        QApplication::clipboard()->setText(devText.join("\n"));
    });
    ui->buttonBox->addButton(copyToClipboardButton, QDialogButtonBox::ActionRole);
}
