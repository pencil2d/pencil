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
#include "pencilerror.h"

class QProgressDialog;
class LayerBitmap;
class LayerVector;
class LayerCamera;
class LayerSound;
class EditorState;


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

class Object : public QObject
{
    Q_OBJECT    

public:
    Object( QObject* parent = nullptr );
    virtual ~Object();

    void init();
    void createWorkingDir();

    QString filePath() const { return mFilePath; }
    void    setFilePath( QString strFileName ) { mFilePath = strFileName; }
    
    QString workingDir() const { return mWorkingDirPath; }
    //void    setWorkingDir( QString dirPath ) { mWorkingDirPath = dirPath; }

    QString dataDir() const { return mDataDirPath; }
    void    setDataDir( QString dirPath ) { mDataDirPath = dirPath; }

    QString mainXMLFile() const { return mMainXMLFile; }
    void    setMainXMLFile( QString file ){ mMainXMLFile = file; }

    QDomElement saveXML( QDomDocument& doc );
    bool loadXML( QDomElement element );

    void paintImage( QPainter& painter, int frameNumber, bool background, bool antialiasing );

    QString copyFileToDataFolder( QString strFilePath );

    // Color palette
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

    void loadDefaultPalette();

    LayerBitmap* addNewBitmapLayer();
    LayerVector* addNewVectorLayer();
    LayerSound* addNewSoundLayer();
    LayerCamera* addNewCameraLayer();

    Layer* getLayer( int i );
    int  getLayerCount();
    bool moveLayer( int i, int j );
    void deleteLayer( int i );
    void deleteLayer( Layer* );

    //void playSoundIfAny( int frame, int fps );
    //void stopSoundIfAny();

    // these functions need to be moved to somewhere...
    bool exportFrames( int frameStart, int frameEnd, Layer* currentLayer, QSize exportSize, QString filePath, const char* format, int quality, bool transparency, bool antialiasing, QProgressDialog* progress, int progressMax );
    bool exportFrames1( ExportFrames1Parameters parameters );
    bool exportMovie( ExportMovieParameters parameters );
    bool exportX( int frameStart, int frameEnd, QTransform view, QSize exportSize, QString filePath, bool antialiasing );
    bool exportIm(int frameStart, int frameEnd, QTransform view, QSize exportSize, QString filePath, QString format, bool antialiasing , bool transparency);
    bool exportFlash( int startFrame, int endFrame, QTransform view, QSize exportSize, QString filePath, int fps, int compression );

    void modification() { modified = true; }
    bool isModified() { return modified; }
    void setModified( bool b ) { modified = b; }

    int getUniqueLayerID();

    EditorState* editorState();
    void setEditorData( EditorState* );

    void setLayerUpdated(int layerId);

Q_SIGNALS:
    void layerChanged( int layerId );

private:
    int getMaxLayerID();

    QString mFilePath;       //< where this object come from. (empty if new project)
    QString mWorkingDirPath; //< the folder that pclx will uncompress to.
    QString mDataDirPath;    //< the folder which contains all bitmap & vector image & sound files.
    QString mMainXMLFile;    //< the location of main.xml

    QList< Layer* > mLayers;
    bool modified = false;

    QList< ColourRef > mPalette;

    std::unique_ptr< EditorState > mEditorState;
};


#endif
