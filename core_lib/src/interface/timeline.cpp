/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

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

#include "layer.h"
#include "editor.h"
#include "layermanager.h"
#include "timecontrols.h"
#include "timelinecells.h"


TimeLine::TimeLine(QWidget* parent) : BaseDockWidget(parent)
{
}

void TimeLine::initUI()
{
    Q_ASSERT(editor() != nullptr);

    setWindowTitle(tr("Timeline"));

    QWidget* timeLineContent = new QWidget(this);

    mLayerList = new TimeLineCells(this, editor(), TIMELINE_CELL_TYPE::Layers);
    mTracks = new TimeLineCells(this, editor(), TIMELINE_CELL_TYPE::Tracks);

    mHScrollbar = new QScrollBar(Qt::Horizontal);
    mVScrollbar = new QScrollBar(Qt::Vertical);
    mVScrollbar->setMinimum(0);
    mVScrollbar->setMaximum(1);
    mVScrollbar->setPageStep(1);

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
    QLabel* layerLabel = new QLabel(tr("Layers:"));
    layerLabel->setIndent(5);

    QToolButton* addLayerButton = new QToolButton(this);
    addLayerButton->setIcon(QIcon(":icons/add.png"));
    addLayerButton->setToolTip(tr("Add Layer"));
    addLayerButton->setFixedSize(24, 24);

    QToolButton* removeLayerButton = new QToolButton(this);
    removeLayerButton->setIcon(QIcon(":icons/remove.png"));
    removeLayerButton->setToolTip(tr("Remove Layer"));
    removeLayerButton->setFixedSize(24, 24);

    layerButtons->addWidget(layerLabel);
    layerButtons->addWidget(addLayerButton);
    layerButtons->addWidget(removeLayerButton);
    layerButtons->setFixedHeight(30);

    QHBoxLayout* leftToolBarLayout = new QHBoxLayout();
    leftToolBarLayout->setMargin(0);
    leftToolBarLayout->addWidget(layerButtons);
    leftToolBar->setLayout(leftToolBarLayout);

    QAction* newBitmapLayerAct = new QAction(QIcon(":icons/layer-bitmap.png"), tr("New Bitmap Layer"), this);
    QAction* newVectorLayerAct = new QAction(QIcon(":icons/layer-vector.png"), tr("New Vector Layer"), this);
    QAction* newSoundLayerAct = new QAction(QIcon(":icons/layer-sound.png"), tr("New Sound Layer"), this);
    QAction* newCameraLayerAct = new QAction(QIcon(":icons/layer-camera.png"), tr("New Camera Layer"), this);

    QMenu* layerMenu = new QMenu(tr("&Layer", "Timeline add-layer menu"), this);
    layerMenu->addAction(newBitmapLayerAct);
    layerMenu->addAction(newVectorLayerAct);
    layerMenu->addAction(newSoundLayerAct);
    layerMenu->addAction(newCameraLayerAct);
    addLayerButton->setMenu(layerMenu);
    addLayerButton->setPopupMode(QToolButton::InstantPopup);

    QGridLayout* leftLayout = new QGridLayout();
    leftLayout->addWidget(leftToolBar, 0, 0);
    leftLayout->addWidget(mLayerList, 1, 0);
    leftLayout->setMargin(0);
    leftLayout->setSpacing(0);
    leftWidget->setLayout(leftLayout);

    // --- right widget ---
    // --------- key buttons ---------
    QToolBar* timelineButtons = new QToolBar(this);
    QLabel* keyLabel = new QLabel(tr("Keys:"));
    keyLabel->setIndent(5);

    QToolButton* addKeyButton = new QToolButton(this);
    addKeyButton->setIcon(QIcon(":icons/add.png"));
    addKeyButton->setToolTip(tr("Add Frame"));
    addKeyButton->setFixedSize(24, 24);

    QToolButton* removeKeyButton = new QToolButton(this);
    removeKeyButton->setIcon(QIcon(":icons/remove.png"));
    removeKeyButton->setToolTip(tr("Remove Frame"));
    removeKeyButton->setFixedSize(24, 24);

    QToolButton* duplicateKeyButton = new QToolButton(this);
    duplicateKeyButton->setIcon(QIcon(":icons/controls/duplicate.png"));
    duplicateKeyButton->setToolTip(tr("Duplicate Frame"));
    duplicateKeyButton->setFixedSize(24, 24);

    QLabel* onionLabel = new QLabel(tr("Onion skin:"));

    QToolButton* onionTypeButton = new QToolButton(this);
    onionTypeButton->setIcon(QIcon(":icons/onion_type.png"));
    onionTypeButton->setToolTip(tr("Toggle match keyframes"));
    onionTypeButton->setFixedSize(24, 24);

    timelineButtons->addWidget(keyLabel);
    timelineButtons->addWidget(addKeyButton);
    timelineButtons->addWidget(removeKeyButton);
    timelineButtons->addWidget(duplicateKeyButton);
    timelineButtons->addSeparator();
    timelineButtons->addWidget(onionLabel);
    timelineButtons->addWidget(onionTypeButton);
    timelineButtons->addSeparator();
    timelineButtons->setFixedHeight(30);

    // --------- Time controls ---------
    mTimeControls = new TimeControls(this);
    mTimeControls->setEditor(editor());
    mTimeControls->initUI();
    mTimeControls->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    updateLength();

    QHBoxLayout* rightToolBarLayout = new QHBoxLayout();
    rightToolBarLayout->addWidget(timelineButtons);
    rightToolBarLayout->setAlignment(Qt::AlignLeft);
    rightToolBarLayout->addWidget(mTimeControls);
    rightToolBarLayout->setMargin(0);
    rightToolBarLayout->setSpacing(0);
    rightToolBar->setLayout(rightToolBarLayout);

    QGridLayout* rightLayout = new QGridLayout();
    rightLayout->addWidget(rightToolBar, 0, 0);
    rightLayout->addWidget(mTracks, 1, 0);
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightWidget->setLayout(rightLayout);

    // --- Splitter ---
    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setSizes(QList<int>() << 100 << 600);


    QGridLayout* lay = new QGridLayout();
    lay->addWidget(splitter, 0, 0);
    lay->addWidget(mVScrollbar, 0, 1);
    lay->addWidget(mHScrollbar, 1, 0);
    lay->setMargin(0);
    lay->setSpacing(0);
    timeLineContent->setLayout(lay);
    setWidget(timeLineContent);

    setWindowFlags(Qt::WindowStaysOnTopHint);

    connect(mHScrollbar, &QScrollBar::valueChanged, mTracks, &TimeLineCells::hScrollChange);
    connect(mTracks, &TimeLineCells::offsetChanged, mHScrollbar, &QScrollBar::setValue);
    connect(mVScrollbar, &QScrollBar::valueChanged, mTracks, &TimeLineCells::vScrollChange);
    connect(mVScrollbar, &QScrollBar::valueChanged, mLayerList, &TimeLineCells::vScrollChange);

    connect(splitter, &QSplitter::splitterMoved, this, &TimeLine::updateLength);

    connect(addKeyButton, &QToolButton::clicked, this, &TimeLine::addKeyClick);
    connect(removeKeyButton, &QToolButton::clicked, this, &TimeLine::removeKeyClick);
    connect(duplicateKeyButton, &QToolButton::clicked, this, &TimeLine::duplicateKeyClick);
    connect(onionTypeButton, &QToolButton::clicked, this, &TimeLine::toogleAbsoluteOnionClick);

    connect(mTimeControls, &TimeControls::soundClick, this, &TimeLine::soundClick);
    connect(mTimeControls, &TimeControls::fpsClick, this, &TimeLine::fpsClick);
    connect(mTimeControls, &TimeControls::fpsClick, this, &TimeLine::updateLength);
    connect(mTimeControls, &TimeControls::playButtonTriggered, this, &TimeLine::playButtonTriggered);

    connect(newBitmapLayerAct, &QAction::triggered, this, &TimeLine::newBitmapLayer);
    connect(newVectorLayerAct, &QAction::triggered, this, &TimeLine::newVectorLayer);
    connect(newSoundLayerAct, &QAction::triggered, this, &TimeLine::newSoundLayer);
    connect(newCameraLayerAct, &QAction::triggered, this, &TimeLine::newCameraLayer);
    connect(removeLayerButton, &QPushButton::clicked, this, &TimeLine::deleteCurrentLayer);

    connect(mLayerList, &TimeLineCells::mouseMovedY, mLayerList, &TimeLineCells::setMouseMoveY);
    connect(mLayerList, &TimeLineCells::mouseMovedY, mTracks, &TimeLineCells::setMouseMoveY);
    connect(mTracks, &TimeLineCells::lengthChanged, this, &TimeLine::updateLength);

    connect(editor(), &Editor::currentFrameChanged, this, &TimeLine::updateFrame);

    LayerManager* layer = editor()->layers();
    connect(layer, &LayerManager::layerCountChanged, this, &TimeLine::updateLayerNumber);

    scrubbing = false;
}

