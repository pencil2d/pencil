#ifndef MOVIEIMPORTER_H
#define MOVIEIMPORTER_H

#include "pencilerror.h"

#include <QObject>
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

    /** Test the movie before importing, this is useful because you can
     * use the information to know how many frames will be imported (estimated) and whether to inform the user or not about it.
     *
     * @param filePath
     * @param fps
     * @return A status, either OK, FAIL or AWAIT
     */
    Status testVideo(const QString& filePath, int fps);

    /**
     *
     * @param filePath path to File
     * @param fps frames per second
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


    /** Only valid if test() has been used, otherwise will return 0.
     *
     * @return Estimated amount of frames to import
     */
    int estimatedFrames();

    void cancel() { mCanceled = true; }

private:

    Status verifyFFMPEG();
    Status importMovieVideo(const QString& filePath, int fps,
                            std::function<void(int)> progress,
                            std::function<void(QString)> progressMessage);
    Status importMovieAudio(const QString& filePath, std::function<void(int)> progress);

    Status generateFrames(std::function<void(int)> progress);

    Editor* mEditor = nullptr;

    QTemporaryDir* mTempDir = nullptr;

    bool mCanceled = false;

    int mEstimatedFrames = 0;
};

#endif // MOVIEIMPORTER_H
