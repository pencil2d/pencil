#ifndef MOVIEEXPORTER_H
#define MOVIEEXPORTER_H

#include <functional>
#include <QString>
#include <QSize>
#include <QTemporaryDir>
#include "pencilerror.h"

class Object;

struct ExportMovieDesc
{
	QString strFileName;
	int     startFrame = 0;
	int     endFrame   = 0;
	//int     videoFps   = 30;
	int     fps        = 12;
	QSize   exportSize{ 0, 0 };
	QString strCameraName;
};

class MovieExporter
{
public:
    MovieExporter();
	~MovieExporter();
	
	Status run( const Object* obj, 
				const ExportMovieDesc& desc, 
				std::function<void(float)> progress );
	QString error();

	void cancel() { mCanceled = true; }

private:
	Status assembleAudio( const Object* obj, QString ffmpegPath, std::function<void( float )> progress );
	Status generateVideo( const Object* obj, std::function<void(float)> progress );
	Status combineVideoAndAudio( QString ffmpegPath, QString strOutputFile );
    Status secondPassEncoding( QString ffmpeg, QString strIn, QString strOut );
    
	QString setupTempWorkDir( const QString& strOutFile );

    QTemporaryDir mTempDir;
	QString mTempWorkDir;
	ExportMovieDesc mDesc;
	bool mCanceled = false;
};

#endif // MOVIEEXPORTER_H