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

    devInfoText->setStyleSheet("QLabel { background-color: #ffffff;"
                                        "border-style: solid; border-width: 1px; border-color: gray;}");
    devInfoText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    devInfoText->setFixedHeight(60);
    devInfoText->setAlignment(Qt::AlignCenter);
    devInfoText->setText(tr("commit: " GIT_CURRENT_SHA1 /*"<br> version: " APP_VERSION*/ "<br> date: " GIT_TIMESTAMP));

    connect(okButton, &QAbstractButton::clicked, this, &QDialog::accept);

    buttonsLayout->addWidget(okButton);

    lay->addWidget(logo);
    lay->addWidget(logoText);
    lay->addWidget(aboutText);
    lay->addWidget(devInfoText);

    lay->addLayout(buttonsLayout);

    setLayout(lay);
}
