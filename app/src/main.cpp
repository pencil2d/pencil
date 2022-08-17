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

#include "log.h"
#include "pencil2d.h"
#include "pencilerror.h"
#include "platformhandler.h"

/**
 * This is the entrypoint of the program. It performs basic initialization, then
 * boots the actual application (@ref Pencil2D).
 */
int main(int argc, char* argv[])
{
    Q_INIT_RESOURCE(core_lib);
    PlatformHandler::initialise();
    initCategoryLogging();

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
