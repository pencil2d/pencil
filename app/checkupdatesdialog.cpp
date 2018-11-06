#include "checkupdatesdialog.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QDesktopServices>

#include "util.h"


CheckUpdatesDialog::CheckUpdatesDialog()
{
    Qt::WindowFlags eFlags = Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint;
    setWindowFlags(eFlags);
    setMinimumSize(QSize(380, 120));

    //hLayout->setContentsMargins(20, 40, 40, 20);

    QLabel* logoLabel = new QLabel;
    logoLabel->setPixmap(QPixmap(":/icons/logo.png"));
    logoLabel->setFixedSize(QSize(72, 72));

    mTitleLabel = new QLabel;
    mTitleLabel->setText(tr("Checking for Updates...", "status description in the check-for-update dialog"));

    mDetailLabel = new QLabel;

    //If minimum and maximum both are set to 0, the bar shows a busy indicator instead of a percentage of steps. 
    mProgressBar = new QProgressBar;
    mProgressBar->setMaximum(0);
    mProgressBar->setMinimum(0);
    mProgressBar->setValue(0);

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
#ifdef NIGHTLY
    nightlyBuildCheck();
#else
    regularBuildCheck();
#endif
}

void CheckUpdatesDialog::regularBuildCheck()
{
    mNetworkManager = new QNetworkAccessManager(this);
    QUrl url("https://www.pencil2d.org/version/");

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
                          .arg("<a href=\"https://www.pencil2d.org/download/#nightlybuild\">").arg("</a>"));
    mDetailLabel->setOpenExternalLinks(true);
    mProgressBar->setRange(0, 1);
    mProgressBar->setValue(1);
    mProgressBar->setTextVisible(false);
    mDownloadButton->setEnabled(false);
}

void CheckUpdatesDialog::networkErrorHappened()
{
    mTitleLabel->setText(tr("<b>An error occurred while checking for updates</b>"));
    mDetailLabel->setText(tr("Please check your internet connection and try again later."));
    mProgressBar->setRange(0, 1);
    mProgressBar->setValue(1);
    mProgressBar->setTextVisible(false);
    mDownloadButton->setEnabled(false);
}

void CheckUpdatesDialog::networkRequestFinished(QNetworkReply* reply)
{
    OnScopeExit(reply->deleteLater());

    QNetworkReply::NetworkError errorCode = reply->error();
    if (errorCode != QNetworkReply::NoError)
    {
        // show error message
        qDebug() << reply->errorString();
        networkErrorHappened();
        return;
    }

    auto latestVersionString = QString::fromUtf8(reply->readAll()).trimmed();
    qDebug() << latestVersionString;

    bool isNewVersionAvailable = compareVersion(APP_VERSION, latestVersionString);
    if (isNewVersionAvailable)
    {
        mTitleLabel->setText(tr("<b>A new version of Pencil2D is available!</b>"));
        mDetailLabel->setText(tr("Pencil2D %1 is now available -- you have %2. Would you like to download it?")
                              .arg(latestVersionString)
                              .arg(APP_VERSION));
        mDownloadButton->setEnabled(true);
    }
    else
    {
        mTitleLabel->setText(tr("<b>Pencil2D is up to date</b>"));
        mDetailLabel->setText(tr("Version") + " " APP_VERSION);
        mProgressBar->setRange(0, 1);
        mProgressBar->setValue(1);
        mProgressBar->setTextVisible(false);
        mDownloadButton->setEnabled(false);
    }
}

bool CheckUpdatesDialog::compareVersion(QString currentVersion, QString latestVersion)
{
    QStringList currentVersionTokens = currentVersion.split('.');
    QStringList latestVersionTokens = latestVersion.split('.');

    if (currentVersionTokens.size() != 3 || latestVersionTokens.size() != 3)
    {
        // something went wrong
        return false;
    }

    // Example of versioning: 
    // 0.8.5 -> major: 0, minor: 8, patch: 5

    int majorVersionCurrent = currentVersionTokens.at(0).toInt();
    int majorVersionLatest = latestVersionTokens.at(0).toInt();
    if (majorVersionLatest > majorVersionCurrent)
    {
        return true; // yes, a new version is available
    }

    int minorVersionCurrent = currentVersionTokens.at(1).toInt();
    int minorVersionLatest = latestVersionTokens.at(1).toInt();
    if (minorVersionLatest > minorVersionCurrent)
    {
        return true;
    }

    int patchVersionCurrent = currentVersionTokens.at(2).toInt();
    int patchVersionLatest = latestVersionTokens.at(2).toInt();
    if (patchVersionLatest > patchVersionCurrent)
    {
        return true;
    }
    return false;
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
