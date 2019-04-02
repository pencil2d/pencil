#ifndef CONSOLEWINDOW_H
#define CONSOLEWINDOW_H

#include <QMainWindow>

namespace Ui {
class ConsoleWindow;
}

class MainWindow2;
class QMediaPlayer;
class LayerCamera;
class LayerBitmap;
class AsciiPreviewDialog;

class ConsoleWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConsoleWindow(QWidget *parent = nullptr);
    ~ConsoleWindow() override;

    void print(QString s);
    void printHelp();
    void printLook(QString arg);
    void printEquip(QString term, QStringList args);
    void printPaper(QStringList args);
    void printPaper(QSize renderSize=QSize());

    void doPress(QStringList args);
    void doMove(QStringList args);
    void doRelease();
    void doPlugSpeaker(bool shouldPlay);
    void doPage(QString arg);
    void doPlay();
    void doStop();
    void doOpen();
    void doSave();

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void runCommand();
    void frameChanged(int index);
    void frameUpdate();

private:
    Ui::ConsoleWindow *ui;

    const QSize RENDER_SIZE{100, 50};

    MainWindow2 *mMainWindow;
    QMediaPlayer *mSpeaker;
    bool mIsOnSplash = true;
    bool mHasLookedLamp = false;
    int mDiscardedPaper = 0;
    QPointF mCurrentPos;
    bool mIsDrawing = false;
    LayerCamera *mCamLayer;
    LayerBitmap *mDrawingLayer;
    AsciiPreviewDialog *mPreviewDialog;
    QSize mCurrentRenderSize;
};

#endif // CONSOLEWINDOW_H
