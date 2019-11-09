#include "framecommentwidget.h"
#include "ui_framecommentwidget.h"

#include "editor.h"
#include "keyframe.h"

#include "layermanager.h"
#include "playbackmanager.h"
#include "keyframetextedit.h"

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
    connect(this, &FrameCommentWidget::visibilityChanged, this, &FrameCommentWidget::updateConnections);
    updateConnections();
}

void FrameCommentWidget::updateUI()
{

}

void FrameCommentWidget::setCore(Editor *editor)
{
    mEditor = editor;
    mLayer = editor->layers()->currentLayer();
    mKeyframe = mLayer->getKeyFrameAt(mEditor->currentFrame());
    currentFrameChanged(mEditor->currentFrame());
}

void FrameCommentWidget::dialogueTextChanged()
{
    int len = ui->textEditDialogue->toPlainText().length();
    ui->labelDialogueCounter->setText(tr("%1 chars").arg(QString::number(len)));
}

void FrameCommentWidget::actionTextChanged()
{
    int len = ui->textEditAction->toPlainText().length();
    ui->labelActionCounter->setText(tr("%1 chars").arg(QString::number(len)));
}

void FrameCommentWidget::notesTextChanged()
{
    int len = ui->textEditSlug->toPlainText().length();
    ui->labelSlugCounter->setText(tr("%1 chars").arg(QString::number(len)));
}

void FrameCommentWidget::currentFrameChanged(int frame)
{
    if (!mIsPlaying)
    {
        if (mLayer->firstKeyFramePosition() <= frame)
        {
            fillFrameComments();
        }
        else
        {
            clearFrameCommentsFields();
        }
    }
}

void FrameCommentWidget::currentLayerChanged(int index)
{
    Q_UNUSED(index)
    mLayer = mEditor->layers()->currentLayer();
    currentFrameChanged(mEditor->currentFrame());
}

void FrameCommentWidget::clearFrameCommentsFields()
{
    ui->textEditDialogue->clear();
    ui->textEditAction->clear();
    ui->textEditSlug->clear();
}

void FrameCommentWidget::applyFrameComments()
{
    mKeyframe = mLayer->getKeyFrameAt(mEditor->currentFrame());
    if (mKeyframe == nullptr)
        mKeyframe = mLayer->getKeyFrameAt(mLayer->getPreviousFrameNumber(mEditor->currentFrame(), true));
    if (mKeyframe == nullptr) { return; }
    mKeyframe->setDialogueComment(ui->textEditDialogue->toPlainText());
    mKeyframe->setActionComment(ui->textEditAction->toPlainText());
    mKeyframe->setSlugComment(ui->textEditSlug->toPlainText());
    mLayer->setModified(mKeyframe->pos(), true);
}

void FrameCommentWidget::playStateChanged(bool isPlaying)
{
    mIsPlaying = isPlaying;
    if (!mIsPlaying)
    {
        mLayer = mEditor->layers()->currentLayer();
        currentFrameChanged(mEditor->currentFrame());
    }
}

void FrameCommentWidget::updateConnections()
{
    if (!isVisible())
    {
        disconnectNotifiers();
    }
    else
    {
        makeConnections();
    }
}

void FrameCommentWidget::fillFrameComments()
{
    mKeyframe = mLayer->getKeyFrameAt(mEditor->currentFrame());
    if (mKeyframe == nullptr)
        mKeyframe = mLayer->getKeyFrameAt(mLayer->getPreviousFrameNumber(mEditor->currentFrame(), true));
    if (mKeyframe == nullptr) { return; }
    ui->textEditDialogue->setPlainText(mKeyframe->getDialogueComment());
    ui->textEditAction->setPlainText(mKeyframe->getActionComment());
    ui->textEditSlug->setPlainText(mKeyframe->getSlugComment());
}

void FrameCommentWidget::makeConnections()
{
    connect(ui->textEditDialogue, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::dialogueTextChanged);
    connect(ui->textEditAction, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::actionTextChanged);
    connect(ui->textEditSlug, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::notesTextChanged);
    connect(ui->btnClearFields, &QPushButton::clicked, this, &FrameCommentWidget::clearFrameCommentsFields);

    connect(ui->textEditSlug, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyFrameComments);
    connect(ui->textEditAction, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyFrameComments);
    connect(ui->textEditDialogue, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyFrameComments);

    connect(mEditor, &Editor::currentFrameChanged, this, &FrameCommentWidget::currentFrameChanged);
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &FrameCommentWidget::currentLayerChanged);
    connect(mEditor, &Editor::objectLoaded, this, &FrameCommentWidget::fillFrameComments);
    connect(mEditor->playback(), &PlaybackManager::playStateChanged, this, &FrameCommentWidget::playStateChanged);
}

void FrameCommentWidget::disconnectNotifiers()
{
    disconnect(ui->textEditDialogue, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::dialogueTextChanged);
    disconnect(ui->textEditAction, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::actionTextChanged);
    disconnect(ui->textEditSlug, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::notesTextChanged);
    disconnect(ui->btnClearFields, &QPushButton::clicked, this, &FrameCommentWidget::clearFrameCommentsFields);

    disconnect(ui->textEditSlug, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyFrameComments);
    disconnect(ui->textEditAction, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyFrameComments);
    disconnect(ui->textEditDialogue, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyFrameComments);

    disconnect(mEditor, &Editor::currentFrameChanged, this, &FrameCommentWidget::currentFrameChanged);
    disconnect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &FrameCommentWidget::currentLayerChanged);
    disconnect(mEditor, &Editor::objectLoaded, this, &FrameCommentWidget::fillFrameComments);
    disconnect(mEditor->playback(), &PlaybackManager::playStateChanged, this, &FrameCommentWidget::playStateChanged);
}
