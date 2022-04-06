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
#include "movieimporter.h"

#include <QDebug>
#include <QTemporaryDir>
#include <QProcess>
#include <QtMath>
#include <QTime>
#include <QFileInfo>

#include "movieexporter.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "soundmanager.h"

#include "soundclip.h"
#include "bitmapimage.h"

#include "util.h"
#include "editor.h"

MovieImporter::MovieImporter(QObject* parent) : QObject(parent)
{
}

MovieImporter::~MovieImporter()
{
}

Status MovieImporter::estimateFrames(const QString &filePath, int fps, int *frameEstimate)
{
    Status status = Status::OK;
    DebugDetails dd;
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() != Layer::BITMAP)
    {
        status = Status::FAIL;
        status.setTitle(tr("Bitmap only"));
        status.setDescription(tr("You need to be on the bitmap layer to import a movie clip"));
        return status;
    }

    // --------- Import all the temporary frames ----------
    STATUS_CHECK(verifyFFmpegExists());
    QString ffmpegPath = ffmpegLocation();
    dd << "ffmpeg path:" << ffmpegPath;

    // Get frame estimate
    int frames = -1;
    bool ok = true;
    QString ffprobePath = ffprobeLocation();
    dd << "ffprobe path:" << ffprobePath;
    if (QFileInfo::exists(ffprobePath))
    {
        QStringList probeArgs = {"-v", "error", "-show_entries", "format=duration", "-of", "default=noprint_wrappers=1:nokey=1", filePath};
        QProcess ffprobe;
        ffprobe.setReadChannel(QProcess::StandardOutput);
        ffprobe.start(ffprobePath, probeArgs);
        ffprobe.waitForFinished();
        if (ffprobe.exitStatus() == QProcess::NormalExit && ffprobe.exitCode() == 0)
        {
            QString output(ffprobe.readAll());
            double seconds = output.toDouble(&ok);
            if (ok)
            {
                frames = qCeil(seconds * fps);
            }
            else
            {
                ffprobe.setReadChannel(QProcess::StandardError);
                dd << "FFprobe output could not be parsed"
                   << "stdout:"
                   << output
                   << "stderr:"
                   << ffprobe.readAll();
            }
        }
        else
        {
            ffprobe.setProcessChannelMode(QProcess::MergedChannels);
            dd << "FFprobe did not exit normally"
               << QString("Exit status: ").append(ffprobe.exitStatus() == QProcess::NormalExit ? "NormalExit" : "CrashExit")
               << QString("Exit code: %1").arg(ffprobe.exitCode())
               << "Output:"
               << ffprobe.readAll();
        }
        if (frames < 0)
        {
            qDebug() << "ffprobe execution failed. Details:";
            qDebug() << dd.str();
        }
    }
    if (frames < 0)
    {
        // Fallback to ffmpeg
        QStringList probeArgs = {"-i", filePath};
        QProcess ffmpeg;
        // FFmpeg writes to stderr only for some reason, so we just read both channels together
        ffmpeg.setProcessChannelMode(QProcess::MergedChannels);
        ffmpeg.start(ffmpegPath, probeArgs);
        if (ffmpeg.waitForStarted() == true)
        {
            int index = -1;
            while (ffmpeg.state() == QProcess::Running)
            {
                if (!ffmpeg.waitForReadyRead()) break;

                QString output(ffmpeg.readAll());
                QStringList sList = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
                for (const QString& s : sList)
                {
                    index = s.indexOf("Duration: ");
                    if (index >= 0)
                    {
                        QString format("hh:mm:ss.zzz");
                        QString durationString = s.mid(index + 10, format.length()-1) + "0";
                        int curFrames = qCeil(QTime(0, 0).msecsTo(QTime::fromString(durationString, format)) / 1000.0 * fps);
                        frames = qMax(frames, curFrames);

                        // We've got what we need, stop running
                        ffmpeg.terminate();
                        ffmpeg.waitForFinished(3000);
                        if (ffmpeg.state() == QProcess::Running) ffmpeg.kill();
                        ffmpeg.waitForFinished();
                        break;
                    }
                }
            }
        }
    }

    if (frames < 0)
    {
        status = Status::FAIL;
        status.setTitle(tr("Loading video failed"));
        status.setDescription(tr("Could not get duration from the specified video. Are you sure you are importing a valid video file?"));
        status.setDetails(dd);
        return status;
    }

    *frameEstimate = frames;
    return status;
}

