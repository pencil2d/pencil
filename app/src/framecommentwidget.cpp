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

void FrameCommentWidget::slugTextChanged()
{
    int len = ui->textEditSlug->toPlainText().length();
    ui->labelSlugCounter->setText(tr("%1 chars").arg(QString::number(len)));
}

void FrameCommentWidget::currentFrameChanged(int frame)
{
    if (!mIsPlaying)
    {
        if (mEditor->layers()->currentLayer()->firstKeyFramePosition() <= frame)
        {
            fillComments();
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
    currentFrameChanged(mEditor->currentFrame());
}

void FrameCommentWidget::clearFrameCommentsFields()
{
    ui->textEditDialogue->clear();
    ui->textEditAction->clear();
    ui->textEditSlug->clear();
}

void FrameCommentWidget::playStateChanged(bool isPlaying)
{
    mIsPlaying = isPlaying;
    if (!mIsPlaying)
    {
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

void FrameCommentWidget::applyCommentsToKeyframe(const int frame)
{
    KeyFrame* keyframe = getKeyFrame(frame);
    if (keyframe == nullptr) { return; }

    keyframe->setDialogueComment(ui->textEditDialogue->toPlainText());
    keyframe->setActionComment(ui->textEditAction->toPlainText());
    keyframe->setSlugComment(ui->textEditSlug->toPlainText());
    mEditor->layers()->currentLayer()->setModified(keyframe->pos(), true);
}

void FrameCommentWidget::fillComments()
{
    KeyFrame* keyframe = getKeyFrame(mEditor->currentFrame());
    if (keyframe == nullptr) { return; }

    ui->textEditDialogue->setPlainText(keyframe->getDialogueComment());
    ui->textEditAction->setPlainText(keyframe->getActionComment());
    ui->textEditSlug->setPlainText(keyframe->getSlugComment());
}

void FrameCommentWidget::applyComments()
{
    KeyFrame* keyframe = getKeyFrame(mEditor->currentFrame());
    if (keyframe == nullptr) { return; }

    keyframe->setDialogueComment(ui->textEditDialogue->toPlainText());
    keyframe->setActionComment(ui->textEditAction->toPlainText());
    keyframe->setSlugComment(ui->textEditSlug->toPlainText());
    mEditor->layers()->currentLayer()->setModified(keyframe->pos(), true);
}

KeyFrame* FrameCommentWidget::getKeyFrame(int frame)
{
    Layer* layer = mEditor->layers()->currentLayer();
    KeyFrame* keyframe = layer->getKeyFrameAt(frame);
    if (keyframe == nullptr)
        keyframe = layer->getKeyFrameAt(layer->getPreviousFrameNumber(frame, true));
    if (keyframe == nullptr) { return nullptr; }

    return keyframe;
}

void FrameCommentWidget::makeConnections()
{
    connect(ui->textEditDialogue, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::dialogueTextChanged);
    connect(ui->textEditAction, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::actionTextChanged);
    connect(ui->textEditSlug, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::slugTextChanged);
    connect(ui->btnClearFields, &QPushButton::clicked, this, &FrameCommentWidget::clearFrameCommentsFields);

    connect(ui->textEditSlug, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyComments);
    connect(ui->textEditAction, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyComments);
    connect(ui->textEditDialogue, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyComments);

    connect(mEditor, &Editor::currentFrameChanged, this, &FrameCommentWidget::currentFrameChanged);
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &FrameCommentWidget::currentLayerChanged);
    connect(mEditor, &Editor::objectLoaded, this, &FrameCommentWidget::fillComments);
    connect(mEditor->playback(), &PlaybackManager::playStateChanged, this, &FrameCommentWidget::playStateChanged);
    connect(mEditor, &Editor::aboutToChangeFrame, this, &FrameCommentWidget::applyCommentsToKeyframe);
}

void FrameCommentWidget::disconnectNotifiers()
{
    disconnect(ui->textEditDialogue, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::dialogueTextChanged);
    disconnect(ui->textEditAction, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::actionTextChanged);
    disconnect(ui->textEditSlug, &KeyFrameTextEdit::textChanged, this, &FrameCommentWidget::slugTextChanged);
    disconnect(ui->btnClearFields, &QPushButton::clicked, this, &FrameCommentWidget::clearFrameCommentsFields);

    disconnect(ui->textEditSlug, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyComments);
    disconnect(ui->textEditAction, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyComments);
    disconnect(ui->textEditDialogue, &KeyFrameTextEdit::lostFocus, this, &FrameCommentWidget::applyComments);

    disconnect(mEditor, &Editor::currentFrameChanged, this, &FrameCommentWidget::currentFrameChanged);
    disconnect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &FrameCommentWidget::currentLayerChanged);
    disconnect(mEditor, &Editor::objectLoaded, this, &FrameCommentWidget::fillComments);
    disconnect(mEditor->playback(), &PlaybackManager::playStateChanged, this, &FrameCommentWidget::playStateChanged);
}
