/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "timeline.h"

#include <QWidget>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QSplitter>
#include <QMessageBox>
#include <QLabel>
#include <QWheelEvent>
#include <QSlider>
#include <QTimer>
#include <QScrollArea>
#include <QDebug>

#include "editor.h"
#include "layermanager.h"
#include "timecontrols.h"
#include "timelinetracklist.h"
#include "timelinelayerlist.h"
#include "timelinelayerheaderwidget.h"
#include "timelinetrackheaderwidget.h"


TimeLine::TimeLine(QWidget* parent) : BaseDockWidget(parent)
{
}

void TimeLine::initUI()
{
    Q_ASSERT(editor() != nullptr);

    setWindowTitle(tr("Timeline", "Subpanel title"));

    QWidget* timeLineContent = new QWidget(this);

    mLayerHeader = new TimeLineLayerHeaderWidget(this, editor());
    mTrackHeader = new TimeLineTrackHeaderWidget(this, editor());
    mLayerList = new TimeLineLayerList(this, editor());
    mTracks = new TimeLineTrackList(this, editor());

    mLayerHeader->setFixedHeight(mLayerList->getLayerHeight());
    mTrackHeader->setFixedHeight(mLayerList->getLayerHeight());

    mHScrollbar = new QScrollBar(Qt::Horizontal);

    QWidget* leftWidget = new QWidget();
    leftWidget->setMinimumWidth(120);
    QWidget* rightWidget = new QWidget();

    QWidget* leftToolBar = new QWidget();
    leftToolBar->setFixedHeight(30);
    QWidget* rightToolBar = new QWidget();
    rightToolBar->setFixedHeight(30);

    // --- left widget ---
    // --------- layer buttons ---------
    QToolBar* layerButtons = new QToolBar(this);
    layerButtons->setIconSize(QSize(22,22));
    QLabel* layerLabel = new QLabel(tr("Layers:"));
    layerLabel->setIndent(5);

    QToolButton* addLayerButton = new QToolButton(this);
    addLayerButton->setIcon(QIcon(":icons/themes/playful/timeline/layer-add.svg"));
    addLayerButton->setToolTip(tr("Add Layer"));

    mLayerDeleteButton = new QToolButton(this);
    mLayerDeleteButton->setIcon(QIcon(":icons/themes/playful/timeline/layer-remove.svg"));
    mLayerDeleteButton->setToolTip(tr("Delete Layer"));

    QToolButton* duplicateLayerButton = new QToolButton(this);
    duplicateLayerButton->setIcon(QIcon(":icons/themes/playful/timeline/layer-duplicate.svg"));
    duplicateLayerButton->setToolTip(tr("Duplicate Layer"));

    layerButtons->addWidget(layerLabel);
    layerButtons->addWidget(addLayerButton);
    layerButtons->addWidget(mLayerDeleteButton);
    layerButtons->addWidget(duplicateLayerButton);
    layerButtons->setFixedHeight(30);

    QHBoxLayout* leftToolBarLayout = new QHBoxLayout();
    leftToolBarLayout->setContentsMargins(0, 0, 0, 0);
    leftToolBarLayout->addWidget(layerButtons);
    leftToolBar->setLayout(leftToolBarLayout);

    QAction* newBitmapLayerAct = new QAction(QIcon(":icons/themes/playful/timeline/cell-bitmap.svg"), tr("New Bitmap Layer"), this);
    QAction* newVectorLayerAct = new QAction(QIcon(":icons/themes/playful/timeline/cell-vector.svg"), tr("New Vector Layer"), this);
    QAction* newSoundLayerAct = new QAction(QIcon(":icons/themes/playful/timeline/cell-sound.svg"), tr("New Sound Layer"), this);
    QAction* newCameraLayerAct = new QAction(QIcon(":icons/themes/playful/timeline/cell-camera.svg"), tr("New Camera Layer"), this);

    QMenu* layerMenu = new QMenu(tr("Layer", "Timeline add-layer menu"), this);
    layerMenu->addAction(newBitmapLayerAct);
    layerMenu->addAction(newVectorLayerAct);
    layerMenu->addAction(newSoundLayerAct);
    layerMenu->addAction(newCameraLayerAct);
    addLayerButton->setMenu(layerMenu);
    addLayerButton->setPopupMode(QToolButton::InstantPopup);

    mLayerScrollArea = new QScrollArea();
    mLayerScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mLayerScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mLayerScrollArea->setWidgetResizable(true);
    mLayerScrollArea->setWidget(mLayerList);
    mLayerScrollArea->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    mLayerScrollArea->horizontalScrollBar()->setEnabled(false);

    mTrackScrollArea = new QScrollArea();
    mTrackScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mTrackScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mTrackScrollArea->setWidgetResizable(true);
    mTrackScrollArea->setWidget(mTracks);
    mTrackScrollArea->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    mTrackScrollArea->horizontalScrollBar()->setEnabled(false);

    QGridLayout* leftLayout = new QGridLayout();
    leftLayout->addWidget(leftToolBar, 0, 0);
    leftLayout->addWidget(mLayerHeader, 1, 0);
    leftLayout->addWidget(mLayerScrollArea, 2, 0);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    leftWidget->setLayout(leftLayout);

    // --- right widget ---
    // --------- key buttons ---------
    QToolBar* timelineButtons = new QToolBar(this);
    timelineButtons->setIconSize(QSize(22,22));
    QLabel* keyLabel = new QLabel(tr("Keys:"));
    keyLabel->setIndent(5);

    QToolButton* addKeyButton = new QToolButton(this);
    addKeyButton->setIcon(QIcon(":icons/themes/playful/timeline/frame-add.svg"));
    addKeyButton->setToolTip(tr("Add Frame"));

    QToolButton* removeKeyButton = new QToolButton(this);
    removeKeyButton->setIcon(QIcon(":icons/themes/playful/timeline/frame-remove.svg"));
    removeKeyButton->setToolTip(tr("Remove Frame"));

    QToolButton* duplicateKeyButton = new QToolButton(this);
    duplicateKeyButton->setIcon(QIcon(":icons/themes/playful/timeline/frame-duplicate.svg"));
    duplicateKeyButton->setToolTip(tr("Duplicate Frame"));

    QLabel* zoomLabel = new QLabel(tr("Zoom:"));
    zoomLabel->setIndent(5);

    QSlider* zoomSlider = new QSlider(this);
    zoomSlider->setRange(4, 40);
    zoomSlider->setFixedWidth(74);
    zoomSlider->setValue(mTracks->getFrameSize());
    zoomSlider->setToolTip(tr("Adjust frame width"));
    zoomSlider->setOrientation(Qt::Horizontal);
    zoomSlider->setFocusPolicy(Qt::TabFocus);

    timelineButtons->addWidget(keyLabel);
    timelineButtons->addWidget(addKeyButton);
    timelineButtons->addWidget(removeKeyButton);
    timelineButtons->addWidget(duplicateKeyButton);
    timelineButtons->addSeparator();
    timelineButtons->addWidget(zoomLabel);
    timelineButtons->addWidget(zoomSlider);
    timelineButtons->setFixedHeight(30);

    // --------- Time controls ---------
    mTimeControls = new TimeControls(this);
    mTimeControls->setIconSize(QSize(22,22));
    mTimeControls->setEditor(editor());
    mTimeControls->initUI();
    updateLength();

    QHBoxLayout* rightToolBarLayout = new QHBoxLayout();
    rightToolBarLayout->addWidget(timelineButtons);
    rightToolBarLayout->setAlignment(Qt::AlignLeft);
    rightToolBarLayout->addWidget(mTimeControls);
    rightToolBarLayout->setContentsMargins(0, 0, 0, 0);
    rightToolBarLayout->setSpacing(0);
    rightToolBar->setLayout(rightToolBarLayout);

    QGridLayout* rightLayout = new QGridLayout();
    rightLayout->addWidget(rightToolBar, 0, 0);
    rightLayout->addWidget(mTrackHeader, 1, 0);
    rightLayout->addWidget(mTrackScrollArea, 2, 0);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    rightWidget->setLayout(rightLayout);

    // --- Splitter ---
    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setSizes(QList<int>() << 100 << 600);


    QGridLayout* lay = new QGridLayout();
    lay->addWidget(splitter, 0, 0);
    lay->addWidget(mHScrollbar, 1, 0);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    timeLineContent->setLayout(lay);
    setWidget(timeLineContent);

    mScrollingStoppedTimer = new QTimer();
    mScrollingStoppedTimer->setSingleShot(true);

    mLayerManager = editor()->layers();

    setWindowFlags(Qt::WindowStaysOnTopHint);

    connect(mHScrollbar, &QScrollBar::valueChanged, mTracks, &TimeLineTrackList::hScrollChange);
    connect(mHScrollbar, &QScrollBar::valueChanged, mTrackHeader, &TimeLineTrackHeaderWidget::onHScrollChange);

    connect(mTracks, &TimeLineTrackList::offsetChanged, mHScrollbar, &QScrollBar::setValue);

    connect(mLayerScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, mTracks, &TimeLineTrackList::vScrollChange);
    connect(mLayerScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &TimeLine::onScrollbarValueChanged);

    connect(mTrackScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, mTracks, &TimeLineTrackList::vScrollChange);
    connect(mTrackScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &TimeLine::onScrollbarValueChanged);

    connect(splitter, &QSplitter::splitterMoved, this, &TimeLine::updateLength);

    connect(addKeyButton, &QToolButton::clicked, this, &TimeLine::insertKeyClick);
    connect(removeKeyButton, &QToolButton::clicked, this, &TimeLine::removeKeyClick);
    connect(duplicateLayerButton, &QToolButton::clicked, this , &TimeLine::duplicateLayerClick);
    connect(duplicateKeyButton, &QToolButton::clicked, this, &TimeLine::duplicateKeyClick);
    connect(zoomSlider, &QSlider::valueChanged, mTracks, &TimeLineTrackList::setFrameSize);

    connect(mTimeControls, &TimeControls::soundToggled, this, &TimeLine::soundClick);
    connect(mTimeControls, &TimeControls::fpsChanged, this, &TimeLine::fpsChanged);
    connect(mTimeControls, &TimeControls::fpsChanged, this, &TimeLine::updateLength);
    connect(mTimeControls, &TimeControls::playButtonTriggered, this, &TimeLine::playButtonTriggered);
    connect(editor(), &Editor::scrubbed, mTimeControls, &TimeControls::updateTimecodeLabel);
    connect(mTimeControls, &TimeControls::fpsChanged, mTimeControls, &TimeControls::setFps);
    connect(this, &TimeLine::fpsChanged, mTimeControls, &TimeControls::setFps);

    connect(newBitmapLayerAct, &QAction::triggered, this, &TimeLine::newBitmapLayer);
    connect(newVectorLayerAct, &QAction::triggered, this, &TimeLine::newVectorLayer);
    connect(newSoundLayerAct, &QAction::triggered, this, &TimeLine::newSoundLayer);
    connect(newCameraLayerAct, &QAction::triggered, this, &TimeLine::newCameraLayer);
    connect(mLayerDeleteButton, &QPushButton::clicked, this, &TimeLine::deleteCurrentLayerClick);

    connect(mLayerList, &TimeLineLayerList::cellDraggedY, mTracks, &TimeLineTrackList::setCellDragY);
    connect(mTracks, &TimeLineTrackList::lengthChanged, this, &TimeLine::updateLength);
    connect(mTracks, &TimeLineTrackList::selectionChanged, this, &TimeLine::selectionChanged);
    connect(mTracks, &TimeLineTrackList::insertNewKeyFrame, this, &TimeLine::insertKeyClick);

    connect(editor(), &Editor::scrubbed, this, &TimeLine::updateFrame);
    connect(editor(), &Editor::frameModified, this, &TimeLine::updateContent);
    connect(editor(), &Editor::framesModified, this, &TimeLine::updateContent);

    connect(mLayerManager, &LayerManager::layerCountChanged, this, &TimeLine::onLayerCountUpdated);
    connect(mLayerManager, &LayerManager::layerOrderChanged, this, &TimeLine::onLayerOrderUpdated);
    connect(mLayerManager, &LayerManager::currentLayerChanged, this, &TimeLine::onCurrentLayerChanged);

    mLayerList->loadLayerCells();

    scrubbing = false;
}

