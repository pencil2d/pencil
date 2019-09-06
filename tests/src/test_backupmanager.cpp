/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

// test header
#include "catch.hpp"

// required qt headers...
#include "QDebug"


// pencil2d headers
#include "editor.h"
#include "bitmapimage.h"
#include "layerbitmap.h"
#include "backupmanager.h"
#include "backupelement.h"
#include "object.h"

class QUndoStack;

TEST_CASE("Backupmanager setup")
{
    Object* object = new Object;
    Editor* editor = new Editor;
    editor->setObject(object);

    BackupManager* backups = new BackupManager(editor);
    SECTION("Init Undostack")
    {

        REQUIRE(editor != nullptr);
        object->init();
        backups->init();
    }

    // Doesn't work because of scribblearea...
//    SECTION("Try saveStates")
//    {
//        backups->saveStates();
//    }
}
