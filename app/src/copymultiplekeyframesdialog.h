#ifndef COPYMULTIPLEKEYFRAMESDIALOG_H
#define COPYMULTIPLEKEYFRAMESDIALOG_H

#include <QDialog>
#include "layermanager.h"

namespace Ui {
class CopyMultiplekeyframesDialog;
}

class CopyMultiplekeyframesDialog : public QDialog
{
    Q_OBJECT

public:
    enum KeyFrameManipulation
    {
        COPY,
        MOVE,
        REVERSE,
        DELETE,
    };

    explicit CopyMultiplekeyframesDialog(QWidget *parent = nullptr);
    explicit CopyMultiplekeyframesDialog(LayerManager *lm, int startLoop = 1, int stopLoop = 2, QWidget *parent = nullptr);
    ~CopyMultiplekeyframesDialog();
    void init();
    int getFirstFrame() { return mFirstFrame; }             ///< first frame in range
    int getLastFrame() { return mLastFrame; }               ///< last frame in range
    QString getFromLayer() { return mFromLayer; }           ///< Layer thet holds the Range
    QString getCopyToLayer() { return mCopyToLayer; }       ///< Layer that receives Copy Range
    QString getMoveToLayer() { return mMoveToLayer; }       ///< Layer the Range is moved to
    int getNumLoops() { return mNumLoops; }                 ///< Number of loops asked for
    int getCurrentTab() { return mCurrentTab; }             ///< Tab/Method currently picked
    int getCopyStartFrame();
    int getMoveStartFrame();
    int getReverseStartFrame();
    bool getValidity();
    void setStartEnd(int methodChosen);

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

    void setMethodPicked(int tabIndex);


private:
    Ui::CopyMultiplekeyframesDialog *ui;

    int mCurrentTab;        ///< Index of current tab
    int mFirstFrame;        ///< Frame# that starts loop
    int mLastFrame;         ///< Frame# that ends loop
    int mNumLoops;          ///< Number of loops
    int mCopyStart;         ///< Frame# to insert first copied frame
    int mMoveStart;         ///< Frame# to insert first moved frame
    int mReverseStart;      ///< Frame# to insert first reversed frame
    int mManiStartAt;       ///< First frame affected of change
    int mManiEndAt;         ///< Last frame affected of change

    bool mValidAction;      ///< Validity of desired action

    QString mFromLayer;     ///< name of From Layer
    QString mCopyToLayer;   ///< name of To Layer you copy to
    QString mMoveToLayer;   ///< name of To Layer you move to
    QString mLabWarning;
    LayerManager *lMgr;

    void checkValidity();

};

#endif // COPYMULTIPLEKEYFRAMESDIALOG_H
