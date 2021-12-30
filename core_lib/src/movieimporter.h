/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef MOVIEIMPORTER_H
#define MOVIEIMPORTER_H

#include "pencilerror.h"

#include <QObject>
#include <functional>
#include "filetype.h"

class Editor;
class QTemporaryDir;

class MovieImporter : public QObject
{
    Q_OBJECT
public:
    MovieImporter(QObject* parent);
    virtual ~MovieImporter();

    void setCore(Editor* editor) { mEditor = editor; }

    /** Attempts to load a video and determine it's duration.
     *
     * This will analyze the video to estimate how many frames will be imported
     * and set frameEstimate to this value.
     *
     * @param[in] filePath Path to the video file.
     * @param[in] fps Frames per second to import at.
     * @param[out] frameEstimate An estimate of the number of frames if successful, unchanged otherwise.
     * @return Will FAIL if an error occurs during loading the video or calculating the duration, or OK if everything succeded.
     */
    Status estimateFrames(const QString& filePath, int fps, int* frameEstimate);

    /**
     * @param filePath Path to the video file.
     * @param fps Frames per second to import at.
     * @param type FileType to import, should be either MOVIE or SOUND
     * @param progress a function that returns and notify the progress
     * @param progressMessage a function that returns and change the progress message
     * @param askPermission a function that when called, could would be used to notify UI for permission
     * @return whether the run suceeded, failed or canceled
     */
    Status run(const QString& filePath, int fps, FileType type,
               std::function<void(int)> progress,
               std::function<void(QString)> progressMessage,
               std::function<bool()> askPermission);

    void cancel() { mCanceled = true; }

private:

    Status verifyFFmpegExists();
    Status importMovieVideo(const QString& filePath, int fps, int frameEstimate,
                            std::function<bool(int)> progress,
                            std::function<void(QString)> progressMessage);
    Status importMovieAudio(const QString& filePath, std::function<bool(int)> progress);

    Status generateFrames(std::function<bool(int)> progress);

    Editor* mEditor = nullptr;

    QTemporaryDir* mTempDir = nullptr;

    bool mCanceled = false;
};

#endif // MOVIEIMPORTER_H
