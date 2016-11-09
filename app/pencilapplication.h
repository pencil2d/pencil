#ifndef PENCILAPPLICATION_H
#define PENCILAPPLICATION_H

#include <QApplication>

class PencilApplication : public QApplication
{
    Q_OBJECT

public:
    PencilApplication(int &argc, char **argv);

    bool event(QEvent* event);
    void emitOpenFileRequest();

signals:
    void openFileRequested(QString filename);

private:
    QString mStartPath;
};

#endif // PENCILAPPLICATION_H
