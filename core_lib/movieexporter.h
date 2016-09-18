#ifndef MOVIEEXPORTER_H
#define MOVIEEXPORTER_H

#include <QString>
#include <QSize>
#include "pencilerror.h"

class Object;

struct ExportMovieDesc
{
	QString sFileName;
	int     startFrame = 0;
	int     endFrame   = 0;
	QSize   exportSize;
	int     videoFps = 30;
	QString sFormat;
};

class MovieExporter
{
public:
    MovieExporter();

	Status run( Object* obj, const ExportMovieDesc& desc );
	QString error();

private:
	void assembleAudio( Object* obj, QString ffmpegPath );

	QString mStrWorkDir;
	ExportMovieDesc mDesc;
};

#endif // MOVIEEXPORTER_H