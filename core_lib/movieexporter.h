/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

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
    int     endFrame = 0;
    //int     videoFps   = 30;
    int     fps = 12;
    QSize   exportSize{ 0, 0 };
    QString strCameraName;
};

class MovieExporter
{
public:
    MovieExporter();
    ~MovieExporter();

    Status run(const Object* obj,
               const ExportMovieDesc& desc,
               std::function<void(float)> progress);
    QString error();

    void cancel() { mCanceled = true; }

private:
    Status assembleAudio(const Object* obj, QString ffmpegPath, std::function<void(float)> progress);
    Status generateImageSequence(const Object* obj, std::function<void(float)> progress);
    Status combineVideoAndAudio(QString ffmpegPath, QString strOutputFile);

    Status convertVideoAgain(QString ffmpeg, QString strIn, QString strOut);
    Status convertToGif(QString ffmpeg, QString strOut);

    Status executeFFMpegCommand(QString strCmd);
    Status checkInputParameters(const ExportMovieDesc&);

private:
    QTemporaryDir mTempDir;
    QString mTempWorkDir;
    ExportMovieDesc mDesc;
    bool mCanceled = false;
};

#endif // MOVIEEXPORTER_H