void TimeLine::updateUI()
{
    updateContent();
}

void TimeLine::updateUICached()
{
    mLayerList->update();
    mTracks->update();
    mLayerHeader->update();
    mTrackHeader->update();
}

/** Extends the timeline frame length if necessary
 *
 *  @param[in] frame The new animation length
 */
void TimeLine::extendLength(int frame)
{
    int currentLength = mTracks->getFrameLength();
    if(frame > (currentLength * 0.75))
    {
        int newLength = static_cast<int>(std::max(frame, currentLength) * 1.5);

        if (newLength > 9999)
            newLength = 9999;

        mTracks->setFrameLength(newLength);
        updateLength();
    }
}

void TimeLine::resizeEvent(QResizeEvent*)
{
    if (mLayerManager) {
        updateVerticalScrollbarPageCount(mLayerManager->count());
    }
}

void TimeLine::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ShiftModifier)
    {
        mHScrollbar->event(event);
    }
}

void TimeLine::onScrollbarValueChanged(int value)
{
    // After the scrollbar has been updated, prepare to trigger stopped event
    mScrollingStoppedTimer->start(150);
    mLayerScrollArea->verticalScrollBar()->setValue(value);
    mTrackScrollArea->verticalScrollBar()->setValue(value);
}

void TimeLine::updateFrame(int frameNumber)
{
    Q_ASSERT(mTracks);


    mTracks->updateFrame(mLastUpdatedFrame);
    mTracks->updateFrame(frameNumber);
    mTrackHeader->update();

    mLastUpdatedFrame = frameNumber;
}

