﻿/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "checkupdatesdialog.h"
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QDesktopServices>
#include <QRegularExpression>
#include <QVersionNumber>
#include <QXmlStreamReader>

#include "util.h"


CheckUpdatesDialog::CheckUpdatesDialog()
{
    Qt::WindowFlags eFlags = Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint;
    setWindowFlags(eFlags);
    setMinimumSize(QSize(400, 150));

    QLabel* logoLabel = new QLabel;
    logoLabel->setPixmap(QPixmap(":/icons/logo.png"));
    logoLabel->setFixedSize(QSize(72, 72));

    mTitleLabel = new QLabel;
    mTitleLabel->setText(tr("Checking for Updates...", "status description in the check-for-update dialog"));

    mDetailLabel = new QLabel;
    mDetailLabel->setWordWrap(true);

    //If minimum and maximum both are set to 0, the bar shows a busy indicator instead of a percentage of steps.
    mProgressBar = new QProgressBar;
    mProgressBar->setMaximum(0);
    mProgressBar->setMinimum(0);
    mProgressBar->setValue(0);
    mProgressBar->setTextVisible(false);

    mDownloadButton = new QPushButton(tr("Download"));
    mCloseButton = new QPushButton(tr("Close"));

    QHBoxLayout* hButtonLayout = new QHBoxLayout;
    hButtonLayout->addWidget(mDownloadButton);
    hButtonLayout->addWidget(mCloseButton);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(mTitleLabel);
    vLayout->addWidget(mDetailLabel);
    vLayout->addWidget(mProgressBar);
    vLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    vLayout->addLayout(hButtonLayout);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(logoLabel);
    mainLayout->addLayout(vLayout);
    setLayout(mainLayout);

    mDownloadButton->setDisabled(true);

    connect(mDownloadButton, &QPushButton::clicked, this, &CheckUpdatesDialog::gotoDownloadPage);
    connect(mCloseButton, &QPushButton::clicked, this, &CheckUpdatesDialog::closeDialog);
}

CheckUpdatesDialog::~CheckUpdatesDialog()
{
    mNetworkManager->deleteLater();
}

void CheckUpdatesDialog::startChecking()
{
#ifdef PENCIL2D_NIGHTLY_BUILD
    nightlyBuildCheck();
#else
    regularBuildCheck();
#endif
}

void CheckUpdatesDialog::regularBuildCheck()
{
    mNetworkManager = new QNetworkAccessManager(this);
    QUrl url("https://github.com/pencil2d/pencil/releases.atom");

    QNetworkRequest req;
    req.setUrl(url);
    req.setRawHeader("User-Agent", "Mozilla Firefox");

    mNetworkManager->get(req);
    connect(mNetworkManager, &QNetworkAccessManager::finished, this, &CheckUpdatesDialog::networkRequestFinished);
}

void CheckUpdatesDialog::nightlyBuildCheck()
{
    mTitleLabel->setText(tr("<b>You are using a Pencil2D nightly build</b>"));
    mDetailLabel->setText(tr("Please go %1 here %2 to check new nightly builds.")
                          .arg("<a href=\"https://www.pencil2d.org/download/#nightlybuild\">", "</a>"));
    mDetailLabel->setOpenExternalLinks(true);
    mProgressBar->setRange(0, 1);
    mProgressBar->setValue(1);
    mDownloadButton->setEnabled(false);
}

void CheckUpdatesDialog::networkErrorHappened()
{
    mTitleLabel->setText(tr("<b>An error occurred while checking for updates</b>", "error msg of check-for-update"));
    mDetailLabel->setText(tr("Please check your internet connection and try again later.", "error msg of check-for-update"));
    mProgressBar->setRange(0, 1);
    mProgressBar->setValue(1);
    mDownloadButton->setEnabled(false);
}

void CheckUpdatesDialog::networkResponseIsEmpty()
{
    mTitleLabel->setText(tr("<b>An error occurred while checking for updates</b>", "error msg of check-for-update"));
    mDetailLabel->setText(tr("Network response is empty", "error msg of check-for-update"));
    mProgressBar->setRange(0, 1);
    mProgressBar->setValue(1);
    mDownloadButton->setEnabled(false);
}

void CheckUpdatesDialog::invalidReleaseXml()
{
    mTitleLabel->setText(tr("<b>An error occurred while checking for updates</b>", "error msg of check-for-update"));
    mDetailLabel->setText(tr("Couldn't retrieve the version information", "error msg of check-for-update"));
    mProgressBar->setRange(0, 1);
    mProgressBar->setValue(1);
    mDownloadButton->setEnabled(false);
}

void CheckUpdatesDialog::networkRequestFinished(QNetworkReply* reply)
{
    reply->deleteLater();

    QNetworkReply::NetworkError errorCode = reply->error();
    if (errorCode != QNetworkReply::NoError)
    {
        // show error message
        qDebug() << reply->errorString();
        networkErrorHappened();
        return;
    }

    auto releasesAtom = QString::fromUtf8(reply->readAll()).trimmed();
    if (releasesAtom.isEmpty())
    {
        networkResponseIsEmpty();
        return;
    }

    QString latestVersionString = getVersionNumberFromXml(releasesAtom);
    if (latestVersionString == "0.0.1")
    {
        invalidReleaseXml();
        return;
    }

    bool isNewVersionAvailable = compareVersion(APP_VERSION, latestVersionString);
    if (isNewVersionAvailable)
    {
        mTitleLabel->setText(tr("<b>A new version of Pencil2D is available!</b>"));
        mDetailLabel->setText(tr("Pencil2D %1 is now available -- you have %2. Would you like to download it?")
                              .arg(latestVersionString, APP_VERSION));
        mProgressBar->hide();
        mDownloadButton->setEnabled(true);
    }
    else
    {
        mTitleLabel->setText(tr("<b>Pencil2D is up to date</b>"));
        mDetailLabel->setText(tr("Version %1").arg(APP_VERSION));
        mProgressBar->setRange(0, 1);
        mProgressBar->setValue(1);
        mDownloadButton->setEnabled(false);
    }
}

bool CheckUpdatesDialog::compareVersion(QString currentVersion, QString latestVersion)
{
    return QVersionNumber::fromString(currentVersion) < QVersionNumber::fromString(latestVersion);
}

QString CheckUpdatesDialog::getVersionNumberFromXml(QString xml)
{
    // XML source: https://github.com/pencil2d/pencil/releases.atom

    QXmlStreamReader xmlReader(xml);

    while (!xmlReader.atEnd() && !xmlReader.hasError())
    {
        QXmlStreamReader::TokenType tokenType = xmlReader.readNext();
        if (tokenType == QXmlStreamReader::StartElement && xmlReader.name() == QLatin1String("entry"))
        {
            while (!xmlReader.atEnd() && !xmlReader.hasError())
            {
                xmlReader.readNext();
                if (xmlReader.name() == QLatin1String("title"))
                {
                    QString titleTag = xmlReader.readElementText();
                    return titleTag.remove(QRegularExpression("^v")); // remove the leading 'v'
                }
            }
        }
    }
    if (xmlReader.error() != QXmlStreamReader::NoError)
    {
        qDebug() << xmlReader.errorString();
        return "0.0.1";
    }
    return "0.0.1";
}

void CheckUpdatesDialog::gotoDownloadPage()
{
    QString url = "https://pencil2d.org/download";
    QDesktopServices::openUrl(QUrl(url));
}

void CheckUpdatesDialog::closeDialog()
{
    done(QDialog::Accepted);
}
