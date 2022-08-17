/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

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
#include <QCoreApplication>
#include <QString>
#include <QSize>
#include <QTemporaryDir>
#include "pencilerror.h"

class Object;
class QProcess;

struct ExportMovieDesc
{
    QString strFileName;
    int     startFrame = 0;
    int     endFrame = 0;
    //int     videoFps   = 30;
    int     fps = 12;
    QSize   exportSize{ 0, 0 };
    QString strCameraName;
    bool loop = false;
    bool alpha = false;
};

class MovieExporter
{
    Q_DECLARE_TR_FUNCTIONS(MovieExporter)
public:
    MovieExporter();
    ~MovieExporter();

    Status run(const Object* obj,
               const ExportMovieDesc& desc,
               std::function<void(float, float)> majorProgress,
               std::function<void(float)> minorProgress,
               std::function<void(QString)> progressMessage);
    QString error();

    void cancel() { mCanceled = true; }

    // The maximum number of sound frames that can be successfully rendered
    static const int MAX_SOUND_FRAMES = 63;

    static Status executeFFmpeg(const QString& cmd, const QStringList& args, std::function<bool(int)> progress);
private:
    Status assembleAudio(const Object* obj, QString ffmpegPath, std::function<void(float)> progress);
    Status generateMovie(const Object *obj, QString ffmpegPath, QString strOutputFile, std::function<void(float)> progress);
    Status generateGif(const Object *obj, QString ffmpeg, QString strOut, std::function<void(float)>  progress);

    Status executeFFMpegPipe(const QString& cmd, const QStringList& args, std::function<void(float)> progress, std::function<bool(QProcess&,int)> writeFrame);
    Status checkInputParameters(const ExportMovieDesc&);

private:
    QTemporaryDir mTempDir;
    QString mTempWorkDir;
    ExportMovieDesc mDesc;
    bool mCanceled = false;
};

#endif // MOVIEEXPORTER_H
