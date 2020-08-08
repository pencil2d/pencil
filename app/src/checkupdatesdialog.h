/*

Pencil - Traditional Animation Software
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

#ifndef CHECKUPDATESDIALOG_H
#define CHECKUPDATESDIALOG_H

#include <QDialog>

class QNetworkAccessManager;
class QNetworkReply;
class QPushButton;
class QLabel;
class QProgressBar;


class CheckUpdatesDialog : public QDialog
{
    Q_OBJECT

public:
    CheckUpdatesDialog();
    ~CheckUpdatesDialog();

    void startChecking();

private:
    void regularBuildCheck();
    void nightlyBuildCheck();
    void networkErrorHappened();
    void networkResponseIsEmpty();
    void invalidReleaseXml();

    void networkRequestFinished(QNetworkReply* reply);
    bool compareVersion(QString currentVersion, QString latestVersion);
    QString getVersionNumberFromXml(QString xml);

    void gotoDownloadPage();
    void closeDialog();

    QNetworkAccessManager* mNetworkManager = nullptr;
    QProgressBar* mProgressBar = nullptr;
    QLabel* mTitleLabel = nullptr;
    QLabel* mDetailLabel = nullptr;
    QPushButton* mDownloadButton = nullptr;
    QPushButton* mCloseButton = nullptr;
};

#endif // CHECKUPDATESDIALOG_H
