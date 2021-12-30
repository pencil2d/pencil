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
#include <QDebug>
#include <QSettings>

#include "pencildef.h"

namespace PlatformHandler
{
    void configurePlatformSpecificSettings() {}

    void initialise()
    {
        /* If running as an AppImage, sets GStreamer environment variables to ensure
         * the plugins contained in the AppImage are found
         */
        QString appDir = QString::fromLocal8Bit(qgetenv("APPDIR"));
        if (!appDir.isEmpty())
        {
            bool success = qputenv("GST_PLUGIN_SYSTEM_PATH_1_0",
                                   QString("%1/usr/lib/gstreamer-1.0:%2")
                                       .arg(appDir, QString::fromLocal8Bit(qgetenv("GST_PLUGIN_SYSTEM_PATH_1_0")))
                                       .toLocal8Bit());
            success = qputenv("GST_PLUGIN_SCANNER_1_0",
                              QString("%1/usr/lib/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner")
                                 .arg(appDir).toLocal8Bit()) && success;
            if (!success)
            {
                qWarning() << "Unable to set up GStreamer environment";
            }
        }

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
    }
}
