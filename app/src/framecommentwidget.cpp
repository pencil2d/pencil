#include "framecommentwidget.h"
#include "ui_framecommentwidget.h"

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
