/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

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

#include "object.h"
#include "layercamera.h"
#include "layersound.h"
#include "soundclip.h"

// refs
// http://www.topherlee.com/software/pcm-tut-wavformat.html
// http://soundfile.sapp.org/doc/WaveFormat/
//
struct WavFileHeader
{
    char    riff[4];
    int32_t chuckSize;
    char    format[4];
    char    fmtID[4];
    int32_t fmtChuckSize;
    int16_t audioFormat;
    int16_t numChannels;
    int32_t sampleRate;
    int32_t byteRate;
    int16_t blockAlign;
    int16_t bitsPerSample;
    char    dataChuckID[4];
    int32_t dataSize;

    void InitWithDefaultValues()
    {
        strncpy(riff, "RIFF", 4);
        chuckSize = 0;
        strncpy(format, "WAVE", 4);
        strncpy(fmtID, "fmt ", 4);
        fmtChuckSize = 16;
        audioFormat = 1; // 1 means PCM
        numChannels = 2; // stereo
        sampleRate = 44100;
        bitsPerSample = 16;
        blockAlign = (bitsPerSample * numChannels) / 8;
        byteRate = (sampleRate * bitsPerSample * numChannels) / 8;

        strncpy(dataChuckID, "data", 4);
        dataSize = 0;
    }
};

int16_t safeSumInt16(int16_t a, int16_t b)
{
    int32_t a32 = static_cast<int32_t>(a);
    int32_t b32 = static_cast<int32_t>(b);

    if ((a32 + b32) > INT16_MAX)
    {
        return INT16_MAX;
    }
    else if ((a32 + b32) < INT16_MIN)
    {
        return INT16_MIN;
    }
    return a + b;
}

void skipUselessChucks(WavFileHeader& header, QFile& file)
{
    // We only care about the 'data' chuck
    while (memcmp(header.dataChuckID, "data", 4) != 0)
    {
        int skipByteCount = header.dataSize;
        std::vector<char> skipData(skipByteCount);
        file.read(skipData.data(), skipByteCount);

        file.read((char*)&header.dataChuckID, 4);
        file.read((char*)&header.dataSize, 4);
    }
}

