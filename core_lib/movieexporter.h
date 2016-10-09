#ifndef MOVIEEXPORTER_H
#define MOVIEEXPORTER_H

#include <functional>
#include <QString>
#include <QSize>
#include "pencilerror.h"

class Object;

struct ExportMovieDesc
{
	QString strFileName;
	int     startFrame = 0;
	int     endFrame   = 0;
	int     videoFps   = 30;
	int     fps        = 12;
	QSize   exportSize;
	QString strFormat;
	QString strCameraName;
};

class MovieExporter
{
public:
    MovieExporter();

	Status run( const Object* obj, 
				const ExportMovieDesc& desc, 
				std::function<void(float)> progress );
	QString error();

private:
	Status assembleAudio( const Object* obj, QString ffmpegPath );
	Status generateVideo( const Object* obj );
	Status combineVideoAndAudio( QString ffmpegPath ); 

	QString mTempWorkDir;
	ExportMovieDesc mDesc;
};

#endif // MOVIEEXPORTER_H