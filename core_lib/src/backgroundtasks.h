#ifndef BACKGROUNDTASKS_H
#define BACKGROUNDTASKS_H

#include <QObject>
#include <QThread>
class Object;
class QDomDocument;
class BackgroundWorker;
class KeyFrame;

class BackgroundTasks : public QObject
{
    Q_OBJECT
public:
    BackgroundTasks(QObject* parent);
    ~BackgroundTasks();

    void writeMainXmlToWorkingFolder(const Object* object);
    void writeCurrentFrameToWorkingFolder(const Object* object, int layerIndex, int frame);

signals:
    void writeXmlAsync(const QDomDocument& doc, const QString mainXMLPath);
    void writeCurrentFrameAsync(KeyFrame* key, const QString filePath);

private:
    void writeXMLAsyncDone(bool ok, const QString& msg);
    void writeCurrentFrameDone(bool ok, const QString& msg);

private:
    QThread mWorkerThread;
    BackgroundWorker* mWorker = nullptr;
};

#endif // BACKGROUNDTASKS_H