void TimeLine::updateUI()
{
    mTracks->update();
    mLayerList->update();
}

int TimeLine::getLength()
{
    return mTracks->getFrameLength();
}

void TimeLine::setLength(int frame)
{
    mTracks->setFrameLength(frame);
    updateLength();
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
        int newLength = std::max(frame, currentLength) * 1.5;

        if (newLength > 9999)
            newLength = 9999;

        mTracks->setFrameLength(newLength);
        updateLength();
    }
}

void TimeLine::resizeEvent(QResizeEvent*)
{
    updateLayerView();
}

void TimeLine::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ShiftModifier)
    {
        mHScrollbar->event(event);
    }
    else
    {
        mVScrollbar->event(event);
    }
}

void TimeLine::deleteCurrentLayer()
{
    LayerManager* layerMgr = editor()->layers();
    QString strLayerName = layerMgr->currentLayer()->name();

    int ret = QMessageBox::warning(this,
                                   tr("Delete Layer", "Windows title of Delete current layer pop-up."),
                                   tr("Are you sure you want to delete layer: ") + strLayerName + " ?",
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok)
    {
        Status st = layerMgr->deleteLayer(editor()->currentLayerIndex());
        if (st == Status::ERROR_NEED_AT_LEAST_ONE_CAMERA_LAYER)
        {
            QMessageBox::information(this, "",
                                     tr("Please keep at least one camera layer in project"));
        }
    }
}

