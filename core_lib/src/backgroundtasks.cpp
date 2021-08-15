#include "backgroundtasks.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include "object.h"
#include "filemanager.h"
#include "backgroundworker.h"
#include "keyframe.h"


BackgroundTasks::BackgroundTasks(QObject* parent) : QObject(parent)
{
    // Need to register QDomDocument so it can be used in multi-threaded signal/slot connections
    qRegisterMetaType<QDomDocument>("QDomDocument");

    mWorker = new BackgroundWorker;
    mWorker->moveToThread(&mWorkerThread);
    connect(this, &BackgroundTasks::writeXmlAsync, mWorker, &BackgroundWorker::writeXMLAsync);
    connect(mWorker, &BackgroundWorker::writeXMLAsyncDone, this, &BackgroundTasks::writeXMLAsyncDone);

    connect(this, &BackgroundTasks::writeCurrentFrameAsync, mWorker, &BackgroundWorker::writeKeyFrameAsync);
    connect(mWorker, &BackgroundWorker::writeKeyFrameDone, this, &BackgroundTasks::writeCurrentFrameDone);

    mWorkerThread.start();
}

BackgroundTasks::~BackgroundTasks()
{
    mWorkerThread.quit();
    mWorkerThread.wait(); // Wait until the thread is properly terminated by OS
    delete mWorker;
}

void BackgroundTasks::writeMainXmlToWorkingFolder(const Object* object)
{
    FileManager fm;
    QDomDocument xmlDoc = fm.generateXMLFromObject(object);

    emit writeXmlAsync(xmlDoc, object->mainXMLFile());
}

void BackgroundTasks::writeXMLAsyncDone(bool ok, const QString& msg)
{
    qDebug() << ok << msg;
}

void BackgroundTasks::writeCurrentFrameToWorkingFolder(const Object* object, int layerIndex, int frame)
{
    Layer* layer = object->getLayer(layerIndex);
    KeyFrame* key = layer->getKeyFrameAt(frame);

    if (layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR)
    {
        QString keyFramePath = layer->keyFrameFilePath(key, object->dataDir());
        KeyFrame* copiedKey = key->clone();

        Q_ASSERT(QFileInfo(keyFramePath).isAbsolute());
        emit writeCurrentFrameAsync(copiedKey, keyFramePath);
    }
}

void BackgroundTasks::writeCurrentFrameDone(bool ok, const QString& msg)
{
    qDebug() << ok << msg;
}
