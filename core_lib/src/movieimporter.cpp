#include "movieimporter.h"

#include <QDebug>
#include <QTemporaryDir>
#include <QProcess>
#include <QtMath>
#include <QTime>

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

Status MovieImporter::testVideo(const QString& filePath, int fps)
{
    Status status = Status::OK;
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() != Layer::BITMAP)
    {
        status = Status::FAIL;
        status.setTitle(QObject::tr("Bitmap only"));
        status.setDescription(QObject::tr("You need to be on the bitmap layer to import a movie clip"));
        return status;
    }

    // --------- Import all the temporary frames ----------
    status = verifyFFmpegExists();
    if (status == Status::ERROR_FFMPEG_NOT_FOUND) { return status; }
    QString ffmpegPath = ffmpegLocation();

    // Get frame estimate
    int frames = 1;
    QString ffprobePath = ffprobeLocation();
    if (QFileInfo::exists(ffprobePath))
    {
        QString probeCmd = QString("\"%1\"").arg(ffprobePath);
        probeCmd += QString(" -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1");
        probeCmd += QString(" \"%1\"").arg(filePath);
        QProcess ffprobe;
        ffprobe.setReadChannel(QProcess::StandardOutput);
        ffprobe.start(probeCmd);
        ffprobe.waitForFinished();
        if (ffprobe.exitStatus() == QProcess::NormalExit)
        {
            double seconds = QString(ffprobe.readAll()).toDouble();
            frames = qCeil(seconds * fps);
        }
    }
    else
    {
        // Fallback to ffmpeg
        QString probeCmd = QString("\"%1\"").arg(ffmpegPath);
        probeCmd += QString(" -i \"%1\"").arg(filePath);
        QProcess ffmpeg;
        // FFmpeg writes to stderr only for some reason, so we just read both channels together
        ffmpeg.setProcessChannelMode(QProcess::MergedChannels);
        ffmpeg.start(probeCmd);
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

    mEstimatedFrames = frames;

    if (mEditor->currentFrame() + frames > MaxFramesBound) {
        status = Status::CANCELED;
        status.setTitle(QObject::tr("Imported movie too big!"));
        status.setDescription(QObject::tr("The movie clip exceeds max number of frames, which is") +
                                          QString(" %1. %2")
                                          .arg(MaxFramesBound)
                                          .arg("Canceling import..."));

        return status;
    }
    else if(frames > 200) {
        status = Status::AWAIT;
        status.setTitle(QObject::tr("200 Frames and beyond"));
        status.setDescription(QObject::tr("This is a warning that a lot of frames will be imported, are you sure you want to proceed?"));
        return status;
    }

    return status;
}

Status MovieImporter::run(const QString& filePath, int fps, FileType type,
                          std::function<void(int)> progress,
                          std::function<void(QString)> progressMessage,
                          std::function<bool()> askPermission)
{
    if (mCanceled) return Status::CANCELED;

    Status status = Status::OK;

    status = verifyFFmpegExists();
    if (status == Status::ERROR_FFMPEG_NOT_FOUND) { return status; }

    mTempDir = new QTemporaryDir();
    if (!mTempDir->isValid())
    {
        status = Status::FAIL;
        status.setTitle(QObject::tr("Error creating folder"));
        status.setDescription(QObject::tr("Was not able to create a temporary folder, "
                                   "the following error was given:") + mTempDir->errorString());
        return status;
    }
    mEditor->addTemporaryDir(mTempDir);

    if (type == FileType::MOVIE) {

        status = testVideo(filePath, fps);

        if (status == Status::AWAIT) {
            bool canProceed = askPermission();

            if (!canProceed) { return Status::CANCELED; }

        } else if (!status.ok()) { return status; }

        return importMovieVideo(filePath, fps, [&progress, this](int prog) {
            progress(prog); return !mCanceled;
        }, [&progressMessage](QString message) {
            progressMessage(message);
        });
    } else if (type == FileType::SOUND) {
        return importMovieAudio(filePath, [&progress, this](int prog) {
            progress(prog); return !mCanceled;
        });
    } else {
        Status st = Status::FAIL;
        st.setTitle(tr("Unknown error"));
        st.setTitle(tr("This should not happen..."));
        return st;
    }
}

