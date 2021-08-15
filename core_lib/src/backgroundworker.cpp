#include "backgroundworker.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDomDocument>
#include <QThread>
#include "bitmapimage.h"
#include "vectorimage.h"


// The object cannot be moved to a thread if it has a parent, so leave it as null
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

void BackgroundWorker::writeKeyFrameAsync(KeyFrame* key, const QString filePath)
{
    auto bitmapImg = dynamic_cast<BitmapImage*>(key);
    if (bitmapImg)
    {
        Q_ASSERT(filePath.endsWith(".png"));
        Status st = bitmapImg->writeFile(filePath);

        QString msg = QString("Done writing %1").arg(filePath);
        emit writeKeyFrameDone(st.ok(), msg);
    }
    auto vecImg = dynamic_cast<VectorImage*>(key);
    if (vecImg)
    {
        Q_ASSERT(filePath.endsWith(".vec"));
        Status st = vecImg->write(filePath, "VEC");

        QString msg = QString("Done Writing %1").arg(filePath);
        emit writeKeyFrameDone(st.ok(), msg);
    }
    delete key;
}
