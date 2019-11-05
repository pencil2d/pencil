#include "framecommentwidget.h"
#include "ui_framecommentwidget.h"

#include "editor.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include "keyframe.h"
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
    updateConnections();
}

void FrameCommentWidget::updateUI()
{

}

void FrameCommentWidget::setCore(Editor *editor)
{
    mEditor = editor;
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
        if (mEditor->layers()->currentLayer()->keyExists(frame))
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
    currentFrameChanged(mEditor->currentFrame());
}

void FrameCommentWidget::clearFrameCommentsFields()
{
    ui->leDialogue->clear();
    ui->leAction->clear();
    ui->leNotes->clear();
    if (!mEditor->layers()->currentLayer()->keyExists(mEditor->currentFrame()))
        ui->labLayerFrame->setText("");
}

void FrameCommentWidget::applyFrameComments()
{
    KeyFrame* key = mEditor->layers()->currentLayer()->getKeyFrameAt(mEditor->currentFrame());
    if (key == nullptr) { return; }
    key->setDialogueComment(ui->leDialogue->text());
    key->setActionComment(ui->leAction->text());
    key->setNotesComment(ui->leNotes->text());
    mEditor->layers()->currentLayer()->setModified(key->pos(), true);
}

void FrameCommentWidget::playStateChanged(bool isPlaying)
{
    mIsPlaying = isPlaying;
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
    KeyFrame* key = mEditor->layers()->currentLayer()->getKeyFrameAt(mEditor->currentFrame());
    if (key == nullptr) { return; }
    ui->leDialogue->setText(key->getDialogueComment());
    ui->leAction->setText(key->getActionComment());
    ui->leNotes->setText(key->getNotesComment());
    ui->labLayerFrame->setText(tr("%1 #%2 :").arg(mEditor->layers()->currentLayer()->name()).arg(QString::number(mEditor->currentFrame())));
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

