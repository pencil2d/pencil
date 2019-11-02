#include "framecommentwidget.h"
#include "ui_framecommentwidget.h"

#include "editor.h"
#include "layermanager.h"
#include "keyframe.h"

FrameCommentWidget::FrameCommentWidget(QWidget *parent) :
    BaseDockWidget(parent)
{
    QWidget* innerWidget = new QWidget;
    setWindowTitle(tr("Frame Comments"));

    ui = new Ui::FrameComment;
    ui->setupUi(innerWidget);
    setWidget(innerWidget);
}

FrameCommentWidget::~FrameCommentWidget()
{
    delete ui;
}

void FrameCommentWidget::initUI()
{
    connect(ui->leDialogue, &QLineEdit::textChanged, this, &FrameCommentWidget::dialogueTextChanged);
    connect(ui->leAction, &QLineEdit::textChanged, this, &FrameCommentWidget::actionTextChanged);
    connect(ui->leNotes, &QLineEdit::textChanged, this, &FrameCommentWidget::notesTextChanged);
    connect(editor(), &Editor::currentFrameChanged, this, &FrameCommentWidget::currentFrameChanged);
    connect(ui->btnClearFields, &QPushButton::clicked, this, &FrameCommentWidget::clearFrameCommentsFields);
    connect(ui->btnApplyComments, &QPushButton::clicked, this, &FrameCommentWidget::updateFrameComments);
    dialogueTextChanged("");
    actionTextChanged("");
    notesTextChanged("");
}

void FrameCommentWidget::updateUI()
{

}

void FrameCommentWidget::dialogueTextChanged(QString text)
{
    int len = text.length();
    ui->labDialogueCount->setText(tr("%1 chars").arg(QString::number(len)));
}

void FrameCommentWidget::actionTextChanged(QString text)
{
    int len = text.length();
    ui->labActionCount->setText(tr("%1 chars").arg(QString::number(len)));
}

void FrameCommentWidget::notesTextChanged(QString text)
{
    int len = text.length();
    ui->labNotesCount->setText(tr("%1 chars").arg(QString::number(len)));
}

void FrameCommentWidget::currentFrameChanged(int frame)
{
    if (editor()->layers()->currentLayer()->keyExists(frame))
    {
        fillFrameComments();
        ui->btnApplyComments->setEnabled(true);
    }
    else
    {
        clearFrameCommentsFields();
        ui->btnApplyComments->setEnabled(false);
    }
}

void FrameCommentWidget::clearFrameCommentsFields()
{
    ui->leDialogue->clear();
    ui->leAction->clear();
    ui->leNotes->clear();
}

void FrameCommentWidget::updateFrameComments()
{
    KeyFrame* key = editor()->layers()->currentLayer()->getKeyFrameAt(editor()->currentFrame());
    if (key == nullptr) { return; }
    key->setDialogueComment(ui->leDialogue->text());
    key->setActionComment(ui->leAction->text());
    key->setNotesComment(ui->leNotes->text());
}

void FrameCommentWidget::fillFrameComments()
{
    KeyFrame* key = editor()->layers()->currentLayer()->getKeyFrameAt(editor()->currentFrame());
    if (key == nullptr) { return; }
    ui->leDialogue->setText(key->getDialogueComment());
    ui->leAction->setText(key->getActionComment());
    ui->leNotes->setText(key->getNotesComment());
}

