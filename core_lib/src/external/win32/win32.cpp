/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2009 Mj Mendoza IV
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "platformhandler.h"

#include <QCoreApplication>
#include <QSettings>

#include <ShObjIdl.h>

#include "pencildef.h"

namespace PlatformHandler
{
    void configurePlatformSpecificSettings() {}
    bool isDarkMode() { return false; };
    void initialise()
    {
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
#if defined(PENCIL2D_RELEASE_BUILD)
        SetCurrentProcessExplicitAppUserModelID(L"Pencil2D.Pencil2D.Release");
#elif defined(PENCIL2D_NIGHTLY_BUILD)
        SetCurrentProcessExplicitAppUserModelID(L"Pencil2D.Pencil2D.Nightly");
#else
        SetCurrentProcessExplicitAppUserModelID(L"Pencil2D.Pencil2D");
#endif
#endif // _WIN32_WINNT >= _WIN32_WINNT_WIN7

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        // Temporary solution for high DPI displays
        // EnableHighDpiScaling is a just in case mechanism in the event that we
        // want to disable this without recompiling, see #922
        QSettings settings(PENCIL2D, PENCIL2D);
        if (settings.value("EnableHighDpiScaling", "true").toBool())
        {
            // Enable auto screen scaling on high dpi display, for example, a 4k monitor
            // This attr has to be set before the QApplication is constructed
            // Only works on Windows & X11
            QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        }
#endif // QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    };
}
