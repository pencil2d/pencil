/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "timeline.h"

#include <QImage>
#include <QPoint>
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
#include "toolbox.h"
#include "timecontrols.h"
#include "timelinecells.h"


TimeLine::TimeLine( QWidget* parent ) : BaseDockWidget( parent, Qt::Tool )
{
}

void TimeLine::initUI()
{
    Q_ASSERT( editor() != nullptr );

    setWindowTitle( tr( "Timeline" ) );

    QWidget* timeLineContent = new QWidget( this );

    mLayerList = new TimeLineCells( this, editor(), TIMELINE_CELL_TYPE::Layers );
    mTracks = new TimeLineCells( this, editor(), TIMELINE_CELL_TYPE::Tracks );

    mHScrollbar = new QScrollBar( Qt::Horizontal );
    mVScrollbar = new QScrollBar( Qt::Vertical );
    mVScrollbar->setMinimum( 0 );
    mVScrollbar->setMaximum( 1 );
    mVScrollbar->setPageStep( 1 );

    QWidget* leftWidget = new QWidget();
    leftWidget->setMinimumWidth( 120 );
    QWidget* rightWidget = new QWidget();

    QWidget* leftToolBar = new QWidget();
    leftToolBar->setFixedHeight( 30 );
    QWidget* rightToolBar = new QWidget();
    rightToolBar->setFixedHeight( 30 );

    // --- left widget ---
    // --------- layer buttons ---------
    QToolBar* T_LayerButtons = new QToolBar( this );
    QLabel* layerLabel = new QLabel( tr( "Layers:" ) );
    layerLabel->setIndent( 5 );
    layerLabel->setFont( QFont( "Helvetica", 10 ) );

    QToolButton* addLayerButton = new QToolButton( this );
    addLayerButton->setIcon( QIcon( ":icons/add.png" ) );
    addLayerButton->setToolTip( tr("Add Layer") );
    addLayerButton->setFixedSize( 24, 24 );

    QToolButton* removeLayerButton = new QToolButton( this );
    removeLayerButton->setIcon( QIcon( ":icons/remove.png" ) );
    removeLayerButton->setToolTip( tr("Remove Layer") );
    removeLayerButton->setFixedSize( 24, 24 );

    T_LayerButtons->addWidget( layerLabel );
    T_LayerButtons->addWidget( addLayerButton );
    T_LayerButtons->addWidget( removeLayerButton );
    T_LayerButtons->setFixedHeight(30);

    QHBoxLayout* leftToolBarLayout = new QHBoxLayout();
    leftToolBarLayout->setMargin( 0 );
    leftToolBarLayout->addWidget( T_LayerButtons );
    leftToolBar->setLayout( leftToolBarLayout );

    QAction* newBitmapLayerAct = new QAction( QIcon( ":icons/layer-bitmap.png" ), tr( "New Bitmap Layer" ), this );
    QAction* newVectorLayerAct = new QAction( QIcon( ":icons/layer-vector.png" ), tr( "New Vector Layer" ), this );
    QAction* newSoundLayerAct = new QAction( QIcon( ":icons/layer-sound.png" ), tr( "New Sound Layer" ), this );
    QAction* newCameraLayerAct = new QAction( QIcon( ":icons/layer-camera.png" ), tr( "New Camera Layer" ), this );

    QMenu* layerMenu = new QMenu( tr( "&Layer" ), this );
    layerMenu->addAction( newBitmapLayerAct );
    layerMenu->addAction( newVectorLayerAct );
    layerMenu->addAction( newSoundLayerAct );
    layerMenu->addAction( newCameraLayerAct );
    addLayerButton->setMenu( layerMenu );
    addLayerButton->setPopupMode( QToolButton::InstantPopup );

    QGridLayout* leftLayout = new QGridLayout();
    leftLayout->addWidget( leftToolBar, 0, 0 );
    leftLayout->addWidget( mLayerList, 1, 0 );
    leftLayout->setMargin( 0 );
    leftLayout->setSpacing( 0 );
    leftWidget->setLayout( leftLayout );

    // --- right widget ---
    // --------- key buttons ---------
    QToolBar* T_timelineButtons = new QToolBar( this );
    QLabel* keyLabel = new QLabel( tr( "Keys:" ) );
    keyLabel->setFont( QFont( "Helvetica", 10 ) );

    QToolButton* addKeyButton = new QToolButton( this );
    addKeyButton->setIcon( QIcon( ":icons/add.png" ) );
    addKeyButton->setToolTip( tr("Add Frame") );
    addKeyButton->setFixedSize( 24, 24 );

    QToolButton* removeKeyButton = new QToolButton( this );
    removeKeyButton->setIcon( QIcon( ":icons/remove.png" ) );
    removeKeyButton->setToolTip( tr("Remove Frame") );
    removeKeyButton->setFixedSize( 24, 24 );

    QToolButton* duplicateKeyButton = new QToolButton( this );
    duplicateKeyButton->setIcon( QIcon( ":icons/controls/duplicate.png" ) );
    duplicateKeyButton->setToolTip( tr("Duplicate Frame") );
    duplicateKeyButton->setFixedSize( 24, 24 );

    QLabel* onionLabel = new QLabel( tr( "Onion skin:" ) );
    onionLabel->setFont( QFont( "Helvetica", 10 ) );

    QToolButton* onionTypeButton = new QToolButton( this );
    onionTypeButton->setIcon( QIcon( ":icons/onion_type.png" ) );
    onionTypeButton->setToolTip( tr("Toggle match keyframes") );
    onionTypeButton->setFixedSize( 24, 24 );

    T_timelineButtons->addWidget( keyLabel );
    T_timelineButtons->addWidget( addKeyButton );
    T_timelineButtons->addWidget( removeKeyButton );
    T_timelineButtons->addWidget( duplicateKeyButton );
    T_timelineButtons->addSeparator();
    T_timelineButtons->addWidget( onionLabel );
    T_timelineButtons->addWidget( onionTypeButton );
    T_timelineButtons->addSeparator();
    T_timelineButtons->setFixedHeight(30);

    // --------- Time controls ---------
    mTimeControls = new TimeControls( this );
    mTimeControls->setCore( editor() );
    mTimeControls->initUI();
    mTimeControls->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    updateLength();

    QHBoxLayout* rightToolBarLayout = new QHBoxLayout();
    rightToolBarLayout->addWidget( T_timelineButtons );
    rightToolBarLayout->setAlignment(Qt::AlignLeft);
    rightToolBarLayout->addWidget( mTimeControls );
    rightToolBarLayout->setMargin( 0 );
    rightToolBarLayout->setSpacing( 0 );
    rightToolBar->setLayout( rightToolBarLayout );

    QGridLayout* rightLayout = new QGridLayout();
    rightLayout->addWidget( rightToolBar, 0, 0 );
    rightLayout->addWidget( mTracks, 1, 0 );
    rightLayout->setMargin( 0 );
    rightLayout->setSpacing( 0 );
    rightWidget->setLayout( rightLayout );

    // --- Splitter ---
    QSplitter* splitter = new QSplitter( this );
    splitter->addWidget( leftWidget );
    splitter->addWidget( rightWidget );
    splitter->setSizes( QList<int>() << 100 << 600 );


    QGridLayout* lay = new QGridLayout();
    lay->addWidget( splitter, 0, 0 );
    lay->addWidget( mVScrollbar, 0, 1 );
    lay->addWidget( mHScrollbar, 1, 0 );
    lay->setMargin( 0 );
    lay->setSpacing( 0 );
    timeLineContent->setLayout( lay );
    setWidget( timeLineContent );

    setWindowFlags( Qt::WindowStaysOnTopHint );

    connect( mHScrollbar, &QScrollBar::valueChanged, mTracks, &TimeLineCells::hScrollChange );
    connect (mTracks, &TimeLineCells::offsetChanged, mHScrollbar, &QScrollBar::setValue);
    connect( mVScrollbar, &QScrollBar::valueChanged, mTracks, &TimeLineCells::vScrollChange );
    connect( mVScrollbar, &QScrollBar::valueChanged, mLayerList, &TimeLineCells::vScrollChange );

    connect( splitter, &QSplitter::splitterMoved, this, &TimeLine::updateLength );

    connect( addKeyButton,    &QToolButton::clicked, this, &TimeLine::addKeyClick );
    connect( removeKeyButton, &QToolButton::clicked, this, &TimeLine::removeKeyClick );
    connect( duplicateKeyButton, &QToolButton::clicked, this, &TimeLine::duplicateKeyClick );
    connect( onionTypeButton, &QToolButton::clicked, this, &TimeLine::toogleAbsoluteOnionClick );

    connect( mTimeControls, &TimeControls::loopStartClick, this, &TimeLine::loopStartClick );
    connect( mTimeControls, &TimeControls::loopEndClick, this, &TimeLine::loopEndClick );
    connect( mTimeControls, &TimeControls::loopStartClick, this, &TimeLine::updateLength );
    connect( mTimeControls, &TimeControls::loopEndClick, this, &TimeLine::updateLength );
    connect( mTimeControls, &TimeControls::rangeStateChange, this, &TimeLine::updateLength );

    connect( mTimeControls, &TimeControls::soundClick, this, &TimeLine::soundClick );
    connect( mTimeControls, &TimeControls::fpsClick, this, &TimeLine::fpsClick );
    connect( mTimeControls, &TimeControls::fpsClick, this, &TimeLine::updateLength );

    connect( newBitmapLayerAct, &QAction::triggered, this, &TimeLine::newBitmapLayer );
    connect( newVectorLayerAct, &QAction::triggered, this, &TimeLine::newVectorLayer );
    connect( newSoundLayerAct, &QAction::triggered, this, &TimeLine::newSoundLayer );
    connect( newCameraLayerAct, &QAction::triggered, this, &TimeLine::newCameraLayer );
    connect( removeLayerButton, &QPushButton::clicked, this, &TimeLine::deleteCurrentLayer );

    connect( mLayerList, &TimeLineCells::mouseMovedY, mLayerList, &TimeLineCells::setMouseMoveY );
    connect( mLayerList, &TimeLineCells::mouseMovedY, mTracks,    &TimeLineCells::setMouseMoveY );
    connect (mTracks, &TimeLineCells::lengthChanged, this, &TimeLine::updateLength );

    connect( editor(), &Editor::currentFrameChanged, this, &TimeLine::updateFrame );

    LayerManager* layer = editor()->layers();
    connect( layer, &LayerManager::layerCountChanged, this, &TimeLine::updateLayerNumber );

    scrubbing = false;
}

