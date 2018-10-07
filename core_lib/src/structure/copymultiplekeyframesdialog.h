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
    int getFirstFrame();        // first frame in range
    int getLastFrame();         // last frame in range
    QString getFromLayer();     // Layer thet holds the Range
    QString getCopyToLayer();   // Layer that receives Copy Range
    QString getMoveToLayer();   // Layer the Range is moved to
    QString getDeleteOnLayer(); // Layer the range is deleted on
    int getNumLoops();          // Number of loops asked for
    void setNumLoopsMax(int numLoopsMax);
    int getCopyStartFrame();
    int getMoveStartFrame();
    int getReverseStartFrame();
    QString getActiveTab();

private slots:
    // Range and Layers
    void setFirstFrame(int firstFrame);
    void setLastFrame(int lastFrame);
    void setFromLayer(QString fromLayer);
    // copy Range
    void setNumLoops(int numLoops);
    void setStartFrame(int startFrame);
    void setCopyToLayer(QString copyToLayer);
    // move Range
    void setMoveStartFrame(int startFrame);
    void setMoveToLayer(QString moveToLayer);
    // reverse Range
    void setReverseFrom(int reverseFrom);
    // delete Range
    void setDeleteOnLayer(QString deleteFromLayer);

    void setMethodPicked(int tabIndex);


private:
    Ui::CopyMultiplekeyframesDialog *ui;

    int mCurrentTab;        // Index of current tab
    int mFirstFrame;        // Frame# that starts loop
    int mLastFrame;         // Frame# that ends loop
    int mNumLoops;          // Number of loops
    int mCopyStart;         // Frame# to insert first copied frame
    int mMoveStart;         // Frame# to insert first moved frame
    int mReverseStart;      // Frame# to insert first reversed frame
    QString mFromLayer;     // name of From Layer
    QString mCopyToLayer;   // name of To Layer you copy to
    QString mMoveToLayer;   // name of To Layer you move to
    QString mDeleteOnLayer;
    LayerManager *lMgr;
    int mTimelineLength;    // frames in scene/in timeline

    void checkValidity();

};

#endif // COPYMULTIPLEKEYFRAMESDIALOG_H