Status MovieImporter::run(const QString &filePath, int fps, FileType type,
                          std::function<void(int)> progress,
                          std::function<void(QString)> progressMessage,
                          std::function<bool()> askPermission)
{
    if (mCanceled) return Status::CANCELED;

    Status status = Status::OK;
    DebugDetails dd;

    STATUS_CHECK(verifyFFmpegExists())

    mTempDir = new QTemporaryDir();
    if (!mTempDir->isValid())
    {
        status = Status::FAIL;
        status.setTitle(tr("Error creating folder"));
        status.setDescription(tr("Unable to create a temporary folder, cannot import video."));
        dd << QString("Path: ").append(mTempDir->path())
           << QString("Error: ").append(mTempDir->errorString());
        status.setDetails(dd);
        return status;
    }
    mEditor->addTemporaryDir(mTempDir);

    if (type == FileType::MOVIE) {
        int frames = 0;
        STATUS_CHECK(estimateFrames(filePath, fps, &frames));

        if (mEditor->currentFrame() + frames > MaxFramesBound) {
            status = Status::FAIL;
            status.setTitle(tr("Imported movie too big!"));
            status.setDescription(tr("The movie clip is too long. Pencil2D can only hold %1 frames, but this movie would go up to about frame %2. "
                                              "Please make your video shorter and try again.")
                                              .arg(MaxFramesBound)
                                              .arg(mEditor->currentFrame() + frames));

            return status;
        }

        if(frames > 200)
        {
            bool canProceed = askPermission();

            if (!canProceed) { return Status::CANCELED; }
        }

        auto progressCallback = [&progress, this](int prog) -> bool
        {
            progress(prog); return !mCanceled;
        };
        auto progressMsgCallback = [&progressMessage](QString message)
        {
            progressMessage(message);
        };
        return importMovieVideo(filePath, fps, frames, progressCallback, progressMsgCallback);
    }
    else if (type == FileType::SOUND)
    {
        return importMovieAudio(filePath, [&progress, this](int prog) -> bool
        {
            progress(prog); return !mCanceled;
        });
    }
    else
    {
        Status st = Status::FAIL;
        st.setTitle(tr("Unknown error"));
        st.setTitle(tr("This should not happen..."));
        return st;
    }
}

Status MovieImporter::importMovieVideo(const QString &filePath, int fps, int frameEstimate,
                                       std::function<bool(int)> progress,
                                       std::function<void(QString)> progressMessage)
{
    Status status = Status::OK;

    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() != Layer::BITMAP)
    {
        status = Status::FAIL;
        status.setTitle(tr("Bitmap only"));
        status.setDescription(tr("You need to be on the bitmap layer to import a movie clip"));
        return status;
    }

    QStringList args = {"-i", filePath};
    args << "-r" << QString::number(fps);
    args << QDir(mTempDir->path()).filePath("%05d.png");

    status = MovieExporter::executeFFmpeg(ffmpegLocation(), args, [&progress, frameEstimate, this] (int frame) {
        progress(qFloor(qMin(frame / static_cast<double>(frameEstimate), 1.0) * 50)); return !mCanceled; }
    );

    if (!status.ok() && status != Status::CANCELED) { return status; }

    if(mCanceled) return Status::CANCELED;

    progressMessage(tr("Video processed, adding frames..."));

    progress(50);

    return generateFrames([this, &progress](int prog) -> bool
    {
        progress(prog); return mCanceled;
    });
}

