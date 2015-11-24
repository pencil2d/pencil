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
#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <QObject>
#include <QList>
#include <QColor>
#include "layer.h"
#include "colourref.h"

class QProgressDialog;
class LayerBitmap;
class LayerVector;
class LayerCamera;
class LayerSound;


struct ExportMovieParameters
{
    int startFrame;
    int endFrame;
    QTransform view;
    Layer* currentLayer;
    QSize exportSize;
    QString filePath;
    int fps;
    int exportFps;
    QString exportFormat;
};

struct ExportFrames1Parameters
{
    int         frameStart;
    int         frameEnd;
    QTransform  view;
    Layer*      currentLayer;
    QSize       exportSize;
    QString     filePath;
    const char* format;
    int         quality;
    bool        background;
    bool        antialiasing;
    QProgressDialog* progress;
    int         progressMax;
    int         fps;
    int         exportFps;
};


class EditorData
{
public:
    void setFps( int f ) { mFps = f; }
    int  fps() { return mFps; }

    void setCurrentLayer( int n ) { mCurrentColor = n; }
    int currentLayer() { return mCurrentLayer; }

    void setCurrentFrame( int n ) { mCurrentFrame = n; }
    int currentFrame() { return mCurrentFrame; }

    void setCurrentColor( QColor c ) { mCurrentColor = c; }
    QColor currentColor() { return mCurrentColor; }

    void setCurrentView( QTransform t ) { mCurrentView = t; }
    QTransform currentView() { return mCurrentView; }

private:
    int    mFps          = 0;
    int    mCurrentLayer = 0;
    int    mCurrentFrame = 0;
    QColor mCurrentColor{ 255, 255, 255, 255 };
    QTransform mCurrentView;
};

class Object : public QObject
{
    Q_OBJECT    

public:
    Object( QObject* parent = nullptr );
    virtual ~Object();

    void init();

    QString filePath() const { return mstrFilePath; }
    void    setFilePath( QString strFileName ) { mstrFilePath = strFileName; }
    
    QString workingDir() const { return mstrWorkingDir; }
    void    setWorkingDir( QString strWorkingDir ) { mstrWorkingDir = strWorkingDir; }

    QDomElement saveXML( QDomDocument& doc );
    bool loadXML( QDomElement element, QString dataDirPath );

    void paintImage( QPainter& painter, int frameNumber, bool background, bool antialiasing );

    ColourRef getColour( int i );
    void setColour( int index, QColor newColour )
    {
        Q_ASSERT( index >= 0 );
        mPalette[ index ].colour = newColour;
    }
    void addColour( QColor );
    void addColour( ColourRef newColour ) { mPalette.append( newColour ); }
    bool removeColour( int index );
    void renameColour( int i, QString text );
    int getColourCount() { return mPalette.size(); }
    bool importPalette( QString filePath );
    bool exportPalette( QString filePath );
    bool savePalette( QString filePath );
    bool loadPalette( QString filePath );
    void loadDefaultPalette();

    LayerBitmap* addNewBitmapLayer();
    LayerVector* addNewVectorLayer();
    LayerSound* addNewSoundLayer();
    LayerCamera* addNewCameraLayer();

    Layer* getLayer( int i );
    int  getLayerCount();
    bool moveLayer( int i, int j );
    void deleteLayer( int i );

    //void playSoundIfAny( int frame, int fps );
    //void stopSoundIfAny();

    // these functions need to be moved to somewhere...
    bool exportFrames( int frameStart, int frameEnd, Layer* currentLayer, QSize exportSize, QString filePath, const char* format, int quality, bool background, bool antialiasing, QProgressDialog* progress, int progressMax );
    bool exportFrames1( ExportFrames1Parameters parameters );
    bool exportMovie( ExportMovieParameters parameters );
    bool exportX( int frameStart, int frameEnd, QTransform view, QSize exportSize, QString filePath, bool antialiasing );
    bool exportIm( int frameStart, int frameEnd, QTransform view, QSize exportSize, QString filePath, bool antialiasing );
    bool exportFlash( int startFrame, int endFrame, QTransform view, QSize exportSize, QString filePath, int fps, int compression );

    void modification() { modified = true; }
    bool isModified() { return modified; }
    void setModified( bool b ) { modified = b; }

    int getUniqueLayerID();

    EditorData* editorData();
    void setEditorData( EditorData* );

private:
    int getMaxLayerID();

    QString mstrFilePath;
    QString mstrWorkingDir;

    QList< Layer* > mLayers;
    bool modified = false;

    QList<ColourRef> mPalette;

    std::unique_ptr< EditorData > mEditorData;
};


#endif
