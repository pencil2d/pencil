#ifndef FRAMECOMMENTWIDGET_H
#define FRAMECOMMENTWIDGET_H

#include <QWidget>
#include "basedockwidget.h"

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

private:
    Ui::FrameComment *ui;
};

#endif // FRAMECOMMENTWIDGET_H
