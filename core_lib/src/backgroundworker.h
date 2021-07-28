#ifndef BACKGROUNDWORKER_H
#define BACKGROUNDWORKER_H

#include <QObject>
#include <QDomDocument>

class BackgroundWorker : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundWorker();

public slots:
    void writeXMLAsync(const QDomDocument& doc, const QString filePath);

signals:
    void writeXMLAsyncDone(bool ok, const QString& result);

};


#endif // BACKGROUNDWORKER_H
