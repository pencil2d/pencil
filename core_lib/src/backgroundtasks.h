#ifndef BACKGROUNDTASKS_H
#define BACKGROUNDTASKS_H

#include <QObject>
#include <QThread>
#include <QDomDocument>
class Object;
class QDomDocument;
class BackgroundWorker;


class BackgroundTasks : public QObject
{
    Q_OBJECT
public:
    BackgroundTasks(QObject* parent);
    ~BackgroundTasks();

    void writeMainXmlToWorkingFolder(const Object* object);

signals:
    void writeXmlAsync(const QDomDocument& doc, const QString mainXMLPath);

private:
    void writeXMLAsyncDone(bool ok, const QString& msg);

private:
    QThread mWorkerThread;
    BackgroundWorker* mWorker = nullptr;
};

#endif // BACKGROUNDTASKS_H
