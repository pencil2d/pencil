/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

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
