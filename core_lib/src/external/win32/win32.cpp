/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2009 Mj Mendoza IV
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <cstdint>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QString>
#include <QProgressDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QSettings>
#include <QDebug>

#include "object.h"
#include "editor.h"
#include "layersound.h"
#include "platformhandler.h"

namespace PlatformHandler
{
    void configurePlatformSpecificSettings() {}
}
