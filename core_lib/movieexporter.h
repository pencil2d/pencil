#ifndef MOVIEEXPORTER_H
#define MOVIEEXPORTER_H

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

	Status run( Object* obj, const ExportMovieDesc& desc );
	QString error();

private:
	Status assembleAudio( Object* obj, QString ffmpegPath );
	Status generateVideo( Object* obj );
	Status combineVideoAndAudio( QString ffmpegPath ); 

	QString mTempWorkDir;
	ExportMovieDesc mDesc;
};

#endif // MOVIEEXPORTER_H