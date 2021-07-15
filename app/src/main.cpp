/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <clocale>

#include <QSettings>

#include "log.h"
#include "pencil2d.h"
#include "pencildef.h"
#include "pencilerror.h"
#include "platformhandler.h"

/**
 * This is the entrypoint of the program. It performs basic initialization, then
 * boots the actual application (@ref Pencil2D).
 */
int main(int argc, char* argv[])
{
    // iss #940
    // Force dot separator on numbers because some localizations
    // use comma as separator.
    std::setlocale(LC_NUMERIC, "en_US.UTF-8");

    Q_INIT_RESOURCE(core_lib);
    PlatformHandler::initialise();
    initCategoryLogging();

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

    Pencil2D app(argc, argv);
    switch (app.handleCommandLineOptions().code())
    {
        case Status::OK:
            return Pencil2D::exec();
        case Status::SAFE:
            return EXIT_SUCCESS;
        default:
            return EXIT_FAILURE;
    }
}
