#include "backgroundworker.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDomDocument>
#include <QThread>

// The parent is intentionally set as null because The object cannot be moved to a thread if it has a parent.
// ref: https://doc.qt.io/qt-5/qobject.html#moveToThread
BackgroundWorker::BackgroundWorker() : QObject(nullptr)
{

}

void BackgroundWorker::writeXMLAsync(const QDomDocument& doc, const QString filePath)
{
    Q_ASSERT(!filePath.isEmpty());

    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        emit writeXMLAsyncDone(false, "Cannot open the file");
        return;
    }

    const int indentSize = 2;

    QTextStream out(&file);
    doc.save(out, indentSize);
    out.flush();
    file.close();
    emit writeXMLAsyncDone(true, QString("Done writing %1").arg(filePath));
}