Status MovieImporter::generateFrames(std::function<bool(int)> progress)
{
    Layer* layer = mEditor->layers()->currentLayer();
    Status status = Status::OK;
    int i = 1;
    QDir tempDir(mTempDir->path());
    auto amountOfFrames = tempDir.count();
    QString currentFile(tempDir.filePath(QString("%1.png").arg(i, 5, 10, QChar('0'))));
    QPoint imgTopLeft;

    ViewManager* viewMan = mEditor->view();

    while (QFileInfo::exists(currentFile))
    {
        int currentFrame = mEditor->currentFrame();
        if(layer->keyExists(mEditor->currentFrame())) {
            mEditor->importImage(currentFile);
        }
        else {
            BitmapImage* bitmapImage = new BitmapImage(imgTopLeft, currentFile);
            if(imgTopLeft.isNull()) {
                imgTopLeft.setX(static_cast<int>(viewMan->getImportView().dx()) - bitmapImage->image()->width() / 2);
                imgTopLeft.setY(static_cast<int>(viewMan->getImportView().dy()) - bitmapImage->image()->height() / 2);
                bitmapImage->moveTopLeft(imgTopLeft);
            }
            layer->addKeyFrame(currentFrame, bitmapImage);
            mEditor->layers()->notifyAnimationLengthChanged();
            mEditor->scrubTo(currentFrame + 1);
        }
        if (mCanceled) return Status::CANCELED;
        progress(qFloor(50 + i / static_cast<qreal>(amountOfFrames) * 50));
        i++;
        currentFile = tempDir.filePath(QString("%1.png").arg(i, 5, 10, QChar('0')));
    }

    if (!QFileInfo::exists(tempDir.filePath("00001.png"))) {
        status = Status::FAIL;
        status.setTitle(tr("Failed import"));
        status.setDescription(tr("Was unable to find internal files, import unsuccessful."));
        return status;
    }

    return status;
}

Status MovieImporter::importMovieAudio(const QString& filePath, std::function<bool(int)> progress)
{
    Layer* layer = mEditor->layers()->currentLayer();

    Status status = Status::OK;
    if (layer->type() != Layer::SOUND)
    {
        status = Status::FAIL;
        status.setTitle(tr("Sound only"));
        status.setDescription(tr("You need to be on a sound layer to import the audio"));
        return status;
    }

    int currentFrame = mEditor->currentFrame();

    if (layer->keyExists(currentFrame))
    {
        SoundClip* key = static_cast<SoundClip*>(layer->getKeyFrameAt(currentFrame));
        if (!key->fileName().isEmpty())
        {
            status = Status::FAIL;
            status.setTitle(tr("Move to an empty frame"));
            status.setDescription(tr("A frame already exists on frame: %1 Move the scrubber to a empty position on the timeline and try again").arg(currentFrame));
            return status;
        }
        layer->removeKeyFrame(currentFrame);
    }

    QString audioPath = QDir(mTempDir->path()).filePath("audio.wav");

    QStringList args{ "-i", filePath, audioPath };

    status = MovieExporter::executeFFmpeg(ffmpegLocation(), args, [&progress, this] (int frame) {
        Q_UNUSED(frame)
        progress(50); return !mCanceled;
    });

    if(mCanceled) return Status::CANCELED;
    progress(90);

    Q_ASSERT(!layer->keyExists(currentFrame));

    SoundClip* key = new SoundClip;
    layer->addKeyFrame(currentFrame, key);

    key->setSoundClipName(QFileInfo(filePath).fileName()); // keep the original file name
    Status st = mEditor->sound()->loadSound(key, audioPath);

    if (!st.ok())
    {
        layer->removeKeyFrame(currentFrame);
        return st;
    }

    return Status::OK;
}


Status MovieImporter::verifyFFmpegExists()
{
    QString ffmpegPath = ffmpegLocation();
    if (!QFile::exists(ffmpegPath))
    {
        Status status = Status::ERROR_FFMPEG_NOT_FOUND;
        status.setTitle(tr("FFmpeg Not Found"));
        status.setDescription(tr("Please place the ffmpeg binary in plugins directory and try again"));
        return status;
    }
    return Status::OK;
}
