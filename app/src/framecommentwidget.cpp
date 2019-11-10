#include "framecommentwidget.h"
#include "ui_framecommentwidget.h"

#include "editor.h"
#include "keyframe.h"

#include "layermanager.h"
#include "playbackmanager.h"

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
        if (frame >= mEditor->layers()->currentLayer()->firstKeyFramePosition())
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

void FrameCommentWidget::fillComments()
{
    KeyFrame* keyframe = getKeyFrame();
    if (keyframe == nullptr) { return; }

    QSignalBlocker b(ui->textEditDialogue);
    QSignalBlocker b2(ui->textEditAction);
    QSignalBlocker b3(ui->textEditSlug);

    ui->textEditDialogue->setPlainText(keyframe->getDialogueComment());
    ui->textEditAction->setPlainText(keyframe->getActionComment());
    ui->textEditSlug->setPlainText(keyframe->getSlugComment());
}

void FrameCommentWidget::applyComments()
{
    KeyFrame* keyframe = getKeyFrame();
    if (keyframe == nullptr) { return; }

    keyframe->setDialogueComment(ui->textEditDialogue->toPlainText());
    keyframe->setActionComment(ui->textEditAction->toPlainText());
    keyframe->setSlugComment(ui->textEditSlug->toPlainText());
    mEditor->layers()->currentLayer()->setModified(keyframe->pos(), true);
}

KeyFrame* FrameCommentWidget::getKeyFrame()
{
    const int frame = mEditor->currentFrame();
    const Layer* layer = mEditor->layers()->currentLayer();
    KeyFrame* keyframe = layer->getKeyFrameAt(frame);
    if (keyframe == nullptr)
        keyframe = layer->getKeyFrameAt(layer->getPreviousFrameNumber(frame, true));
    if (keyframe == nullptr) { return nullptr; }

    return keyframe;
}

void FrameCommentWidget::makeConnections()
{
    connect(ui->textEditDialogue, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::dialogueTextChanged);
    connect(ui->textEditAction, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::actionTextChanged);
    connect(ui->textEditSlug, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::slugTextChanged);
    connect(ui->btnClearFields, &QPushButton::clicked, this, &FrameCommentWidget::clearFrameCommentsFields);

    connect(ui->textEditSlug, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::applyComments);
    connect(ui->textEditAction, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::applyComments);
    connect(ui->textEditDialogue, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::applyComments);

    connect(mEditor, &Editor::currentFrameChanged, this, &FrameCommentWidget::currentFrameChanged);
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &FrameCommentWidget::currentLayerChanged);
    connect(mEditor, &Editor::objectLoaded, this, &FrameCommentWidget::fillComments);
    connect(mEditor->playback(), &PlaybackManager::playStateChanged, this, &FrameCommentWidget::playStateChanged);
}

void FrameCommentWidget::disconnectNotifiers()
{
    disconnect(ui->textEditDialogue, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::dialogueTextChanged);
    disconnect(ui->textEditAction, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::actionTextChanged);
    disconnect(ui->textEditSlug, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::slugTextChanged);
    disconnect(ui->btnClearFields, &QPushButton::clicked, this, &FrameCommentWidget::clearFrameCommentsFields);

    disconnect(ui->textEditSlug, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::applyComments);
    disconnect(ui->textEditAction, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::applyComments);
    disconnect(ui->textEditDialogue, &QPlainTextEdit::textChanged, this, &FrameCommentWidget::applyComments);

    disconnect(mEditor, &Editor::currentFrameChanged, this, &FrameCommentWidget::currentFrameChanged);
    disconnect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &FrameCommentWidget::currentLayerChanged);
    disconnect(mEditor, &Editor::objectLoaded, this, &FrameCommentWidget::fillComments);
    disconnect(mEditor->playback(), &PlaybackManager::playStateChanged, this, &FrameCommentWidget::playStateChanged);
}
