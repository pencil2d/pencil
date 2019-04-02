#include "consolewindow.h"
#include "ui_consolewindow.h"

#include <QRegularExpression>
#include <QMediaPlaylist>
#include <QMediaPlayer>

#include "mainwindow2.h"
#include "ui_mainwindow2.h"
#include "editor.h"
#include "toolmanager.h"
#include "pointerevent.h"
#include "viewmanager.h"
#include "playbackmanager.h"
#include "colormanager.h"
#include "layermanager.h"
#include "layercamera.h"
#include "layerbitmap.h"
#include "asciiimage.h"
#include "asciipreviewdialog.h"

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
    mMainWindow->mEditor->color()->setColor(Qt::black);
    LayerManager *lm = mMainWindow->mEditor->layers();
    mCamLayer = lm->createCameraLayer("ASCII Camera");
    mCamLayer->setViewRect(QRect(mMainWindow->mEditor->view()->mapScreenToCanvas(QPoint(0, 0)).toPoint(), QSize(50, 50)));
    LayerCamera *curCam;
    while ((curCam = static_cast<LayerCamera*>(lm->getLastCameraLayer())) != mCamLayer)
    {
        lm->deleteLayer(lm->getIndex(curCam));
    }
    mDrawingLayer = lm->createBitmapLayer("ASCII Bitmap");
    lm->setCurrentLayer(mDrawingLayer);

    mPreviewDialog = new AsciiPreviewDialog(this);
    connect(mMainWindow->mEditor, &Editor::currentFrameChanged, this, &ConsoleWindow::frameChanged);

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
    command = command.toLower().replace(QRegularExpression("[^A-Za-z 0-9\\.\\-]"), "");
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
        printLook("");
    }
    else if (command.startsWith(tr("look ")))
    {
        printLook(command.mid(tr("look ").size()));
    }
    else if (command == tr("close") || command == tr("quit"))
    {
        // Prevents sound glitching
        mSpeaker->pause();

        // Quit application
        close();
    }
    else if ((QStringList() << tr("equip") << tr("pick") << tr("pickup") << tr("pick up") << tr("grab")).contains(command))
    {
        print(tr("What do you want to %1? To specify, use the action: %2 <object>").arg(command, command.toUpper()));
    }
    else if (command.startsWith(tr("equip ")) || command.startsWith(tr("pick ")) || command.startsWith(tr("pickup ")) || command.startsWith(tr("grab ")))
    {
        if (command.startsWith(tr("pick up ")))
        {
            printEquip(tr("pick up"), command.mid(tr("pick up ").size()).split(' '));
        }
        else
        {
            printEquip(command.left(command.indexOf(' ')), command.mid(command.indexOf(' ') + 1).split(' '));
        }
    }
    else if (command == "version")
    {
        print("Pencil2D ASCII Edition v0.1");
    }
    else if (command == tr("press"))
    {
        print(tr("Press needs x and y coordinates of where to start pressing. Use PRESS <x> <y>"));
    }
    else if (command.startsWith(tr("press ")))
    {
        doPress(command.mid(tr("press ").size()).split(' '));
    }
    else if (command == tr("move") || command == tr("go to"))
    {
        print(tr("Move needs x and y coordinates of where to move to. Use MOVE <x> <y>"));
    }
    else if (command.startsWith(tr("move ")))
    {
        doMove(command.mid(tr("move ").size()).split(' '));
    }
    else if (command.startsWith(tr("go to ")))
    {
        doMove(command.mid(tr("go to ").size()).split(' '));
    }
    else if (command == tr("release")) {
        doRelease();
    }
    else if (command == tr("unplug"))
    {
        print("What do you want to unplug? To specify, use the action: UNPLUG <object>");
    }
    else if (command.startsWith(tr("unplug ")))
    {
        if (command == tr("unplug speaker"))
        {
            doPlugSpeaker(false);
        }
        else
        {
            print(tr("Cannot unplug %1").arg(command.mid(tr("unplug ").size())));
        }
    }
    else if (command == tr("plug in"))
    {
        print("What do you want to plug in? To specify, use the action: PLUG IN <object>");
    }
    else if (command.startsWith(tr("plug in ")))
    {
        if (command == tr("plug in speaker"))
        {
            doPlugSpeaker(true);
        }
        else
        {
            print(tr("Cannot plug in %1").arg(command.mid(tr("plug in ").size())));
        }
    }
    else if (command == "iddqd")
    {
        // A debug command to display the main window
        if (mMainWindow->isVisible())
        {
            mMainWindow->hide();
        }
        else
        {
            mMainWindow->show();
        }
    }
    else if (command == tr("view") || command == tr("render"))
    {
        printPaper(RENDER_SIZE);
    }
    else if (command.startsWith("view ") || command.startsWith(tr("render ")))
    {
        printPaper(command.mid(command.indexOf(' ') + 1).split(" "));
    }
    else if (command.startsWith(tr("page")))
    {
        doPage(command.mid(tr("page").size()).trimmed());
    }
    else if (command == tr("flip") || command == tr("play"))
    {
        doPlay();
    }
    else if (command == tr("stop") || command == tr("pause"))
    {
        doStop();
    }
    else if (command == tr("open"))
    {
        doOpen();
    }
    else if (command == tr("save"))
    {
        doSave();
    }
    else
    {
        print(tr("I do not understand that."));
    }
}

