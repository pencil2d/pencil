#ifndef FRAMECOMMENTWIDGET_H
#define FRAMECOMMENTWIDGET_H

#include <QWidget>
#include "basedockwidget.h"
#include "editor.h"
#include "keyframe.h"
#include "layer.h"

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
    void dialogueTextChanged(QString text);
    void actionTextChanged(QString text);
    void notesTextChanged(QString text);
    void currentFrameChanged(int frame);
    void currentLayerChanged(int index);
    void clearFrameCommentsFields();
    void applyFrameComments();
    void playStateChanged(bool isPlaying);
    void updateConnections();

private:
    Ui::FrameComment *ui;

    void fillFrameComments();
    void connectAll();
    void disconnectAll();

    bool mIsPlaying = false;

    Editor* mEditor = nullptr;
    Layer* mLayer = nullptr;
    KeyFrame* mKeyframe = nullptr;

};

#endif // FRAMECOMMENTWIDGET_H
