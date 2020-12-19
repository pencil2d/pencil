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

#include "movieexporter.h"

#include <ctime>
#include <vector>
#include <cstdint>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QApplication>
#include <QStandardPaths>
#include <QThread>
#include <QtMath>
#include <QPainter>

#include "object.h"
#include "layercamera.h"
#include "layersound.h"
#include "soundclip.h"
#include "util.h"

MovieExporter::MovieExporter()
{
}

MovieExporter::~MovieExporter()
{
}

/** Begin exporting the movie described by exportDesc.
 *
 * @param[in] obj An Object containing the animation to export.
 * @param[in] desc A structure containing all the export parameters.
 *            See ExportMovieDesc.
 * @param[out] majorProgress A function to update the major progress bar.
 *                          The major progress bar goes from 0-100% only
 *                          one time, representing the overall progress of
 *                          the export. The first float parameter is
 *                          the current progress %, and the second is
 *                          the desired progress when the next sub-task
 *                          completes. This should only be called at the
 *                          beginning of a subtask.
 * @param[out] minorProgress A function to update the minor progress bar.
 *                           The minor progress bar goes from 0-100% for
 *                           each sub-task of the exporting process.
 *                           It is up to minor progress to update the
 *                           major progress bar to reflect the sub-task
 *                           progress.
 * @param[out] progressMessage A function ot update the progres bar
 *                             message. The messages will describe
 *                             the current sub-task of the exporting
 *                             process.
 *
 * @return Returns Status:OK on success, or Status::FAIL on error.
 */
Status MovieExporter::run(const Object* obj,
                          const ExportMovieDesc& desc,
                          std::function<void(float, float)> majorProgress,
                          std::function<void(float)> minorProgress,
                          std::function<void(QString)> progressMessage)
{
    majorProgress(0.f, 0.03f);
    minorProgress(0.f);
    progressMessage(QObject::tr("Checking environment..."));

    clock_t t1 = clock();

    QString ffmpegPath = ffmpegLocation();
    qDebug() << ffmpegPath;
    if (!QFile::exists(ffmpegPath))
    {
#ifdef _WIN32
        qCritical() << "Please place ffmpeg.exe in " << ffmpegPath << " directory";
#else
        qCritical() << "Please place ffmpeg in " << ffmpegPath << " directory";
#endif
        return Status::ERROR_FFMPEG_NOT_FOUND;
    }

    STATUS_CHECK(checkInputParameters(desc))
    mDesc = desc;

    qDebug() << "OutFile: " << mDesc.strFileName;

    // Setup temporary folder
    if (!mTempDir.isValid())
    {
        Q_ASSERT(false && "Cannot create temp folder.");
        return Status::FAIL;
    }

    mTempWorkDir = mTempDir.path();

    minorProgress(0.f);
    if (desc.strFileName.endsWith("gif", Qt::CaseInsensitive))
    {
        majorProgress(0.03f, 1.f);
        progressMessage(QObject::tr("Generating GIF..."));
        minorProgress(0.f);
        STATUS_CHECK(generateGif(obj, ffmpegPath, desc.strFileName, minorProgress))
    }
    else
    {
        majorProgress(0.03f, 0.25f);
        progressMessage(QObject::tr("Assembling audio..."));
        minorProgress(0.f);
        STATUS_CHECK(assembleAudio(obj, ffmpegPath, minorProgress))
        minorProgress(1.f);
        majorProgress(0.25f, 1.f);
        progressMessage(QObject::tr("Generating movie..."));
        STATUS_CHECK(generateMovie(obj, ffmpegPath, desc.strFileName, minorProgress))
    }
    minorProgress(1.f);
    majorProgress(1.f, 1.f);
    progressMessage(QObject::tr("Done"));

    clock_t t2 = clock() - t1;
    qDebug("MOVIE = %.1f sec", static_cast<double>(t2 / CLOCKS_PER_SEC));

    return Status::OK;
}

QString MovieExporter::error()
{
    return QString();
}

/** Combines all audio tracks in obj into a single file.
 *
 *  @param[in] obj
 *  @param[in] ffmpegPath
 *  @param[out] progress A function that takes one float argument
 *              (the percentage of the audio assembly complete) and
 *              may display the output to the user in any way it
 *              sees fit.
 *
 *  @return Returns the final status of the operation. Ok if successful,
 *          or safe if there was intentionally no output.
 */
