#ifndef COPYMULTIPLEKEYFRAMESDIALOG_H
#define COPYMULTIPLEKEYFRAMESDIALOG_H

#include <QDialog>
#include "layermanager.h"
#include "editor.h"

namespace Ui {
class CopyMultiplekeyframesDialog;
}

class CopyMultiplekeyframesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopyMultiplekeyframesDialog(QWidget *parent = 0);
    explicit CopyMultiplekeyframesDialog(LayerManager *lm, int startLoop = 1, int stopLoop = 2, QWidget *parent = 0);
    ~CopyMultiplekeyframesDialog();
    void init();
    int getFirstFrame();
    int getLastFrame();
    int getNumLoops();
    void setNumLoopsMax(int i);
    int getStartFrame();
    QString getRadioChecked();

private slots:
    void setFirstFrame(int i);
    void setLastFrame(int i);
    void setNumLoops(int i);
    void setStartFrame(int i);

private:
    Ui::CopyMultiplekeyframesDialog *ui;

    int mFirstFrame;    // Frame# that starts loop
    int mLastFrame;     // Frame# that ends loop
    int mNumLoops;      // Number of loops
    int mStartFrame;     // Frame# to insert first copied frame
    int mTimelineLength; // frames in scene/in timeline
    Editor* mEditor = nullptr;

    void checkValidity();


};

#endif // COPYMULTIPLEKEYFRAMESDIALOG_H
