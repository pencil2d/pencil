#ifndef FRAMECOMMENTWIDGET_H
#define FRAMECOMMENTWIDGET_H

#include <QWidget>
#include "basedockwidget.h"

class Editor;
class KeyFrame;
class Layer;

namespace Ui {
    class FrameComment;
}

class FrameCommentWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    explicit FrameCommentWidget(QWidget *parent = nullptr);
    ~FrameCommentWidget() override;

    void initUI() override;
    void updateUI() override;
    void setCore(Editor* editor);

    void applyComments();
    void fillComments();

private:
    Ui::FrameComment *ui;

    void dialogueTextChanged();
    void actionTextChanged();
    void slugTextChanged();

    void enableCommentFields();
    void disableCommentFields();

    void currentFrameChanged(int frame);
    void currentLayerChanged(int index);

    void clearFrameCommentsFields();

    void updateConnections();

    void playStateChanged(bool isPlaying);

    KeyFrame* getKeyFrame();

    void makeConnections();
    void disconnectNotifiers();

    bool mIsPlaying = false;

    Editor* mEditor = nullptr;
};

#endif // FRAMECOMMENTWIDGET_H