Status MovieExporter::assembleAudio(const Object* obj,
                                    QString ffmpegPath,
                                    std::function<void(float)> progress)
{
    // Quicktime assemble call
    const int startFrame = mDesc.startFrame;
    const int endFrame = mDesc.endFrame;
    const int fps = mDesc.fps;

    Q_ASSERT(startFrame >= 0);
    Q_ASSERT(endFrame >= startFrame);

    QDir dir(mTempWorkDir);
    Q_ASSERT(dir.exists());

    QString tempAudioPath = QDir(mTempWorkDir).filePath("tmpaudio.wav");
    qDebug() << "TempAudio=" << tempAudioPath;

    std::vector< SoundClip* > allSoundClips;

    std::vector< LayerSound* > allSoundLayers = obj->getLayersByType<LayerSound>();
    for (LayerSound* layer : allSoundLayers)
    {
        layer->foreachKeyFrame([&allSoundClips](KeyFrame* key)
        {
            if (!key->fileName().isEmpty())
            {
                allSoundClips.push_back(static_cast<SoundClip*>(key));
            }
        });
    }

    if (allSoundClips.empty()) return Status::SAFE;

    int clipCount = 0;

    QString filterComplex, amergeInput, panChannelLayout;
    QStringList args;

    int wholeLen = qCeil((endFrame - startFrame) * 44100.0 / fps);
    for (auto clip : allSoundClips)
    {
        if (mCanceled)
        {
            return Status::CANCELED;
        }

        // Add sound file as input
        args << "-i" << clip->fileName();

        // Offset the sound to its correct position
        // See https://superuser.com/questions/716320/ffmpeg-placing-audio-at-specific-location
        filterComplex += QString("[%1:a:0] aformat=sample_fmts=fltp:sample_rates=44100:channel_layouts=mono,volume=1,adelay=%2S|%2S,apad=whole_len=%3[ad%1];")
                    .arg(clipCount).arg(qRound(44100.0 * (clip->pos() - 1) / fps)).arg(wholeLen);
        amergeInput += QString("[ad%1]").arg(clipCount);
        panChannelLayout += QString("c%1+").arg(clipCount);

        clipCount++;
    }
    // Remove final '+'
    panChannelLayout.chop(1);
    // Output arguments
    // Mix audio
    args << "-filter_complex" << QString("%1%2 amerge=inputs=%3, pan=mono|c0=%4 [out]")
            .arg(filterComplex).arg(amergeInput).arg(clipCount).arg(panChannelLayout);
    // Convert audio file: 44100Hz sampling rate, stereo, signed 16 bit little endian
    // Supported audio file types: wav, mp3, ogg... ( all file types supported by ffmpeg )
    args << "-ar" << "44100" << "-acodec" << "pcm_s16le" << "-ac" << "2" << "-map" << "[out]" << "-y";
    // Trim audio
    args << "-ss" << QString::number((startFrame - 1) / static_cast<double>(fps));
    args << "-to" << QString::number(endFrame / static_cast<double>(fps));
    // Output path
    args << tempAudioPath;

    STATUS_CHECK(MovieExporter::executeFFmpeg(ffmpegPath, args, [&progress, this] (int frame) { progress(frame / static_cast<float>(mDesc.endFrame - mDesc.startFrame)); return !mCanceled; }))
    qDebug() << "audio file: " + tempAudioPath;

    return Status::OK;
}

/** Exports obj to a movie image at strOut using FFmpeg.
 *
 *  @param[in]  obj An Object containing the animation to export.
 *  @param[in]  ffmpegPath The path to the FFmpeg binary.
 *  @param[in]  strOutputFile The output path. Should end with .gif.
 *  @param[out] progress A function that takes one float argument
 *              (the percentage of the gif generation complete) and
 *              may display the output to the user in any way it
 *              sees fit.
 *
 *  The movie formats supported by this operation are any file
 *  formats that the referenced FFmpeg binary supports and that have
 *  the required features (ex. video and audio support)
 *
 *  @return Returns the final status of the operation (ok or canceled)
 */