void TimeLine::updateUI()
{
    mTracks->update();
    mLayerList->update();
}

int TimeLine::getFrameLength()
{
    return mTracks->getFrameLength();
}

void TimeLine::resizeEvent(QResizeEvent*)
{
    updateLayerView();
}

void TimeLine::wheelEvent(QWheelEvent* event)
{
    if( event->modifiers() & Qt::ShiftModifier )
    {
        mHScrollbar->event(event);
    }
    else
    {
        mVScrollbar->event(event);
        /*QPoint numPixels = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;
        int isForward =0;
        if ( !numPixels.isNull() )
        {
            if ( numPixels.ry() > 0 )
                isForward =1;
            else if ( numPixels.ry() < 0 )
                isForward =-1;
        }
        else if (!numDegrees.isNull())
        {
            if ( numDegrees.ry() > 0 )
                isForward =1;
            else if ( numDegrees.ry() < 0 )
                isForward =-1;
        }

        if ( isForward > 0 )
        {
            mVScrollbar->triggerAction( QAbstractSlider::SliderSingleStepAdd );
        }
        else if ( isForward < 0 )
        {
            mVScrollbar->triggerAction( QAbstractSlider::SliderSingleStepSub );
        }
        else
        {
            //Do nothing we've had a wheel event where we are neither going forward or backward
            //which should never happen?
        }

        event->accept();*/
    }
}

