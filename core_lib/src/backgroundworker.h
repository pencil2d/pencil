#ifndef BACKGROUNDWORKER_H
#define BACKGROUNDWORKER_H

#include <QObject>
class QDomDocument;
class KeyFrame;


class BackgroundWorker : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundWorker();

public slots:
    void writeXMLAsync(const QDomDocument& doc, const QString filePath);
    void writeKeyFrameAsync(KeyFrame* key, const QString filePath);
signals:
    void writeXMLAsyncDone(bool ok, const QString& msg);
    void writeKeyFrameDone(bool ok, const QString& msg);

};


#endif // BACKGROUNDWORKER_H
