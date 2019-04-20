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