void TimeLine::deleteCurrentLayer()
{
    QString strLayerName = editor()->layers()->currentLayer()->name();

    int ret = QMessageBox::warning( this,
                                    tr( "Warning" ),
                                    tr( "Are you sure you want to delete layer: " ) + strLayerName + " ?",
                                    QMessageBox::Ok | QMessageBox::Cancel,
                                    QMessageBox::Ok );
    if ( ret == QMessageBox::Ok )
    {
        editor()->layers()->deleteCurrentLayer();
    }
}

void TimeLine::updateFrame( int frameNumber )
{
    Q_ASSERT ( mTracks );

    mTracks->updateFrame( mLastUpdatedFrame );
    mTracks->updateFrame( frameNumber );

    mLastUpdatedFrame = frameNumber;
}

void TimeLine::updateLayerView()
{
    int pageDisplay = ( mTracks->height() - mTracks->getOffsetY() ) / mTracks->getLayerHeight();

    mVScrollbar->setMinimum( 0 );
    mVScrollbar->setMaximum( qMax(0, qMax( 0, mNumLayers - pageDisplay ) ) );
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
    int frameLength = getFrameLength();
    mHScrollbar->setMaximum( qMax( 0, frameLength - mTracks->width() / mTracks->getFrameSize() ) );
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

void TimeLine::setLoop( bool loop )
{
    mTimeControls->toggleLoop(loop);
}

void TimeLine::setPlaying( bool isPlaying )
{
    mTimeControls->updatePlayState();
}

void TimeLine::setRangeState( bool range )
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
