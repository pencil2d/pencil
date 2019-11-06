#include "framecommentwidget.h"
#include "ui_framecommentwidget.h"

#include "editor.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include <QDebug>

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
    dialogueTextChanged("");
    actionTextChanged("");
    notesTextChanged("");
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
    if (!mIsPlaying)
    {
        if (mLayer->firstKeyFramePosition() <= frame)
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
}

void FrameCommentWidget::currentLayerChanged(int index)
{
    Q_UNUSED(index)
    mLayer = mEditor->layers()->currentLayer();
    currentFrameChanged(mEditor->currentFrame());
}

void FrameCommentWidget::clearFrameCommentsFields()
{
    ui->leDialogue->clear();
    ui->leAction->clear();
    ui->leNotes->clear();
    if (!mLayer->keyExists(mEditor->currentFrame()))
        ui->labLayerFrame->setText("");
}

void FrameCommentWidget::applyFrameComments()
{
    mKeyframe = mLayer->getKeyFrameAt(mEditor->currentFrame());
    if (mKeyframe == nullptr)
        mKeyframe = mLayer->getKeyFrameAt(mLayer->getPreviousFrameNumber(mEditor->currentFrame(), true));
    if (mKeyframe == nullptr) { return; }
    mKeyframe->setDialogueComment(ui->leDialogue->text());
    mKeyframe->setActionComment(ui->leAction->text());
    mKeyframe->setSlugComment(ui->leNotes->text());
    ui->labLayerFrame->setText(tr("%1 #%2 :").arg(mLayer->name()).arg(QString::number(mKeyframe->pos())));
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
    if (isVisible())
    {
        connectAll();
    }
    else
    {
        disconnectAll();
    }
}

void FrameCommentWidget::fillFrameComments()
{
    mKeyframe = mLayer->getKeyFrameAt(mEditor->currentFrame());
    if (mKeyframe == nullptr)
        mKeyframe = mLayer->getKeyFrameAt(mLayer->getPreviousFrameNumber(mEditor->currentFrame(), true));
    if (mKeyframe == nullptr) { return; }
    ui->leDialogue->setText(mKeyframe->getDialogueComment());
    ui->leAction->setText(mKeyframe->getActionComment());
    ui->leNotes->setText(mKeyframe->getSlugComment());
    ui->labLayerFrame->setText(tr("%1 #%2 :").arg(mEditor->layers()->currentLayer()->name()).arg(QString::number(mKeyframe->pos())));
}

void FrameCommentWidget::connectAll()
{
    connect(ui->leDialogue, &QLineEdit::textChanged, this, &FrameCommentWidget::dialogueTextChanged);
    connect(ui->leAction, &QLineEdit::textChanged, this, &FrameCommentWidget::actionTextChanged);
    connect(ui->leNotes, &QLineEdit::textChanged, this, &FrameCommentWidget::notesTextChanged);
    connect(mEditor, &Editor::currentFrameChanged, this, &FrameCommentWidget::currentFrameChanged);
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &FrameCommentWidget::currentLayerChanged);
    connect(ui->btnClearFields, &QPushButton::clicked, this, &FrameCommentWidget::clearFrameCommentsFields);
    connect(ui->btnApplyComments, &QPushButton::clicked, this, &FrameCommentWidget::applyFrameComments);
    connect(mEditor, &Editor::objectLoaded, this, &FrameCommentWidget::fillFrameComments);
    connect(mEditor->playback(), &PlaybackManager::playStateChanged, this, &FrameCommentWidget::playStateChanged);
}

void FrameCommentWidget::disconnectAll()
{
    disconnect(ui->leDialogue, &QLineEdit::textChanged, this, &FrameCommentWidget::dialogueTextChanged);
    disconnect(ui->leAction, &QLineEdit::textChanged, this, &FrameCommentWidget::actionTextChanged);
    disconnect(ui->leNotes, &QLineEdit::textChanged, this, &FrameCommentWidget::notesTextChanged);
    disconnect(mEditor, &Editor::currentFrameChanged, this, &FrameCommentWidget::currentFrameChanged);
    disconnect(mEditor->layers(), &LayerManager::currentLayerChanged, this, &FrameCommentWidget::currentLayerChanged);
    disconnect(ui->btnClearFields, &QPushButton::clicked, this, &FrameCommentWidget::clearFrameCommentsFields);
    disconnect(ui->btnApplyComments, &QPushButton::clicked, this, &FrameCommentWidget::applyFrameComments);
    disconnect(mEditor, &Editor::objectLoaded, this, &FrameCommentWidget::fillFrameComments);
    disconnect(mEditor->playback(), &PlaybackManager::playStateChanged, this, &FrameCommentWidget::playStateChanged);
}

