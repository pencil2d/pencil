#include "consolewindow.h"
#include "ui_consolewindow.h"

#include <QRegularExpression>
#include <QMediaPlaylist>
#include <QMediaPlayer>

#include "mainwindow2.h"

ConsoleWindow::ConsoleWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConsoleWindow)
{
    ui->setupUi(this);

    // Actions
    connect(ui->prompt, &QLineEdit::returnPressed, this, &ConsoleWindow::runCommand);
    ui->prompt->installEventFilter(this);

    // Styling
    ui->prompt->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->prompt->setFocus();

    mMainWindow = new MainWindow2(this);

    // Play music
    QMediaPlaylist *playlist = new QMediaPlaylist();
    playlist->addMedia(QUrl("qrc:/audio/electric-city.wav"));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    mSpeaker = new QMediaPlayer();
    mSpeaker->setPlaylist(playlist);
    mSpeaker->play();
}

ConsoleWindow::~ConsoleWindow()
{
    delete ui;
}

bool ConsoleWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->prompt)
    {
        if (event->type() == QEvent::FocusOut)
        {
            // Keep focus on prompt
            ui->prompt->setFocus();
        }
    }
    return false;
}

void ConsoleWindow::runCommand()
{
    // Handle exiting the splash screen
    if (mIsOnSplash)
    {
        // Remove title
        ui->title->hide();
        // Remove logo
        ui->console->clear();

        // Clear prompt
        ui->prompt->setText("");
        // Make prompt editable
        ui->prompt->setReadOnly(false);

        // Show help text
        // TODO

        // Prevent this code from being run again
        mIsOnSplash = false;
        return;
    }

    // Get command
    QString command = ui->prompt->text().trimmed();
    // Clean command for robustness
    command = command.toLower().replace(QRegularExpression("[^A-Za-z 0-9\\.]"), "");
    // Echo to output
    ui->console->appendPlainText("> " + command);
    // Clear prompt
    ui->prompt->setText("");

    // Take action!

    // TODO
}
