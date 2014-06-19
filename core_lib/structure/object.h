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
#ifndef OBJECT_H
#define OBJECT_H

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
    QMatrix view;
    Layer* currentLayer;
    QSize exportSize;
    QString filePath;
    int fps;
    int exportFps;
    QString exportFormat;
};

struct ExportFrames1Parameters
{
    int frameStart;
    int frameEnd;
    QMatrix view;
    Layer* currentLayer;
    QSize exportSize;
    QString filePath;
    const char* format;
    int quality;
    bool background;
    bool antialiasing;
    QProgressDialog* progress;
    int progressMax;
    int fps;
    int exportFps;
};

class Object : public QObject
{
    Q_OBJECT

public:
    Object( QObject* parent = nullptr );
    virtual ~Object();

    void init();

    QString filePath() { return mStrFilePath; }
    void    setFilePath( QString strFileName ) { mStrFilePath = strFileName; }

    QDomElement createDomElement( QDomDocument& doc );
    bool loadDomElement( QDomElement element, QString dataDirPath );

    bool read( QString filePath );
    bool write( QString filePath );

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
    int getLayerCount();
    int getMaxID();
    void moveLayer( int i, int j );
    void deleteLayer( int i );

    void playSoundIfAny( int frame, int fps );
    void stopSoundIfAny();

    bool exportFrames( int frameStart, int frameEnd, Layer* currentLayer, QSize exportSize, QString filePath, const char* format, int quality, bool background, bool antialiasing, QProgressDialog* progress, int progressMax );
    bool exportFrames1( ExportFrames1Parameters parameters );
    bool exportMovie( ExportMovieParameters parameters );
    bool exportX( int frameStart, int frameEnd, QMatrix view, QSize exportSize, QString filePath, bool antialiasing );
    bool exportIm( int frameStart, int frameEnd, QMatrix view, QSize exportSize, QString filePath, bool antialiasing );
    bool exportFlash( int startFrame, int endFrame, QMatrix view, QSize exportSize, QString filePath, int fps, int compression );

    void modification() { modified = true; }
    void toggleMirror() { mMirror = !mMirror; }
    void resetMirror() { mMirror = false; }

    bool isModified() { return modified; }
    void setModified( bool b ) { modified = b; }

private:
    QString mStrFilePath;
    QList<Layer*> mLayers;
    bool modified = false;

    QList<ColourRef> mPalette;

    bool mMirror; // if true, the returned image is flipped horizontally
};

#endif
