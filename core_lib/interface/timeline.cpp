/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QSplitter>

#include "editor.h"
#include "layermanager.h"
#include "toolbox.h"
#include "timecontrols.h"
#include "timelinecells.h"
#include "timeline.h"

TimeLine::TimeLine( QWidget* parent ) : BaseDockWidget( parent, Qt::Tool )
{
}

void TimeLine::initUI()
{
    setFocusPolicy( Qt::NoFocus );

    QWidget* timeLineContent = new QWidget( this );

    LayerManager* pLayerManager = core()->layerManager();
    connect( pLayerManager, &LayerManager::currentKeyFrameChanged, this, &TimeLine::updateFrame );

    list = new TimeLineCells( this, core(), TIMELINE_CELL_TYPE::Layers );
    cells = new TimeLineCells( this, core(), TIMELINE_CELL_TYPE::Tracks );

    connect( list, SIGNAL( mouseMovedY( int ) ), list, SLOT( setMouseMoveY( int ) ) );
    connect( list, SIGNAL( mouseMovedY( int ) ), cells, SLOT( setMouseMoveY( int ) ) );

    numberOfLayers = 0;
    hScrollBar = new QScrollBar( Qt::Horizontal );
    vScrollBar = new QScrollBar( Qt::Vertical );
    vScrollBar->setMinimum( 0 );
    vScrollBar->setMaximum( 1 );
    vScrollBar->setPageStep( 1 );
    updateLength( getFrameLength() );

    QWidget* leftWidget = new QWidget();
    leftWidget->setMinimumWidth( 120 );
    QWidget* rightWidget = new QWidget();

    QWidget* leftToolBar = new QWidget();
    leftToolBar->setFixedHeight( 31 );
    QWidget* rightToolBar = new QWidget();
    rightToolBar->setFixedHeight( 31 );

    // --- left widget ---
    // --------- layer buttons ---------
    QToolBar* layerButtons = new QToolBar( this );
    QLabel* layerLabel = new QLabel( tr( "Layers:" ) );
    layerLabel->setIndent( 5 );
    layerLabel->setFont( QFont( "Helvetica", 10 ) );

    QToolButton* addLayerButton = new QToolButton( this );
    addLayerButton->setIcon( QIcon( ":icons/add.png" ) );
    addLayerButton->setToolTip( "Add Layer" );
    addLayerButton->setFixedSize( 24, 24 );

    QToolButton* removeLayerButton = new QToolButton( this );
    removeLayerButton->setIcon( QIcon( ":icons/remove.png" ) );
    removeLayerButton->setToolTip( "Remove Layer" );
    removeLayerButton->setFixedSize( 24, 24 );

    layerButtons->addWidget( layerLabel );
    layerButtons->addWidget( addLayerButton );
    layerButtons->addWidget( removeLayerButton );

    QHBoxLayout* leftToolBarLayout = new QHBoxLayout();
    leftToolBarLayout->setAlignment( Qt::AlignLeft );
    leftToolBarLayout->setMargin( 0 );
    leftToolBarLayout->addWidget( layerButtons );
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
    leftLayout->addWidget( list, 1, 0 );
    leftLayout->setMargin( 0 );
    leftLayout->setSpacing( 0 );
    leftWidget->setLayout( leftLayout );

    // --- right widget ---
    // --------- key buttons ---------
    QToolBar* keyButtons = new QToolBar( this );
    QLabel* keyLabel = new QLabel( tr( "Keys:" ) );
    keyLabel->setFont( QFont( "Helvetica", 10 ) );
    keyLabel->setIndent( 5 );

    QToolButton* addKeyButton = new QToolButton( this );
    addKeyButton->setIcon( QIcon( ":icons/add.png" ) );
    addKeyButton->setToolTip( "Add Frame" );
    addKeyButton->setFixedSize( 24, 24 );

    QToolButton* removeKeyButton = new QToolButton( this );
    removeKeyButton->setIcon( QIcon( ":icons/remove.png" ) );
    removeKeyButton->setToolTip( "Remove Frame" );
    removeKeyButton->setFixedSize( 24, 24 );

    QToolButton* duplicateKeyButton = new QToolButton( this );
    duplicateKeyButton->setIcon( QIcon( ":icons/controls/duplicate.png" ) );
    duplicateKeyButton->setToolTip( "Duplicate Frame" );
    duplicateKeyButton->setFixedSize( 24, 24 );
    keyButtons->addWidget( keyLabel );
    keyButtons->addWidget( addKeyButton );
    keyButtons->addWidget( removeKeyButton );
    keyButtons->addWidget( duplicateKeyButton );

    // --------- Time controls ---------
    timeControls = new TimeControls( this );

    QHBoxLayout* rightToolBarLayout = new QHBoxLayout();
    rightToolBarLayout->addWidget( keyButtons );
    rightToolBarLayout->addStretch( 1 );
    rightToolBarLayout->addWidget( timeControls );
    rightToolBarLayout->setMargin( 0 );
    rightToolBarLayout->setSpacing( 0 );
    rightToolBar->setLayout( rightToolBarLayout );

    QGridLayout* rightLayout = new QGridLayout();
    rightLayout->addWidget( rightToolBar, 0, 0 );
    rightLayout->addWidget( cells, 1, 0 );
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
    lay->addWidget( vScrollBar, 0, 1 );
    lay->addWidget( hScrollBar, 1, 0 );
    lay->setMargin( 0 );
    lay->setSpacing( 0 );
    timeLineContent->setLayout( lay );
    setWidget( timeLineContent );

    setWindowFlags( Qt::WindowStaysOnTopHint );
    setWindowTitle( "Timeline" );

    connect( this, SIGNAL( lengthChange( QString ) ), cells, SLOT( lengthChange( QString ) ) );
    connect( this, SIGNAL( fontSizeChange( int ) ), cells, SLOT( fontSizeChange( int ) ) );
    connect( this, SIGNAL( frameSizeChange( int ) ), cells, SLOT( frameSizeChange( int ) ) );
    connect( this, SIGNAL( labelChange( int ) ), cells, SLOT( labelChange( int ) ) );
    connect( this, SIGNAL( scrubChange( int ) ), cells, SLOT( scrubChange( int ) ) );

    connect( hScrollBar, SIGNAL( valueChanged( int ) ), cells, SLOT( hScrollChange( int ) ) );
    connect( vScrollBar, SIGNAL( valueChanged( int ) ), cells, SLOT( vScrollChange( int ) ) );
    connect( vScrollBar, SIGNAL( valueChanged( int ) ), list, SLOT( vScrollChange( int ) ) );

    connect( addKeyButton, SIGNAL( clicked() ), this, SIGNAL( addKeyClick() ) );
    connect( removeKeyButton, SIGNAL( clicked() ), this, SIGNAL( removeKeyClick() ) );
    connect( duplicateKeyButton, SIGNAL( clicked() ), this, SIGNAL( duplicateKeyClick() ) );

    connect( timeControls, SIGNAL( playClick() ), this, SIGNAL( playClick() ) );
    connect( timeControls, SIGNAL( endClick() ), this, SIGNAL( endplayClick() ) );
    connect( timeControls, SIGNAL( startClick() ), this, SIGNAL( startplayClick() ) );
    connect( timeControls, SIGNAL( loopClick( bool ) ), this, SIGNAL( loopClick( bool ) ) );

    connect( timeControls, SIGNAL( loopControlClick( bool ) ), this, SIGNAL( loopControlClick( bool ) ) );//adding loopControl
    connect( timeControls, SIGNAL( loopStartClick( int ) ), this, SIGNAL( loopStartClick( int ) ) );
    connect( timeControls, SIGNAL( loopEndClick( int ) ), this, SIGNAL( loopEndClick( int ) ) );

    connect( timeControls, SIGNAL( soundClick( bool ) ), this, SIGNAL( soundClick( bool ) ) );
    connect( timeControls, SIGNAL( fpsClick( int ) ), this, SIGNAL( fpsClick( int ) ) );

    connect( this, &TimeLine::loopToggled, timeControls, &TimeControls::loopToggled );
    connect( this, &TimeLine::loopControlClick, timeControls, &TimeControls::toggleLoopControl );

    connect( newBitmapLayerAct, SIGNAL( triggered() ), this, SIGNAL( newBitmapLayer() ) );
    connect( newVectorLayerAct, SIGNAL( triggered() ), this, SIGNAL( newVectorLayer() ) );
    connect( newSoundLayerAct, SIGNAL( triggered() ), this, SIGNAL( newSoundLayer() ) );
    connect( newCameraLayerAct, SIGNAL( triggered() ), this, SIGNAL( newCameraLayer() ) );
    connect( removeLayerButton, SIGNAL( clicked() ), this, SIGNAL( deleteCurrentLayer() ) );

    scrubbing = false;
}