QString ffmpegLocation()
{
#ifdef _WIN32
    return QApplication::applicationDirPath() + "/plugins/ffmpeg.exe";
#elif __APPLE__
    return QApplication::applicationDirPath() + "/plugins/ffmpeg";
#else
    QString ffmpegPath = QStandardPaths::findExecutable(
        "ffmpeg",
        QStringList()
        << QApplication::applicationDirPath() + "/plugins"
        << QApplication::applicationDirPath() + "/../plugins" // linuxdeployqt in FHS-like mode
    );
    if (!ffmpegPath.isEmpty())
    {
        return ffmpegPath;
    }
    return QStandardPaths::findExecutable("ffmpeg"); // ffmpeg is a standalone project.
#endif
}

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
    progressMessage(QApplication::tr("Checking environment..."));

    clock_t t1 = clock();

    QString ffmpegPath = ffmpegLocation();
    qDebug() << ffmpegPath;
    if (!QFile::exists(ffmpegPath))
    {
#ifdef _WIN32
        qCritical() << "Please place ffmpeg.exe in " << ffmpegPath << " directory";
#elif __APPLE__
        qCritical() << "Please place ffmpeg in " << ffmpegPath << " directory";
#else
        qCritical() << "Please place ffmpeg in " << ffmpegPath << " directory";
#endif
        return Status::ERROR_FFMPEG_NOT_FOUND;
    }

    STATUS_CHECK(checkInputParameters(desc));
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
        progressMessage("Generating gif...");
        minorProgress(0.f);
        STATUS_CHECK(generateGif(obj, ffmpegPath, desc.strFileName, minorProgress));
    }
    else
    {
        majorProgress(0.03f, 0.25f);
        progressMessage("Assembling audio...");
        minorProgress(0.f);
        STATUS_CHECK(assembleAudio(obj, ffmpegPath, minorProgress));
        minorProgress(1.f);
        majorProgress(0.25f, 1.f);
        progressMessage("Generating movie...");
        STATUS_CHECK(generateMovie(obj, ffmpegPath, desc.strFileName, minorProgress));
    }
    minorProgress(1.f);
    majorProgress(1.f, 1.f);
    progressMessage(QApplication::tr("Done"));
    
    clock_t t2 = clock() - t1;
    qDebug("MOVIE = %.1f sec", static_cast<float>(t2) / CLOCKS_PER_SEC);

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

    float lengthInSec = (endFrame - startFrame + 1) / (float)fps;
    qDebug() << "Audio Length = " << lengthInSec << " seconds";

    int32_t audioDataSize = 44100 * 2 * 2 * lengthInSec;

    std::vector<int16_t> audioData(audioDataSize / sizeof(int16_t));

    bool audioDataValid = false;

    QDir dir(mTempWorkDir);
    Q_ASSERT(dir.exists());

    QString tempAudioPath = mTempWorkDir + "/tmpaudio0.wav";
    qDebug() << "TempAudio=" << tempAudioPath;

    std::vector< SoundClip* > allSoundClips;

    std::vector< LayerSound* > allSoundLayers = obj->getLayersByType<LayerSound>();
    for (LayerSound* layer : allSoundLayers)
    {
        layer->foreachKeyFrame([&allSoundClips](KeyFrame* key)
        {
            allSoundClips.push_back(static_cast<SoundClip*>(key));
        });
    }

    int clipCount = 0;

    for (SoundClip* clip : allSoundClips)
    {
        if (mCanceled)
        {
            return Status::CANCELED;
        }

        // convert audio file: 44100Hz sampling rate, stereo, signed 16 bit little endian
        // supported audio file types: wav, mp3, ogg... ( all file types supported by ffmpeg )
        QString strCmd;
        strCmd += QString("\"%1\"").arg(ffmpegPath);
        strCmd += QString(" -i \"%1\" ").arg(clip->fileName());
        strCmd += "-ar 44100 -acodec pcm_s16le -ac 2 -y ";
        strCmd += QString("\"%1\"").arg(tempAudioPath);

        executeFFMpeg(strCmd, [](float){});
        qDebug() << "audio file: " + tempAudioPath;

        // Read wav file header
        WavFileHeader header;
        QFile file(tempAudioPath);
        file.open(QIODevice::ReadOnly);
        file.read((char*)&header, sizeof(WavFileHeader));

        skipUselessChucks(header, file);

        int32_t audioSize = header.dataSize;

        qDebug() << "audio len " << audioSize;

        // before calling malloc should check: audioSize < max credible value
        std::vector< int16_t > data(audioSize / sizeof(int16_t));
        file.read((char*)data.data(), audioSize);
        audioDataValid = true;

        float fframe = (float)clip->pos() / (float)fps;
        int delta = fframe * 44100 * 2;
        qDebug() << "audio delta " << delta;

        int indexMax = std::min(audioSize / 2, audioDataSize / 2 - delta);

        // audio files 'mixing': 'higher' sound layers overwrite 'lower' sound layers
        for (int i = 0; i < indexMax; i++)
        {
            audioData[i + delta] = safeSumInt16(audioData[i + delta], data[i]);
        }

        file.close();

        progress((float)clipCount / allSoundClips.size());
        clipCount++;
    }

    if (!audioDataValid)
    {
        return Status::SAFE;
    }

    // save mixed audio file ( will be used as audio stream )
    QFile file(mTempWorkDir + "/tmpaudio.wav");
    file.open(QIODevice::WriteOnly);

    WavFileHeader outputHeader;
    outputHeader.InitWithDefaultValues();
    outputHeader.dataSize = audioDataSize;
    outputHeader.chuckSize = 36 + audioDataSize;

    file.write((char*)&outputHeader, sizeof(outputHeader));
    file.write((char*)audioData.data(), audioDataSize);
    file.close();

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
    bool transparency = false;
    QString strCameraName = mDesc.strCameraName;
    bool loop = mDesc.loop;

    auto cameraLayer = (LayerCamera*)obj->findLayerByName(strCameraName, Layer::CAMERA);
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
    int frameWindow = (int) (1e9 / (camSize.width() * camSize.height() * 4.0));

    // Build FFmpeg command

    //int exportFps = mDesc.videoFps;
    const QString tempAudioPath = mTempWorkDir + "/tmpaudio.wav";

    QString strCmd = QString("\"%1\"").arg(ffmpegPath);
    strCmd += QString(" -f rawvideo -pixel_format bgra");
    strCmd += QString(" -video_size %1x%2").arg(exportSize.width()).arg(exportSize.height());
    strCmd += QString(" -framerate %1").arg(mDesc.fps);

    //strCmd += QString( " -r %1").arg( exportFps );
    strCmd += QString(" -i -");
    strCmd += QString(" -threads %1").arg(QThread::idealThreadCount() == 1 ? 0 : QThread::idealThreadCount());

    if (QFile::exists(tempAudioPath))
    {
        strCmd += QString(" -i \"%1\" ").arg(tempAudioPath);
    }

    if(strOutputFile.endsWith(".apng"))
    {
        strCmd += QString(" -plays %1").arg(loop ? "0" : "1");
    }

    if (strOutputFile.endsWith("mp4", Qt::CaseInsensitive))
    {
        strCmd += QString(" -pix_fmt yuv420p");
    }
    strCmd += " -y";
    strCmd += QString(" \"%1\"").arg(strOutputFile);

    // Run FFmpeg command

    STATUS_CHECK(executeFFMpegPipe(strCmd, progress, [&](QProcess& ffmpeg, int framesProcessed)
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

    auto cameraLayer = (LayerCamera*)obj->findLayerByName(strCameraName, Layer::CAMERA);
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

    QTransform view = cameraLayer->getViewAtFrame(currentFrame);

    QSize camSize = cameraLayer->getViewSize();
    QTransform centralizeCamera;
    centralizeCamera.translate(camSize.width() / 2, camSize.height() / 2);

    // Build FFmpeg command

    QString strCmd = QString("\"%1\"").arg(ffmpegPath);
    strCmd += QString(" -f rawvideo -pixel_format bgra");
    strCmd += QString(" -video_size %1x%2").arg(exportSize.width()).arg(exportSize.height());
    strCmd += QString(" -framerate %1").arg(mDesc.fps);

    strCmd += " -i -";

    strCmd += " -y";

    strCmd += " -filter_complex \"[0:v]palettegen [p]; [0:v][p] paletteuse\"";

    strCmd += QString(" -loop %1").arg(loop ? "0" : "-1");
    strCmd += QString(" \"%1\"").arg(strOut);

    // Run FFmpeg command

    STATUS_CHECK(executeFFMpegPipe(strCmd, progress, [&](QProcess& ffmpeg, int framesProcessed)
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
 *  @param[in]  strCmd A string containing the command to execute and
 *              all of its arguments
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
Status MovieExporter::executeFFMpeg(QString strCmd, std::function<void(float)> progress)
{
    qDebug() << strCmd;

    QProcess ffmpeg;
    ffmpeg.setReadChannel(QProcess::StandardOutput);
    // FFmpeg writes to stderr only for some reason, so we just read both channels together
    ffmpeg.setProcessChannelMode(QProcess::MergedChannels);
    ffmpeg.start(strCmd);
    if (ffmpeg.waitForStarted() == true)
    {
        while(ffmpeg.state() == QProcess::Running)
        {
            if(!ffmpeg.waitForReadyRead()) break;

            QString output(ffmpeg.readAll());
            QStringList sList = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
            for (const QString& s : sList)
            {
                qDebug() << "[stdout]" << s;
            }

            if(output.startsWith("frame="))
            {
                QString frame = output.mid(6, output.indexOf(' '));

                progress(frame.toInt() / (float)(mDesc.endFrame - mDesc.startFrame));
            }
        }

        QString output(ffmpeg.readAll());
        QStringList sList = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        for (const QString& s : sList)
        {
            qDebug() << "[ffmpeg]" << s;
        }

        if(ffmpeg.exitStatus() != QProcess::NormalExit)
        {
            qDebug() << "ERROR: FFmpeg crashed";
            return Status::FAIL;
        }
    }
    else
    {
        qDebug() << "ERROR: Could not execute FFmpeg.";
        return Status::FAIL;
    }
    return Status::OK;
}

/** Runs the specified command (should be ffmpeg), and lets
 *  writeFrame pipe data into it 1 frame at a time.
 *
 *  @param[in]  strCmd A string containing the command to execute and
 *              all of its arguments
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
 *  1. Spawn process with the command from strCmd
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
Status MovieExporter::executeFFMpegPipe(QString strCmd, std::function<void(float)> progress, std::function<bool(QProcess&, int)> writeFrame)
{
    qDebug() << strCmd;

    QProcess ffmpeg;
    ffmpeg.setReadChannel(QProcess::StandardOutput);
    // FFmpeg writes to stderr only for some reason, so we just read both channels together
    ffmpeg.setProcessChannelMode(QProcess::MergedChannels);
    ffmpeg.start(strCmd);
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

                const float percentGenerated = framesGenerated / (float)(frameEnd - frameStart);
                const float percentConverted = lastFrameProcessed / (float)(frameEnd - frameStart);
                progress((percentGenerated + percentConverted) / 2);
            }
            const float percentGenerated = framesGenerated / (float)(frameEnd - frameStart);
            const float percentConverted = lastFrameProcessed / (float)(frameEnd - frameStart);
            progress((percentGenerated + percentConverted) / 2);
        }

        QString output(ffmpeg.readAll());
        QStringList sList = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        for (const QString& s : sList)
        {
            qDebug() << "[ffmpeg]" << s;
        }

        if(ffmpeg.exitStatus() != QProcess::NormalExit)
        {
            qDebug() << "ERROR: FFmpeg crashed";
            return Status::FAIL;
        }
    }
    else
    {
        qDebug() << "ERROR: Could not start FFmpeg.";
        return Status::FAIL;
    }

    return Status::OK;
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