Status MovieExporter::generateMovie(
        const Object* obj,
        QString ffmpegPath,
        QString strOutputFile,
        std::function<void(float)> progress)
{
    if (mCanceled)
    {
        return Status::CANCELED;
    }

    // Frame generation setup

    int frameStart = mDesc.startFrame;
    int frameEnd = mDesc.endFrame;
    const QSize exportSize = mDesc.exportSize;
    bool transparency = mDesc.alpha;
    QString strCameraName = mDesc.strCameraName;
    bool loop = mDesc.loop;

    auto cameraLayer = static_cast<LayerCamera*>(obj->findLayerByName(strCameraName, Layer::CAMERA));
    if (cameraLayer == nullptr)
    {
        cameraLayer = obj->getLayersByType< LayerCamera >().front();
    }
    int currentFrame = frameStart;

    /* We create an image with the correct dimensions and background
     * color here and then copy this and draw over top of it to
     * generate each frame. This is faster than having to generate
     * a new background image for each frame.
     */
    QImage imageToExportBase(exportSize, QImage::Format_ARGB32_Premultiplied);
    QColor bgColor = Qt::white;
    if (transparency)
    {
        bgColor.setAlpha(0);
    }
    imageToExportBase.fill(bgColor);

    QSize camSize = cameraLayer->getViewSize();
    QTransform centralizeCamera;
    centralizeCamera.translate(camSize.width() / 2, camSize.height() / 2);

    int failCounter = 0;
    /* Movie export uses a "sliding window" to reduce memory usage
     * while having a relatively small impact on speed. This basically
     * means that there is a maximum number of frames that can be waiting
     * to be encoded by ffmpeg at any one time. The limit is set by the
     * frameWindow variable which is designed to take up a maximum of
     * about 1GB of memory
     */
    int frameWindow = static_cast<int>(1e9 / (camSize.width() * camSize.height() * 4.0));

    // Build FFmpeg command

    //int exportFps = mDesc.videoFps;
    const QString tempAudioPath = QDir(mTempWorkDir).filePath("tmpaudio.wav");

    QStringList args = {"-f", "rawvideo", "-pixel_format", "bgra"};
    args << "-video_size" << QString("%1x%2").arg(exportSize.width()).arg(exportSize.height());
    args << "-framerate" << QString::number(mDesc.fps);

    //args << "-r" << QString::number(exportFps);
    args << "-i" << "-";
    args << "-threads" << (QThread::idealThreadCount() == 1 ? "0" : QString::number(QThread::idealThreadCount()));

    if (QFile::exists(tempAudioPath))
    {
        args << "-i" << tempAudioPath;
    }

    if (strOutputFile.endsWith(".apng", Qt::CaseInsensitive))
    {
        args << "-plays" << (loop ? "0" : "1");
    }

    if (strOutputFile.endsWith("mp4", Qt::CaseInsensitive))
    {
        args << "-pix_fmt" << "yuv420p";
    }

    if (strOutputFile.endsWith(".avi", Qt::CaseInsensitive))
    {
        args << "-q:v" << "5";
    }

    args << "-y";
    args << strOutputFile;

    // Run FFmpeg command

    STATUS_CHECK(executeFFMpegPipe(ffmpegPath, args, progress, [&](QProcess& ffmpeg, int framesProcessed)
    {
        if(framesProcessed < 0)
        {
            failCounter++;
        }

        if(currentFrame > frameEnd)
        {
            ffmpeg.closeWriteChannel();
            return false;
        }

        if((currentFrame - frameStart <= framesProcessed + frameWindow || failCounter > 10) && currentFrame <= frameEnd)
        {
            QImage imageToExport = imageToExportBase.copy();
            QPainter painter(&imageToExport);

            QTransform view = cameraLayer->getViewAtFrame(currentFrame);
            painter.setWorldTransform(view * centralizeCamera);
            painter.setWindow(QRect(0, 0, camSize.width(), camSize.height()));

            obj->paintImage(painter, currentFrame, false, true);
            painter.end();

            // Should use sizeInBytes instead of byteCount to support large images,
            // but this is only supported in QT 5.10+
            int bytesWritten = ffmpeg.write(reinterpret_cast<const char*>(imageToExport.constBits()), imageToExport.byteCount());
            Q_ASSERT(bytesWritten == imageToExport.byteCount());

            currentFrame++;
            failCounter = 0;
            return true;
        }

        return false;
    }));

    return Status::OK;
}