Status MovieImporter::importMovieVideo(const QString& filePath, int fps,
                                       std::function<void(int)> progress,
                                       std::function<void(QString)> progressMessage)
{
    Status status = Status::OK;

    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() != Layer::BITMAP)
    {
        status = Status::FAIL;
        status.setTitle(QObject::tr("Bitmap only"));
        status.setDescription(QObject::tr("You need to be on the bitmap layer to import a movie clip"));
        return status;
    }

    QString ffmpegPath = ffmpegLocation();
    QString strCmd = QString("\"%1\"").arg(ffmpegPath);
    strCmd += QString(" -i \"%1\"").arg(filePath);
    strCmd += QString(" -r %1").arg(fps);
    strCmd += QString(" \"%1\"").arg(mTempDir->filePath("%05d.png"));

    status = MovieExporter::executeFFMpeg(strCmd, mEstimatedFrames, [&progress, this] (double f) {
        progress(qFloor(qMin(f, 1.0) * 50)); return !mCanceled; }
    );

    if (!status.ok() && status != Status::CANCELED) { return status; }

    if(mCanceled) return Status::CANCELED;

    progressMessage(tr("Video processed, adding frames..."));

    progress(50);

    return generateFrames([this, &progress](int prog) {
        progress(prog); return mCanceled;
    });
}

Status MovieImporter::generateFrames(std::function<void (int)> progress)
{
    Layer* layer = mEditor->layers()->currentLayer();
    Status status = Status::OK;
    int i = 1;
    auto amountOfFrames = QDir(mTempDir->path()).count();
    QString currentFile(mTempDir->filePath(QString("%1.png").arg(i, 5, 10, QChar('0'))));
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
        currentFile = mTempDir->filePath(QString("%1.png").arg(i, 5, 10, QChar('0')));
    }

    if (!QFileInfo::exists(mTempDir->filePath("00001.png"))) {
        status = Status::FAIL;
        status.setTitle(tr("Failed import"));
        status.setDescription(tr("Was unable to find internal files, import unsucessful."));
        return status;
    }

    return status;
}

Status MovieImporter::importMovieAudio(const QString& filePath, std::function<void(int)> progress)
{
    Layer* layer = mEditor->layers()->currentLayer();

    Status status = Status::OK;
    if (layer->type() != Layer::SOUND)
    {
        status = Status::FAIL;
        status.setTitle(QObject::tr("Sound only"));
        status.setDescription(QObject::tr("You need to be on a sound layer to import the audio from the movieclip"));
        return status;
    }

    int currentFrame = mEditor->currentFrame();

    if (layer->keyExists(currentFrame))
    {
        SoundClip* key = static_cast<SoundClip*>(layer->getKeyFrameAt(currentFrame));
        if (!key->fileName().isEmpty())
        {
            status = Status::FAIL;
            status.setTitle(QObject::tr("Move to an empty frame"));
            status.setDescription(QObject::tr("A frame already exists on frame: ") + QString::number(currentFrame) + tr(" Move the scrubber to a empty position on the timeline and try again"));
            return status;
        }
    }

    QString audioPath = mTempDir->filePath("audio.wav");

    QString ffmpegPath = ffmpegLocation();
    QString strCmd = QString("\"%1\"").arg(ffmpegPath);
    strCmd += QString(" -i \"%1\"").arg(filePath);
    strCmd += QString(" \"%1\"").arg(audioPath);

    status = MovieExporter::executeFFMpeg(strCmd, mEstimatedFrames, [&progress, this] (double f) {
        progress(qFloor(qMin(f, 1.0) * 50)); return !mCanceled; }
    );

    if(mCanceled) return Status::CANCELED;
    progress(90);

    SoundClip* key = nullptr;

    Q_ASSERT(!layer->keyExists(currentFrame));

    key = new SoundClip();
    layer->addKeyFrame(currentFrame, key);

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
        status.setTitle(QObject::tr("FFMPEG Not found"));
        status.setDescription(QObject::tr("Please place the ffmpeg binary in plugins directory and try again"));
        return status;
    }
    return Status::OK;
}