void TimeLine::updateVerticalScrollbarPageCount(int numberOfLayers)
{
    int pageDisplay = mLayerScrollArea->height() / mLayerList->getLayerHeight();

    QScrollBar* layerScrollBar = mLayerScrollArea->verticalScrollBar();
    QScrollBar* trackScrollBar = mTrackScrollArea->verticalScrollBar();
    layerScrollBar->setRange(0, qMax(0, (numberOfLayers - pageDisplay) * mLayerList->getLayerHeight()));
    layerScrollBar->setPageStep(mLayerList->getLayerHeight());
    layerScrollBar->setSingleStep(mLayerList->getLayerHeight());

    trackScrollBar->setRange(layerScrollBar->minimum(), layerScrollBar->maximum());
    trackScrollBar->setPageStep(layerScrollBar->pageStep());
    trackScrollBar->setSingleStep(layerScrollBar->singleStep());

    updateContent();
}

void TimeLine::onLayerCountUpdated(int numberOfLayers)
{
    mLayerList->loadLayerCells();
    updateVerticalScrollbarPageCount(numberOfLayers);
}

void TimeLine::onLayerOrderUpdated()
{
    mLayerList->loadLayerCells();
}

void TimeLine::updateLength()
{
    int frameLength = mTracks->getFrameLength();
    mHScrollbar->setMaximum(qMax(0, frameLength - mTracks->width() / mTracks->getFrameSize()));
    mTimeControls->updateLength(frameLength);
    updateContent();
}