void TimeLine::updateUI()
{

}

int TimeLine::getFrameLength()
{
    return cells->getFrameLength();
}

void TimeLine::resizeEvent(QResizeEvent*)
{
    updateLayerView();
}

void TimeLine::updateFrame(int frameNumber)
{
    if ( cells )
    {
        cells->updateFrame( m_lastUpdatedFrame );
        cells->updateFrame( frameNumber );
    }
    m_lastUpdatedFrame = frameNumber;
}

void TimeLine::updateLayerView()
{
    vScrollBar->setPageStep( (height()-cells->getOffsetY()-hScrollBar->height())/cells->getLayerHeight() -2 );
    vScrollBar->setMinimum( 0 );
    vScrollBar->setMaximum( qMax(0, numberOfLayers - vScrollBar->pageStep()) );
    update();
    updateContent();
}

void TimeLine::updateLayerNumber(int numberOfLayers)
{
    this->numberOfLayers = numberOfLayers;
    updateLayerView();
}

void TimeLine::updateLength(int frameLength)
{
    hScrollBar->setMaximum( frameLength );
}

void TimeLine::updateContent()
{
    list->updateContent();
    cells->updateContent();
    update();
}

void TimeLine::setFps ( int value )
{
    timeControls->setFps(value);
}

void TimeLine::forceUpdateLength(QString newLength)
{
    bool ok;
    int dec = newLength.toInt(&ok, 10);

    if ( dec > getFrameLength())
    {
        updateLength(dec);
        updateContent();
        QSettings settings("Pencil","Pencil");
        settings.setValue("length", dec);
    }
}