void TimeLine::updateFrame(int frameNumber)
{
    Q_ASSERT(mTracks);

    mTracks->updateFrame(mLastUpdatedFrame);
    mTracks->updateFrame(frameNumber);

    mLastUpdatedFrame = frameNumber;
}

void TimeLine::updateLayerView()
{
    int pageDisplay = (mTracks->height() - mTracks->getOffsetY()) / mTracks->getLayerHeight();

    mVScrollbar->setMinimum(0);
    mVScrollbar->setMaximum(qMax(0, qMax(0, mNumLayers - pageDisplay)));
    update();
    updateContent();
}

void TimeLine::updateLayerNumber(int numberOfLayers)
{
    mNumLayers = numberOfLayers;
    updateLayerView();
}

void TimeLine::updateLength()
{
    int frameLength = getLength();
    mHScrollbar->setMaximum(qMax(0, frameLength - mTracks->width() / mTracks->getFrameSize()));
    mTimeControls->updateLength(frameLength);
    update();
    updateContent();
}

void TimeLine::updateContent()
{
    mLayerList->updateContent();
    mTracks->updateContent();
    update();
}

void TimeLine::setLoop(bool loop)
{
    mTimeControls->toggleLoop(loop);
}

void TimeLine::setPlaying(bool isPlaying)
{
    Q_UNUSED(isPlaying);
    mTimeControls->updatePlayState();
}

void TimeLine::setRangeState(bool range)
{
    mTimeControls->toggleLoopControl(range);
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
}
