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

public slots:
    void dialogueTextChanged();
    void actionTextChanged();
    void notesTextChanged();
    void currentFrameChanged(int frame);
    void currentLayerChanged(int index);
    void clearFrameCommentsFields();
    void applyFrameComments();
    void playStateChanged(bool isPlaying);
    void updateConnections();

private:
    Ui::FrameComment *ui;

    void fillFrameComments();
    void makeConnections();
    void disconnectNotifiers();

    bool mIsPlaying = false;

    Editor* mEditor = nullptr;
    Layer* mLayer = nullptr;
    KeyFrame* mKeyframe = nullptr;

};

#endif // FRAMECOMMENTWIDGET_H
