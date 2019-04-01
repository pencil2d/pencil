#ifndef CONSOLEWINDOW_H
#define CONSOLEWINDOW_H

#include <QMainWindow>

namespace Ui {
class ConsoleWindow;
}

class MainWindow2;
class QMediaPlayer;

class ConsoleWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConsoleWindow(QWidget *parent = nullptr);
    ~ConsoleWindow() override;

    void print(QString s);
    void printHelp();
    void printLook(QString arg);
    void printEquip(QString term, QString arg);

    void doPress(QStringList args);
    void doMove(QStringList args);
    void doRelease();

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void runCommand();

private:
    Ui::ConsoleWindow *ui;

    MainWindow2 *mMainWindow;
    QMediaPlayer *mSpeaker;
    bool mIsOnSplash = true;
    bool mHasLookedLamp = false;
    int mDiscardedPaper = 0;
    QPointF mCurrentPos;
    bool mIsDrawing = false;
};

#endif // CONSOLEWINDOW_H
