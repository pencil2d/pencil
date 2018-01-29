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

#include <vector>
#include <cstdint>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QApplication>
#include <QStandardPaths>
#include "object.h"
#include "layercamera.h"
#include "layersound.h"
#include "soundclip.h"

#define IMAGE_FILENAME "/test_img_%05d.bmp"

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

Status MovieExporter::run(const Object* obj,
                          const ExportMovieDesc& desc,
                          std::function<void(float, float)> majorProgress,
                          std::function<void(float)> minorProgress,
                          std::function<void(const char *)> progressMessage)
{
    majorProgress(0.f, 0.03f);
    minorProgress(0.f);
    progressMessage("Checking environment...");

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
    majorProgress(0.03f, 0.1f);

    if (!desc.strFileName.endsWith("gif"))
    {
        progressMessage("Assembling audio...");
        minorProgress(0.f);
        STATUS_CHECK(assembleAudio(obj, ffmpegPath, minorProgress));
        minorProgress(1.f);
    }

    minorProgress(0.f);
    if (desc.strFileName.endsWith("gif", Qt::CaseInsensitive))
    {
        majorProgress(0.1f, 0.5f);
        progressMessage("Generating frames...");
        STATUS_CHECK(generateImageSequence(obj, minorProgress));
        minorProgress(1.f);

        progressMessage("Generating palette...");
        majorProgress(0.5f, 0.6f);
        STATUS_CHECK(generatePalette(ffmpegPath, minorProgress));
        minorProgress(1.f);

        progressMessage("Combining...");
        majorProgress(0.6f, 1.f);
        minorProgress(0.f);
        STATUS_CHECK(convertToGif(ffmpegPath, desc.strFileName, minorProgress));
    }
    else
    {
        majorProgress(0.1f, 1.f);
        progressMessage("Combining...");
        combineVideoAndAudio(obj, ffmpegPath, desc.strFileName, minorProgress);
    }
    minorProgress(1.f);
    majorProgress(1.f, 1.f);
    progressMessage("Done");

    return Status::OK;
}

QString MovieExporter::error()
{
    return QString();
}

Status MovieExporter::assembleAudio(const Object* obj,
                                    QString ffmpegPath,
                                    std::function<void(float)> progress)
{
    // Quicktime assemble call
    int startFrame = mDesc.startFrame;
    int endFrame = mDesc.endFrame;
    int fps = mDesc.fps;

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

        executeFFMpegCommand(strCmd, [](float){});
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

Status MovieExporter::generateImageSequence(
    const Object* obj,
    std::function<void(float)> progress)
{
    int frameStart = mDesc.startFrame;
    int frameEnd = mDesc.endFrame;
    QSize exportSize = mDesc.exportSize;
    bool transparency = false;
    QString strCameraName = mDesc.strCameraName;

    auto cameraLayer = (LayerCamera*)obj->findLayerByName(strCameraName, Layer::CAMERA);
    if (cameraLayer == nullptr)
    {
        cameraLayer = obj->getLayersByType< LayerCamera >().front();
    }

    for (int currentFrame = frameStart; currentFrame <= frameEnd; currentFrame++)
    {
        if (mCanceled)
        {
            return Status::CANCELED;
        }

        QImage imageToExport(exportSize, QImage::Format_ARGB32_Premultiplied);
        QColor bgColor = Qt::white;
        if (transparency)
        {
            bgColor.setAlpha(0);
        }
        imageToExport.fill(bgColor);

        QPainter painter(&imageToExport);

        QTransform view = cameraLayer->getViewAtFrame(currentFrame);

        QSize camSize = cameraLayer->getViewSize();
        QTransform centralizeCamera;
        centralizeCamera.translate(camSize.width() / 2, camSize.height() / 2);

        painter.setWorldTransform(view * centralizeCamera);
        painter.setWindow(QRect(0, 0, camSize.width(), camSize.height()));

        obj->paintImage(painter, currentFrame, false, true);

        QString imageFileWithFrameNumber = QString().sprintf(IMAGE_FILENAME, currentFrame);

        QString strImgPath = mTempWorkDir + imageFileWithFrameNumber;
        bool bSave = imageToExport.save(strImgPath, "BMP", 100);
        Q_ASSERT(bSave);
        qDebug() << "Save img to: " << strImgPath << ", Success=" << bSave;

        progress(currentFrame / (float)(frameEnd - frameStart));
    }

    return Status::OK;
}

Status MovieExporter::combineVideoAndAudio(
        const Object* obj,
        QString ffmpegPath,
        QString strOutputFile,
        std::function<void(float)>  progress)
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

    auto cameraLayer = (LayerCamera*)obj->findLayerByName(strCameraName, Layer::CAMERA);
    if (cameraLayer == nullptr)
    {
        cameraLayer = obj->getLayersByType< LayerCamera >().front();
    }
    int currentFrame = frameStart;

    // Build FFmpeg command

    //int exportFps = mDesc.videoFps;
    const QString tempAudioPath = mTempWorkDir + "/tmpaudio.wav";

    QString strCmd = QString("\"%1\"").arg(ffmpegPath);
    strCmd += QString(" -f image2pipe");
    strCmd += QString(" -framerate %1").arg(mDesc.fps);

    strCmd += QString(" -start_number %1").arg(mDesc.startFrame);
    //strCmd += QString( " -r %1").arg( exportFps );
    strCmd += QString(" -i -");
    strCmd += QString(" -threads %1").arg(QThread::idealThreadCount() == 1 ? 0 : QThread::idealThreadCount());

    if (QFile::exists(tempAudioPath))
    {
        strCmd += QString(" -i \"%1\" ").arg(tempAudioPath);
    }

    strCmd += QString(" -s %1x%2").arg(exportSize.width()).arg(exportSize.height());

    if (strOutputFile.endsWith("mp4", Qt::CaseInsensitive))
    {
        strCmd += QString(" -pix_fmt yuv420p");
    }
    strCmd += " -y";
    strCmd += QString(" \"%1\"").arg(strOutputFile);

    // Start FFmpeg

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
            if (mCanceled)
            {
                ffmpeg.terminate();
                return Status::CANCELED;
            }

            // Check FFmpeg progress

            ffmpeg.waitForReadyRead();
            QString output(ffmpeg.readAll());
            int framesProcessed = 0;
            qDebug() << output;
            if(output.startsWith("frame="))
            {
                framesProcessed = output.mid(6, output.indexOf(' ')).toInt();
                progress(framesProcessed / (float)(frameEnd - frameStart));
            }

            // Generate frame

            if(currentFrame > frameEnd)
            {
                if(ffmpeg.isWritable())
                {
                    ffmpeg.closeWriteChannel();
                }
                continue;
            }

            while(currentFrame - frameStart <= framesProcessed + 10)
            {
                QImage imageToExport(exportSize, QImage::Format_ARGB32_Premultiplied);
                QColor bgColor = Qt::white;
                if (transparency)
                {
                    bgColor.setAlpha(0);
                }
                imageToExport.fill(bgColor);

                QPainter painter(&imageToExport);

                QTransform view = cameraLayer->getViewAtFrame(currentFrame);

                QSize camSize = cameraLayer->getViewSize();
                QTransform centralizeCamera;
                centralizeCamera.translate(camSize.width() / 2, camSize.height() / 2);

                painter.setWorldTransform(view * centralizeCamera);
                painter.setWindow(QRect(0, 0, camSize.width(), camSize.height()));

                obj->paintImage(painter, currentFrame, false, true);

                QByteArray imgData;
                QBuffer buffer(&imgData);
                bool bSave = imageToExport.save(&buffer, "BMP", 100);
                Q_ASSERT(bSave);
                ffmpeg.write(imgData);

                currentFrame++;
            }
        }

        qDebug() << ffmpeg.readAll();
    }
    else
    {
        qDebug() << "ERROR: Could not execute FFmpeg.";
        return Status::FAIL;
    }

    return Status::OK;
}