/** Exports obj to a gif image at strOut using FFmpeg.
 *
 *  @param[in]  obj An Object containing the animation to export.
 *  @param[in]  ffmpegPath The path to the FFmpeg binary.
 *  @param[in]  strOut The output path. Should end with .gif.
 *  @param[out] progress A function that takes one float argument
 *              (the percentage of the gif generation complete) and
 *              may display the output to the user in any way it
 *              sees fit.
 *
 *  @return Returns the final status of the operation (ok or canceled)
 */
Status MovieExporter::generateGif(
        const Object* obj,
        QString ffmpegPath,
        QString strOut,
        std::function<void(float)> progress)
{

    if (mCanceled)
    {
        return Status::CANCELED;
    }

    // Frame generation setup

    int frameStart = mDesc.startFrame;
    int frameEnd = mDesc.endFrame;
    const QSize exportSize = mDesc.exportSize;
    bool transparency = false;
    QString strCameraName = mDesc.strCameraName;
    bool loop = mDesc.loop;
    int bytesWritten;

    auto cameraLayer = static_cast<LayerCamera*>(obj->findLayerByName(strCameraName, Layer::CAMERA));
    if (cameraLayer == nullptr)
    {
        cameraLayer = obj->getLayersByType< LayerCamera >().front();
    }
    int currentFrame = frameStart;

    /* We create an image with the correct dimensions and background
     * color here and then copy this and draw over top of it to
     * generate each frame. This is faster than having to generate
     * a new background image for each frame.
     */
    QImage imageToExportBase(exportSize, QImage::Format_ARGB32_Premultiplied);
    QColor bgColor = Qt::white;
    if (transparency)
    {
        bgColor.setAlpha(0);
    }
    imageToExportBase.fill(bgColor);

    QSize camSize = cameraLayer->getViewSize();
    QTransform centralizeCamera;
    centralizeCamera.translate(camSize.width() / 2, camSize.height() / 2);

    // Build FFmpeg command

    QStringList args = {"-f", "rawvideo", "-pixel_format", "bgra"};
    args << "-video_size" << QString("%1x%2").arg(exportSize.width()).arg(exportSize.height());
    args << "-framerate" << QString::number(mDesc.fps);

    args << "-i" << "-";

    args << "-y";

    args << "-filter_complex" << "[0:v]palettegen [p]; [0:v][p] paletteuse";

    args << "-loop" << (loop ? "0" : "-1");
    args << strOut;

    // Run FFmpeg command

    STATUS_CHECK(executeFFMpegPipe(ffmpegPath, args, progress, [&](QProcess& ffmpeg, int framesProcessed)
    {
        /* The GIF FFmpeg command requires the entires stream to be
         * written before FFmpeg can encode the GIF. This is because
         * the generated pallete is based off of the colors in all
         * frames. The only way to avoid this would be to generate
         * all the frames twice and run two separate commands, which
         * would likely have unacceptable speed costs.
         */

        Q_UNUSED(framesProcessed);
        if(currentFrame > frameEnd)
        {
            ffmpeg.closeWriteChannel();
            return false;
        }

        QImage imageToExport = imageToExportBase.copy();
        QPainter painter(&imageToExport);

        QTransform view = cameraLayer->getViewAtFrame(currentFrame);
        painter.setWorldTransform(view * centralizeCamera);
        painter.setWindow(QRect(0, 0, camSize.width(), camSize.height()));

        obj->paintImage(painter, currentFrame, false, true);

        bytesWritten = ffmpeg.write(reinterpret_cast<const char*>(imageToExport.constBits()), imageToExport.byteCount());
        Q_ASSERT(bytesWritten == imageToExport.byteCount());

        currentFrame++;

        return true;
    }));

    return Status::OK;
}

/** Runs the specified command (should be ffmpeg) and allows for progress feedback.
 *
 *  @param[in]  cmd A string containing the command to execute
 *  @param[in]  args A string list containing the arguments to
 *              pass to the command
 *  @param[out] progress A function that takes one float argument
 *              (the percentage of the ffmpeg operation complete) and
 *              may display the output to the user in any way it
 *              sees fit.
 *
 *  executeFFMpeg does not allow for writing direct input, the only
 *  input through the "-i" argument to specify input files on the disk.
 *
 *  @return Returns Status::OK if everything went well, and Status::FAIL
 *  and error is detected (usually a non-zero exit code for ffmpeg).
 */
