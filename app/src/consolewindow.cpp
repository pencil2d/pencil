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
        printLook("");
        print(tr("Hint: If you are unsure of what to do, type HELP below and hit enter."));

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

    if (command == tr("help"))
    {
        printHelp();
    }
    else if (command == tr("look"))
    {

    }
    else if (command.startsWith(tr("look ")))
    {
        printLook(command.mid(5));
    }
    else if (command == tr("close") || command == tr("quit"))
    {
        // Prevents sound glitching
        mSpeaker->pause();

        // Quit application
        close();
    }
}

void ConsoleWindow::print(QString s)
{
    ui->console->appendPlainText(s);
}

void ConsoleWindow::printHelp()
{
    print("There's no shame in asking for help! In case you are not familiar with the typical format of text-based games, here is a very brief overview. You type actions into the bar at the bottom and when you hit enter the result of those actions will be displayed here. Usually actions use a simple VERB NOUN form. For example, LOOK MONEY. Capitalization is used throughout to help you find important nouns and verbs to use. LOOK is the most important verb as it helps you understand what objects are of interest and what you can do with them. Use LOOK by itself to generally look around for things.");
}

void ConsoleWindow::printLook(QString arg)
{
    arg = arg.trimmed();
    if (arg.isEmpty())
    {
        QString baseMessage = tr("Ye find yeself in yon animation studio. The room is small and dark with no obvious exits. The only source of illumination is a LAMP on a DRAWING TABLE in the center of the room. PAPER and various TOOLS are neatly organized on top of the table. A WASTEBIN is beside the table. %1");
        if (mSpeaker->state() == QMediaPlayer::PlayingState)
        {
            print(baseMessage.arg(tr("You can hear music playing from a SPEAKER behind you.")));
        }
        else
        {
            print(baseMessage.arg(tr("A SPEAKER lays silent behind you.")));
        }
    }
    else if (arg == tr("corner") || arg == tr("corners"))
    {
        print(tr("There are no corners, you are in a round room. I didn't mention this earlier because there was no point."));
    }
    else if (arg == tr("money"))
    {
        print(tr("That was just an example! You don't actually have any money, and it doesn't do you any good anyway without something to buy."));
    }
    else if ((QStringList() << tr("self") << tr("me") << tr("i") << tr("myself")).contains(arg))
    {
        print(tr("You look down at your body. You've got hands and feet and all the usually stuff in between. There's no mirror in the room to see what your head looks like, but you're probably quite good looking."));
    }
    else if (arg == tr("lamp"))
    {
        if(!mHasLookedLamp)
        {
            print(tr("It's just an ordinary lamp."));
            mHasLookedLamp = true;
        }
        else
        {
            print(tr("It's still just an ordinary lamp, but you swear it has moved since last time you've looked at it."));
        }
    }
    else if (arg == tr("speaker"))
    {
        if (mSpeaker->state() == QMediaPlayer::PlayingState)
        {
            print(tr("This small speaker is playing some chill 8-bit tunes. There is a small plaque with the words \"Electric City by Jazzcat, 8-bit Remix by Dippy\" engraved on it. It is plugged into the wall, so you should be able to turn it off by UNPLUGging it."));
        }
        else
        {
            print(tr("This small speaker was playing some music until you UNPLUGged it. Now it's so silent. Its sole purpose in life was to play music, and now you have deprived it of that. I hope your happy with yourself. If you want to make things right you better PLUG IN it again."));
        }
    }
    else if (arg == tr("tools") || arg == tr("items"))
    {
        // TODO
    }
    else if ((QStringList() << tr("desk") << tr("table") << tr("drawing desk") << tr("drawing table")).contains(arg))
    {
        // TODO
    }
    else if ((QStringList() << tr("wastebin") << tr("garbage bin") << tr("garbage") << tr("trash bin") << tr("trash")).contains(arg))
    {
        QString baseMessage = tr("There is a wastebin to the side of the desk. You could probably use it to DISCARD the current PAPER if you wanted. %1");
        if (mDiscardedPaper == 0)
        {
            print(baseMessage.arg(tr("It is currently empty.")));
        }
        else if (mDiscardedPaper <= 10)
        {
            print(baseMessage.arg(tr("There are already a few papers in it.")));
        }
        else if (mDiscardedPaper <= 25)
        {
            print(tr("You've DISCARDed an aweful lot of PAPERs into this wastebin. It is in danger of overflowing."));
        }
        else
        {
            print(tr("You've DISCARDed so many PAPERs into this wastebin that it is now overflowing onto the floor. What a mess!"));
        }
    }
    else
    {
        print(tr("I don't know where to look for that."));
    }
}
