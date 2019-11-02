#ifndef FRAMECOMMENTWIDGET_H
#define FRAMECOMMENTWIDGET_H

#include <QWidget>
#include "basedockwidget.h"
#include "editor.h"

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

public slots:
    void dialogueTextChanged(QString text);
    void actionTextChanged(QString text);
    void notesTextChanged(QString text);
    void currentFrameChanged(int frame);
    void clearFrameCommentsFields();
    void updateFrameComments();

private:
    Ui::FrameComment *ui;

    void fillFrameComments();
};

#endif // FRAMECOMMENTWIDGET_H
