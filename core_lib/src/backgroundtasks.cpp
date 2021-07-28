#include "backgroundtasks.h"
#include <QDebug>
#include <QFile>
#include "object.h"
#include "filemanager.h"
#include "backgroundworker.h"



BackgroundTasks::BackgroundTasks(QObject* parent) : QObject(parent)
{
	// Need to register QDomDocument so it can be used in multi-threaded signal/slot connections
	qRegisterMetaType<QDomDocument>("QDomDocument");

	mWorker = new BackgroundWorker;
	mWorker->moveToThread(&mWorkerThread);
    connect(this, &BackgroundTasks::writeXmlAsync, mWorker, &BackgroundWorker::writeXMLAsync);
    connect(mWorker, &BackgroundWorker::writeXMLAsyncDone, this, &BackgroundTasks::writeXMLAsyncDone);
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