Status MovieExporter::generatePalette(QString ffmpeg, std::function<void(float)> progress)
{
    if (mCanceled)
    {
        return Status::CANCELED;
    }

    const QString imgPath = mTempWorkDir + IMAGE_FILENAME;

    QString strCmd = QString("\"%1\"").arg(ffmpeg);
    strCmd += QString(" -framerate %1").arg(mDesc.fps);

    strCmd += QString(" -start_number %1").arg(mDesc.startFrame);
    strCmd += QString(" -i \"%1\"").arg(imgPath);

    strCmd += " -y";

    // http://superuser.com/questions/556029/
    // generate a palette
    QString strGifPalette = mTempWorkDir + "/palette.png";
    strCmd += " -vf palettegen";
    strCmd += QString(" \"%1\"").arg(strGifPalette);

    STATUS_CHECK(executeFFMpegCommand(strCmd, progress));

    return Status::OK;
}

Status MovieExporter::convertToGif(QString ffmpeg, QString strOut, std::function<void(float)> progress)
{

    if (mCanceled)
    {
        return Status::CANCELED;
    }

    const QString imgPath = mTempWorkDir + IMAGE_FILENAME;
    const QSize exportSize = mDesc.exportSize;

    QString strCmd = QString("\"%1\"").arg(ffmpeg);
    strCmd += QString(" -framerate %1").arg(mDesc.fps);

    strCmd += QString(" -start_number %1").arg(mDesc.startFrame);
    strCmd += QString(" -i \"%1\"").arg(imgPath);

    strCmd += " -y";

    // Output the GIF using the palette:
    strCmd += QString(" -i \"%1\"").arg(mTempWorkDir + "/palette.png");

    strCmd += QString(" -s %1x%2").arg(exportSize.width()).arg(exportSize.height());

    strCmd += " -filter_complex \"paletteuse\"";
    strCmd += QString(" \"%1\"").arg(strOut);

    STATUS_CHECK(executeFFMpegCommand(strCmd, progress));

    return Status::OK;
}

Status MovieExporter::executeFFMpegCommand(QString strCmd, std::function<void(float)> progress)
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

            qDebug() << "Running!";

            QString output(ffmpeg.readAll());
            qDebug() << output;

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
            qDebug() << "[stdout]" << s;
        }

        QString stdErrMsg(ffmpeg.readAllStandardError());
        sList = stdErrMsg.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        for (const QString& s : sList)
        {
            qDebug() << "[stderr]" << s;
        }
    }
    else
    {
        qDebug() << "ERROR: Could not execute FFmpeg.";
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
