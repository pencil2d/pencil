#ifndef COPYMULTIPLEKEYFRAMESDIALOG_H
#define COPYMULTIPLEKEYFRAMESDIALOG_H

#include <QDialog>

namespace Ui {
class CopyMultiplekeyframesDialog;
}

class CopyMultiplekeyframesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopyMultiplekeyframesDialog(QWidget *parent = 0);
    explicit CopyMultiplekeyframesDialog(QWidget *parent = 0, int startLoop = 1, int stopLoop = 2);
    ~CopyMultiplekeyframesDialog();
    void init();
    int getStartLoop();
    int getStopLoop();
    int getNumLoops();
    void setNumLoopsMax(int i);
    int getStartFrame();

private slots:
    void setStartLoop(int i);
    void setStopLoop(int i);
    void setNumLoops(int i);
    void setStartFrame(int i);

private:
    Ui::CopyMultiplekeyframesDialog *ui;

    int mStartLoop;    // Frame# that starts loop
    int mStopLoop;     // Frame# that ends loop
    int mNumLoops;      // Number of loops
    int mStartFrame;     // Frame# to insert first copied frame
    int mTimelineLength; // frames in scene/in timeline

    void checkValidity();


};

#endif // COPYMULTIPLEKEYFRAMESDIALOG_H