void ConsoleWindow::frameChanged(int index)
{
    Q_UNUSED(index);
    if (!mPreviewDialog->isVisible()) return;
    printPaper();
}

void ConsoleWindow::frameUpdate()
{
    if (!mPreviewDialog->isVisible()) return;
    printPaper();
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
        QString baseMessage = tr("Ye find yeself in yon animation studio. The room is small and dark with no obvious exits. The only source of illumination is a LAMP on a DRAWING TABLE in the center of the room. PAPER and various TOOLS are neatly organized on top of the table. A WASTEBIN and a BOOK are beside the table. %1");
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
        print(tr("There are various drawing implements lined up around the edge of the desk. You can see PENCILs, ERASERs, PENs, and BRUSHes. You could probably PICK UP one of them and use it. For example you could PICK UP PEN 2 for the 2 sized pen."));
    }
    else if ((QStringList() << tr("desk") << tr("table") << tr("drawing desk") << tr("drawing table")).contains(arg))
    {
        print(tr("There is a large desk in the center of the room. On it are various objects including PAPER and TOOLS. To the left is a WASTEBIN and to the right a BOOK."));
    }
    else if (arg == tr("paper") || arg == tr("pages"))
    {
        print(tr("There is a seemingly infinte number of papers on the desk. You could probably use some of the TOOLS to draw on them. You also have the urge to FLIP through them. If you want to take a closer look at the top paper, you can VIEW it."));
    }
    else if (arg == tr("book") || arg == tr("spellbook"))
    {
        print(tr("You pick up the book an skim through it. It appears to be a spellbook written in some mystic language. The only words you even recognize are OPEN and SAVE. Perhaps they are some kind of incantations you can use."));
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

void ConsoleWindow::printEquip(QString term, QStringList args)
{
    if (args.size() > 2)
    {
        print(tr("Too many words, %1 only supports <object> and <size> (in that order).").arg(term));
    }

    // List of tools supported by Pencil2D ASCII Version
    QHash<QString,ToolType> allowableTools;
    allowableTools["pencil"] = ToolType::PENCIL;
    allowableTools["eraser"] = ToolType::ERASER;
    allowableTools["pen"] = ToolType::PEN;
    allowableTools["brush"] = ToolType::BRUSH;

    if (allowableTools.contains(args[0]))
    {
        if (args.size() == 2)
        {
            bool ok = true;
            float s = args[1].toFloat(&ok);
            if (!ok)
            {
                print(tr("%1 is not a valid size, ignoring. Please use a number next time").arg(args[1]));
            }
            else
            {
                mMainWindow->mEditor->tools()->getTool(allowableTools[args[0]])->setWidth(s);
            }
        }
        mMainWindow->mEditor->tools()->setCurrentTool(allowableTools[args[0]]);
        print(tr("You %1 the %2 with size %3. To use it, you have to PRESS <x> <y>, MOVE <x> <y> zero or more times, and then RELEASE. It is also helpful to VIEW what you are doing.").arg(term, args[0], QString::number(mMainWindow->mEditor->tools()->getTool(allowableTools[args[0]])->properties.width)));
    }
    else
    {
        print(tr("You can't %1 the %2.").arg(term, args[0]));
    }
}

void ConsoleWindow::printPaper(QSize renderSize)
{
    if (renderSize.isEmpty())
    {
        renderSize = mCurrentRenderSize;
    }
    mCurrentRenderSize = renderSize;

    // Render image

    QSize cameraSize = mCamLayer->getViewSize();
    int currentFrame = mMainWindow->mEditor->currentFrame();
    QImage imageToExport(renderSize, QImage::Format_ARGB32_Premultiplied);

    QColor bgColor = Qt::white;
    bgColor.setAlpha(0);
    imageToExport.fill(bgColor);

    QPainter painter(&imageToExport);
    painter.setWorldTransform(mCamLayer->getViewAtFrame(currentFrame));
    painter.setWindow(QRect(QPoint(0, 0), cameraSize));

    mMainWindow->mEditor->object()->paintImage(painter, currentFrame, false, true);
    if (mIsDrawing)
    {
        mMainWindow->ui->scribbleArea->mBufferImg->paintImage(painter);
    }

    // Convert image to ASCII

    QString output = AsciiImage::convert(imageToExport);

    // Display image in preview dialog

    mPreviewDialog->setText(output);
    mPreviewDialog->setPageNumber(currentFrame);
    mPreviewDialog->show();
}

void ConsoleWindow::printPaper(QStringList args)
{
    // Check for exactly two arguments (x, y)
    if (args.size() != 2)
    {
        print(tr("You must specify either no arugments or a width and height to view . Use RENDER <x> <y>"));
        return;
    }

    // Try to parse arguments into numbers
    qreal w, h;
    bool ok, succeeded = true;
    w = args[0].toDouble(&ok);
    succeeded &= ok;
    h = args[1].toDouble(&ok);
    succeeded &= ok;
    if (!succeeded)
    {
        print(tr("Could not understand sizes %1 and %2. Please use numerical sizes.").arg(args[0], args[1]));
        return;
    }

    printPaper(QSize(w,h));
}

void ConsoleWindow::doPress(QStringList args)
{
    // Check to make sure we haven't already pressed
    if (mIsDrawing)
    {
        print(tr("Can't press because you are already drawing. Use RELEASE first to finish drawing the previous stroke."));
        return;
    }

    // Check for exactly two arguments (x, y)
    if (args.size() != 2)
    {
        print(tr("Press needs x and y coordinates of where to start pressing. Use PRESS <x> <y>"));
        return;
    }

    // Try to parse arguments into numbers
    float x, y;
    bool ok, succeeded = true;
    x = args[0].toFloat(&ok);
    succeeded &= ok;
    y = args[1].toFloat(&ok);
    succeeded &= ok;
    if (!succeeded)
    {
        print(tr("Could not understand positions %1 and %2. Please use numerical positions.").arg(args[0], args[1]));
        return;
    }

    mCurrentPos = QPointF(static_cast<double>(x), static_cast<double>(y));
    mIsDrawing = true;

    QMouseEvent *e = new QMouseEvent(QEvent::MouseButtonPress, mCurrentPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    mMainWindow->ui->scribbleArea->mousePressEvent(e);
    frameUpdate();
}

void ConsoleWindow::doMove(QStringList args)
{
    // Check to make sure we've already pressed
    if (!mIsDrawing)
    {
        print(tr("Can't move because you are not currently drawing. Use PRESS <x> <y> first."));
        return;
    }

    // Check for exactly two arguments (x, y)
    if (args.size() != 2)
    {
        print(tr("Move needs x and y coordinates of where to move to. Use MOVE <x> <y>"));
        return;
    }

    // Try to parse arguments into numbers
    float x, y;
    bool ok, succeeded = true;
    x = args[0].toFloat(&ok);
    succeeded &= ok;
    y = args[1].toFloat(&ok);
    succeeded &= ok;
    if (!succeeded)
    {
        print(tr("Could not understand positions %1 and %2. Please use numerical positions.").arg(args[0], args[1]));
        return;
    }

    mCurrentPos = QPointF(static_cast<double>(x), static_cast<double>(y));

    QMouseEvent *e = new QMouseEvent(QEvent::MouseMove, mCurrentPos, Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
    mMainWindow->ui->scribbleArea->mouseMoveEvent(e);
    frameUpdate();
}

void ConsoleWindow::doRelease()
{
    // Check to make sure we've already pressed
    if (!mIsDrawing)
    {
        print(tr("Can't release because you are not currently drawing. Use PRESS <x> <y> first."));
        return;
    }

    QMouseEvent *e = new QMouseEvent(QEvent::MouseButtonRelease, mCurrentPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    mMainWindow->ui->scribbleArea->mouseReleaseEvent(e);
    mIsDrawing = false;
    frameUpdate();
}

void ConsoleWindow::doPlugSpeaker(bool shouldPlay)
{
    if (shouldPlay)
    {
        if (mSpeaker->state() == QMediaPlayer::PlayingState)
        {
            print(tr("The speaker is already plugged in!"));
        }
        else
        {
            mSpeaker->play();
            print(tr("Plugged in speaker :)"));
        }
    }
    else
    {
        if(mSpeaker->state() != QMediaPlayer::PlayingState)
        {
            print(tr("The speaker is already unplugged!"));
        }
        else
        {
            mSpeaker->pause();
            print(tr("Unplugged speaker ;("));
        }
    }
}

void ConsoleWindow::doPage(QString arg)
{
    if (arg.isEmpty())
    {
        print(tr("The current page is %1. You can use PAGE PREVIOUS, PAGE NEXT, or PAGE <number> to navigate through your pile of PAPERS.").arg(mMainWindow->mEditor->currentFrame()));
    }
    else if (arg == tr("next") || arg == tr("forward") || arg == tr("forwards"))
    {
        mMainWindow->mEditor->scrubForward();
        print(tr("You put the current PAPER off to the side to VIEW the next one. It is page #%1").arg(mMainWindow->mEditor->currentFrame()));
    }
    else if (arg == tr("previous") || arg == tr("back") || arg == tr("backward") || arg == tr("backwards"))
    {
        mMainWindow->mEditor->scrubBackward();
        print(tr("You put the previous PAPER over the current paper to VIEW it. It is page #%1").arg(mMainWindow->mEditor->currentFrame()));
    }
    else
    {
        bool ok = true;
        int n = arg.toInt(&ok);
        if (!ok || n <= 0)
        {
            print(tr("%1 is not a page number. All page numbers are positive whole numbers.").arg(arg));
        }
        else
        {
            mMainWindow->mEditor->scrubTo(n);
            print(tr("You rummage through your papers until you find page %1. You put in on top so you can VIEW it.").arg(n));
        }
    }
}

void ConsoleWindow::doPlay()
{
    if (!mMainWindow->mEditor->playback()->isPlaying())
    {
        mMainWindow->mEditor->playback()->play();

        print(tr("You pick up the papers and start to flip them between your fingers. As you VIEW them, the still drawings come to life! At any time you can STOP."));
    }
    else
    {
        print(tr("You're already flipping through the papers."));
    }
}

void ConsoleWindow::doStop()
{
    if (mMainWindow->mEditor->playback()->isPlaying())
    {
        mMainWindow->mEditor->playback()->stop();
        print(tr("You stop flipping the papers. That was fun."));
    }
    else
    {
        print(tr("You can't stop what you don't start!"));
    }
}

void ConsoleWindow::doOpen()
{
    mMainWindow->openDocument();
    mMainWindow->mEditor->color()->setColor(Qt::black);
    LayerManager *lm = mMainWindow->mEditor->layers();
    mCamLayer = (LayerCamera *) lm->findLayerByName("ASCII Camera", Layer::CAMERA);
    if (mCamLayer == nullptr)
    {
        mCamLayer = lm->createCameraLayer("ASCII Camera");
        mCamLayer->setViewRect(QRect(mMainWindow->mEditor->view()->mapScreenToCanvas(QPoint(0, 0)).toPoint(), QSize(50, 50)));
    }
    else
    {
        QSize camSize = mCamLayer->getViewSize();
        mMainWindow->mEditor->view()->translate(camSize.width(), camSize.height());
    }
    LayerCamera *curCam;
    mDrawingLayer = (LayerBitmap*) lm->findLayerByName("ASCII Bitmap", Layer::BITMAP);
    if (mDrawingLayer == nullptr)
    {
        mDrawingLayer = lm->createBitmapLayer("ASCII Bitmap");
    }
    lm->setCurrentLayer(mDrawingLayer);
}

void ConsoleWindow::doSave()
{
    bool success = mMainWindow->saveAsNewDocument();

    if (success)
    {
        print(tr("You magically command the papers to save. You're not sure what happend, but you feel like it worked."));
    }
    else
    {
        print(tr("You magically command the papers to save, but nothing happens."));
    }
}
