/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

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
#include "pencildef.h"
#include "objectdata.h"

class QProgressDialog;
class LayerBitmap;
class LayerVector;
class LayerCamera;
class LayerSound;
class ObjectData;

#define ProgressCallback std::function<void(float)>


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


class Object : public QObject
{
    Q_OBJECT    

public:
    Object( QObject* parent = nullptr );
    virtual ~Object();

    void init();
    void createWorkingDir();
    void deleteWorkingDir() const;
    void createDefaultLayers();

    QString filePath() const { return mFilePath; }
    void    setFilePath( QString strFileName ) { mFilePath = strFileName; }
    
    QString workingDir() const { return mWorkingDirPath; }
    //void    setWorkingDir( QString dirPath ) { mWorkingDirPath = dirPath; }

    QString dataDir() const { return mDataDirPath; }
    void    setDataDir( QString dirPath ) { mDataDirPath = dirPath; }

    QString mainXMLFile() const { return mMainXMLFile; }
    void    setMainXMLFile( QString file ){ mMainXMLFile = file; }

    QDomElement saveXML( QDomDocument& doc );
	bool loadXML( QDomElement element, ProgressCallback progress = [] (float){} );

    void paintImage( QPainter& painter, int frameNumber, bool background, bool antialiasing ) const;

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

	int  getLayerCount() const ;

    Layer* getLayer( int i ) const;
	Layer* findLayerByName( QString strName, Layer::LAYER_TYPE type = Layer::UNDEFINED ) const;

	bool moveLayer( int i, int j );
    void deleteLayer( int i );
    void deleteLayer( Layer* );
	
	template< typename T >
	std::vector< T* > getLayersByType() const
	{
		std::vector< T* > result;
		for ( Layer* layer : mLayers )
		{
			T* t = dynamic_cast<T*>( layer );
			if ( t )
			    result.push_back( t );
		}
		return result;
	}

    // these functions need to be moved to somewhere...
    bool exportFrames( int frameStart, int frameEnd, LayerCamera* cameraLayer, QSize exportSize, QString filePath, QString format, bool transparency, bool antialiasing, QProgressDialog* progress, int progressMax );
    bool exportX( int frameStart, int frameEnd, QTransform view, QSize exportSize, QString filePath, bool antialiasing );
    bool exportIm(int frameStart, QTransform view, QSize cameraSize, QSize exportSize, QString filePath, QString format, bool antialiasing , bool transparency);

    void modification() { modified = true; }
    bool isModified() { return modified; }
    void setModified( bool b ) { modified = b; }

    int getUniqueLayerID();

    ObjectData* data();
    void setData( ObjectData* );

    void setLayerUpdated(int layerId);

Q_SIGNALS:
    void layerChanged( int layerId );

private:
    int getMaxLayerID();

    QString mFilePath;       //< where this object come from. (empty if new project)
    QString mWorkingDirPath; //< the folder that pclx will uncompress to.
    QString mDataDirPath;    //< the folder which contains all bitmap & vector image & sound files.
    QString mMainXMLFile;    //< the location of main.xml

    QList<Layer*> mLayers;
    bool modified = false;

    QList<ColourRef> mPalette;

    std::unique_ptr<ObjectData> mData;
};


#endif