Status MovieExporter::executeFFmpeg(const QString& cmd, const QStringList& args, std::function<bool(int)> progress)
{
    qDebug() << cmd;

    QProcess ffmpeg;
    ffmpeg.setReadChannel(QProcess::StandardOutput);
    // FFmpeg writes to stderr only for some reason, so we just read both channels together
    ffmpeg.setProcessChannelMode(QProcess::MergedChannels);
    ffmpeg.start(cmd, args);

    Status status = Status::OK;
    DebugDetails dd;
    dd << QStringLiteral("Command: %1 %2").arg(cmd).arg(args.join(' '));
    if (ffmpeg.waitForStarted())
    {
        while(ffmpeg.state() == QProcess::Running)
        {
            if(!ffmpeg.waitForReadyRead()) break;

            QString output(ffmpeg.readAll());
            QStringList sList = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
            for (const QString& s : sList)
            {
                qDebug() << "[ffmpeg]" << s;
                dd << s;
            }

            if(output.startsWith("frame="))
            {
                QString frame = output.mid(6, output.indexOf(' '));

                bool shouldContinue = progress(frame.toInt());
                if (!shouldContinue)
                {
                    ffmpeg.terminate();
                    ffmpeg.waitForFinished(3000);
                    if (ffmpeg.state() == QProcess::Running) ffmpeg.kill();
                    ffmpeg.waitForFinished();
                    return Status::CANCELED;
                }
            }
        }

        QString output(ffmpeg.readAll());
        QStringList sList = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        for (const QString& s : sList)
        {
            qDebug() << "[ffmpeg]" << s;
            dd << s;
        }

        if(ffmpeg.exitStatus() != QProcess::NormalExit || ffmpeg.exitCode() != 0)
        {
            status = Status::FAIL;
            status.setTitle(QObject::tr("Something went wrong"));
            status.setDescription(QObject::tr("Looks like our video backend did not exit normally. Your movie may not have exported correctly. Please try again and report this if it persists."));
            dd << QString("Exit status: ").append(QProcess::NormalExit ? "NormalExit": "CrashExit")
               << QString("Exit code: %1").arg(ffmpeg.exitCode());
            status.setDetails(dd);
            return status;
        }
    }
    else
    {
        qDebug() << "ERROR: Could not execute FFmpeg.";
        status = Status::FAIL;
        status.setTitle(QObject::tr("Something went wrong"));
        status.setDescription(QObject::tr("Couldn't start the video backend, please try again."));
        status.setDetails(dd);
    }
    return status;
}

/** Runs the specified command (should be ffmpeg), and lets
 *  writeFrame pipe data into it 1 frame at a time.
 *
 *  @param[in]  cmd A string containing the command to execute
 *  @param[in]  args A string list containing the arguments to
 *              pass to the command
 *  @param[out] progress A function that takes one float argument
 *              (the percentage of the ffmpeg operation complete) and
 *              may display the output to the user in any way it
 *              sees fit.
 *  @param[in]  writeFrame A function that takes two arguments, a
 *              process (the ffmpeg process) and an integer
 *              (frames processed or -1, see full description).
 *              This function should write a single frame to the
 *              process. The function returns true value if it
 *              actually wrote a frame.
 *
 *  This function operates generally as follows:
 *  1. Spawn process with the command from cmd
 *  2. Check ffmpeg's output for a progress update.
 *  3. Add frames with writeFrame until it returns false.
 *  4. Repeat from step 2 until all frames have been written.
 *
 *  The idea is that there are two forms of processing occuring
 *  simultaneously, generating frames to send to ffmpeg, and ffmpeg
 *  encoding those frames. Whether these this actually occur
 *  concurrently or one after another appears to depend on the environment.
 *
 *  The writeFrame function deserves a bit of extra details. It does
 *  not only return false when there is an error in generating or
 *  writing a frame, it also does it when it wants to "return control"
 *  to the rest of the executeFFMpegPipe function for the purposes of
 *  reading updates from ffmpeg's output. This should be done every
 *  once in a while if possible, but with some formats (specifically gif),
 *  all frames must be loaded before any processing can continue, so
 *  there is no point returning false for it until all frames have
 *  been written. writeFrame is also responsible for closing the writeChannel
 *  of the process when it has finished writing all frames. This indicates
 *  to executeFFMpegPipe that it no longer needs to call writeFrame.
 *
 *  @return Returns Status::OK if everything went well, and Status::FAIL
 *  and error is detected (usually a non-zero exit code for ffmpeg).
 */