void TimeLine::updateContent()
{
    mLayerList->updateContent();
    mLayerHeader->update();
    mTracks->updateContent();
    mTrackHeader->update();
    update();
}

void TimeLine::setLoop(bool loop)
{
    mTimeControls->setLoop(loop);
}

void TimeLine::setPlaying(bool isPlaying)
{
    Q_UNUSED(isPlaying)
    mTimeControls->updatePlayState();
}

void TimeLine::setRangeState(bool range)
{
    mTimeControls->setRangeState(range);
}

int TimeLine::getRangeLower()
{
    return mTimeControls->getRangeLower();
}

int TimeLine::getRangeUpper()
{
    return mTimeControls->getRangeUpper();
}

void TimeLine::onObjectLoaded()
{
    mTimeControls->updateUI();
    mLayerList->loadLayerCells();
    onLayerCountUpdated(editor()->layers()->count());
}

void TimeLine::onCurrentLayerChanged()
{
    updateVerticalScrollbarPosition();
    mLayerDeleteButton->setEnabled(editor()->layers()->canDeleteLayer(editor()->currentLayerIndex()));
    mLayerList->updateContent();
}

void TimeLine::updateVerticalScrollbarPosition()
{
    // invert index so 0 is at the top
    QScrollBar* verticalTrackScrollBar = mTrackScrollArea->verticalScrollBar();
    int layerCount = mLayerManager->count();
    int idx = layerCount - editor()->currentLayerIndex() - 1;

    float layerHeightF = static_cast<float>(mLayerList->getLayerHeight());
    float diff = qRound(static_cast<float>(verticalTrackScrollBar->maximum()) / layerHeightF);
    int numOfVisibleCells = layerCount - diff;

    int pos = verticalTrackScrollBar->value();
    if (pos > 0) {
        float fracPosition = static_cast<float>(pos) / layerHeightF;
        pos = qRound(fracPosition);
    }

    bool aboveVisibleArea = idx < pos;
    bool belowVisibleArea = idx >= numOfVisibleCells + pos;

    if (belowVisibleArea || aboveVisibleArea) {
        int value = 0;
        if (aboveVisibleArea) {
            value = idx;
        }
        else { // belowVisibleArea
            value = (idx - numOfVisibleCells + 1);
        }
        mLayerScrollArea->verticalScrollBar()->setValue(value * mLayerList->getLayerHeight());
        mTrackScrollArea->verticalScrollBar()->setValue(value * mLayerList->getLayerHeight());
    }
}
