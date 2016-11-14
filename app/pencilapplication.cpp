#include <QFileOpenEvent>
#include <QIcon>

#include "pencilapplication.h"

PencilApplication::PencilApplication(int& argc, char** argv) :
    QApplication(argc, argv)
{
    // Set organization and application name
    setOrganizationName("Pencil2D");
    setOrganizationDomain("pencil2d.org");
    setApplicationName("Pencil2D");
    setApplicationDisplayName("Pencil2D");

    // Set application version
    setApplicationVersion(APP_VERSION);

    // Set application icon
    setWindowIcon(QIcon(":/icons/icon.png"));
}

bool PencilApplication::event(QEvent* event)
{
    if (event->type() == QEvent::FileOpen)
    {
        mStartPath = static_cast<QFileOpenEvent*>(event)->file();
        emit openFileRequested(mStartPath);
        return true;
    }
    return QApplication::event(event);
}

void PencilApplication::emitOpenFileRequest()
{
    if (mStartPath.size() != 0)
    {
        emit openFileRequested(mStartPath);
    }
}