Status MovieExporter::executeFFMpegPipe(const QString& cmd, const QStringList& args, std::function<void(float)> progress, std::function<bool(QProcess&, int)> writeFrame)
{
    qDebug() << cmd;

    QProcess ffmpeg;
    ffmpeg.setReadChannel(QProcess::StandardOutput);
    // FFmpeg writes to stderr only for some reason, so we just read both channels together
    ffmpeg.setProcessChannelMode(QProcess::MergedChannels);
    ffmpeg.start(cmd, args);

    Status status = Status::OK;
    DebugDetails dd;
    dd << QStringLiteral("Command: %1 %2").arg(cmd).arg(args.join(' '));
    if (ffmpeg.waitForStarted())
    {
        int framesGenerated = 0;
        int lastFrameProcessed = 0;
        const int frameStart = mDesc.startFrame;
        const int frameEnd = mDesc.endFrame;
        while(ffmpeg.state() == QProcess::Running)
        {
            if (mCanceled)
            {
                ffmpeg.terminate();
                if (ffmpeg.state() == QProcess::Running) ffmpeg.kill();
                return Status::CANCELED;
            }

            // Check FFmpeg progress

            int framesProcessed = -1;
            if(ffmpeg.waitForReadyRead(10))
            {
                QString output(ffmpeg.readAll());
                QStringList sList = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
                for (const QString& s : sList)
                {
                    qDebug() << "[ffmpeg]" << s;
                    dd << s;
                }
                if(output.startsWith("frame="))
                {
                    lastFrameProcessed = framesProcessed = output.mid(6, output.indexOf(' ')).toInt();
                }
            }

            if(!ffmpeg.isWritable())
            {
                continue;
            }

            while(writeFrame(ffmpeg, framesProcessed))
            {
                framesGenerated++;

                const float percentGenerated = framesGenerated / static_cast<float>(frameEnd - frameStart);
                const float percentConverted = lastFrameProcessed / static_cast<float>(frameEnd - frameStart);
                progress((percentGenerated + percentConverted) / 2);
            }
            const float percentGenerated = framesGenerated / static_cast<float>(frameEnd - frameStart);
            const float percentConverted = lastFrameProcessed / static_cast<float>(frameEnd - frameStart);
            progress((percentGenerated + percentConverted) / 2);
        }

        QString output(ffmpeg.readAll());
        QStringList sList = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        for (const QString& s : sList)
        {
            qDebug() << "[ffmpeg]" << s;
            dd << s;
        }

        if(ffmpeg.exitStatus() != QProcess::NormalExit  || ffmpeg.exitCode() != 0)
        {
            status = Status::FAIL;
            status.setTitle(QObject::tr("Something went wrong"));
            status.setDescription(QObject::tr("Looks like our video backend did not exit normally. Your movie may not have exported correctly. Please try again and report this if it persists."));
            dd << QString("Exit status: ").append(QProcess::NormalExit ? "NormalExit": "CrashExit")
               << QString("Exit code: %1").arg(ffmpeg.exitCode());
            status.setDetails(dd);
            return status;
        }
    }
    else
    {
        qDebug() << "ERROR: Could not execute FFmpeg.";
        status = Status::FAIL;
        status.setTitle(QObject::tr("Something went wrong"));
        status.setDescription(QObject::tr("Couldn't start the video backend, please try again."));
        status.setDetails(dd);
    }

    return status;
}

Status MovieExporter::checkInputParameters(const ExportMovieDesc& desc)
{
    bool b = true;
    b &= (!desc.strFileName.isEmpty());
    b &= (desc.startFrame > 0);
    b &= (desc.endFrame >= desc.startFrame);
    b &= (desc.fps > 0);
    b &= (!desc.strCameraName.isEmpty());

    return b ? Status::OK : Status::INVALID_ARGUMENT;
}
