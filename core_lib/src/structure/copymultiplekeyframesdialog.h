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
    ~CopyMultiplekeyframesDialog();
    int getStartLoop();
    void setStartLoop(int i);
    int getStopLoop();
    void setStopLoop(int i);
    int getNumLoops();
    void setNumLoops(int i);
    void setNumLoopsMax(int i);
    int getStartFrame();
    void setStartFrame(int i);

private:
    Ui::CopyMultiplekeyframesDialog *ui;

    int mStartFrame;    // Frame# that starts loop
    int mStopFrame;     // Frame# that ends loop
    int mNumLoops;      // Number of loops
    int mFromFrame;     // Frame# to insert first copied frame

    bool checkValidity();


};

#endif // COPYMULTIPLEKEYFRAMESDIALOG_H
