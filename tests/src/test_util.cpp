/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "catch.hpp"

#include "util.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QTemporaryDir>
#include <QTemporaryFile>

TEST_CASE("closestCanonicalPath")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());

    SECTION("Existing file returns canonical path")
    {
        QTemporaryFile file(dir.filePath("file.XXXXXX"));
        REQUIRE(file.open());
        QString expected = QFileInfo(file.fileName()).canonicalFilePath();
        REQUIRE(closestCanonicalPath(file.fileName()) == expected);
    }

    SECTION("Non-existing file in existing directory resolves parent canonically")
    {
        QString filePath = dir.filePath("nonexistent.txt");
        QString result = closestCanonicalPath(filePath);
        // The existing parent (dir) should have been canonicalized
        QString canonicalDir = QFileInfo(dir.path()).canonicalFilePath();
        REQUIRE(result.startsWith(canonicalDir));
        REQUIRE(result.endsWith("nonexistent.txt"));
    }

    SECTION("Path with .. is normalized")
    {
        QString filePath = dir.filePath("subdir/../file.txt");
        QString result = closestCanonicalPath(filePath);
        // Should simplify to dir/file.txt (no .. in result)
        REQUIRE(!result.contains(".."));
        REQUIRE(result == closestCanonicalPath(dir.filePath("file.txt")));
    }

    SECTION("Absolute path is returned as-is for existing entry")
    {
        QString absPath = QFileInfo(dir.path()).canonicalFilePath();
        REQUIRE(closestCanonicalPath(absPath) == absPath);
    }

#ifdef Q_OS_UNIX
    SECTION("Dangling symlink returns empty string")
    {
        QString linkPath = dir.filePath("dangling_link");
        QString missingTarget = dir.filePath("does_not_exist");
        REQUIRE(QFile::link(missingTarget, linkPath));
        REQUIRE(closestCanonicalPath(linkPath).isEmpty());
    }

    SECTION("Symlink to existing file resolves to target")
    {
        QTemporaryFile target(dir.filePath("target.XXXXXX"));
        REQUIRE(target.open());
        QString linkPath = dir.filePath("link_to_target");
        REQUIRE(QFile::link(target.fileName(), linkPath));
        QString result = closestCanonicalPath(linkPath);
        REQUIRE(result == QFileInfo(target.fileName()).canonicalFilePath());
    }
#endif
}

TEST_CASE("validateDataPath")
{
    QTemporaryDir dataDir;
    REQUIRE(dataDir.isValid());

    // Create a real file to allow canonical resolution in some tests
    QFile realFile(dataDir.filePath("frame.png"));
    REQUIRE(realFile.open(QIODevice::WriteOnly));
    realFile.close();

    SECTION("Normal relative path inside data dir is accepted")
    {
        QString result = validateDataPath("frame.png", dataDir.path());
        REQUIRE(!result.isEmpty());
        REQUIRE(QFileInfo(result).canonicalFilePath() == QFileInfo(dataDir.filePath("frame.png")).canonicalFilePath());
    }

    SECTION("Nested relative path inside data dir is accepted")
    {
        REQUIRE(QDir(dataDir.path()).mkdir("sub"));
        QFile nested(dataDir.filePath("sub/frame.png"));
        REQUIRE(nested.open(QIODevice::WriteOnly));
        nested.close();

        QString result = validateDataPath("sub/frame.png", dataDir.path());
        REQUIRE(!result.isEmpty());
        REQUIRE(result.contains("sub"));
    }

    SECTION("Non-existing relative path inside data dir is accepted")
    {
        // validateDataPath does not require the file to exist
        QString result = validateDataPath("nonexistent.png", dataDir.path());
        REQUIRE(!result.isEmpty());
    }

    SECTION("Absolute path is rejected")
    {
        QString absPath = dataDir.filePath("frame.png");
        REQUIRE(validateDataPath(absPath, dataDir.path()).isEmpty());
    }

    SECTION("Path traversal with .. escaping data dir is rejected")
    {
        // ../outside.png would resolve to the parent of dataDir
        QString result = validateDataPath("../outside.png", dataDir.path());
        REQUIRE(result.isEmpty());
    }

    SECTION("Path traversal via nested .. is rejected")
    {
        QString result = validateDataPath("sub/../../outside.png", dataDir.path());
        REQUIRE(result.isEmpty());
    }

    SECTION("Sibling directory with matching prefix is rejected")
    {
        // A directory named <dataDir>evil must not be confused as a child
        // e.g. canonicalDataDir="/tmp/abc" must not match "/tmp/abcevil/frame.png"
        QTemporaryDir siblingDir;
        REQUIRE(siblingDir.isValid());

        // Craft a path that would resolve into the sibling dir via relative navigation
        QString relativeToDataDir = QDir(dataDir.path()).relativeFilePath(siblingDir.filePath("frame.png"));
        QString result = validateDataPath(relativeToDataDir, dataDir.path());
        REQUIRE(result.isEmpty());
    }

#ifdef Q_OS_UNIX
    SECTION("Symlink inside data dir pointing outside is rejected")
    {
        QTemporaryDir outsideDir;
        REQUIRE(outsideDir.isValid());
        QFile outsideFile(outsideDir.filePath("secret.dat"));
        REQUIRE(outsideFile.open(QIODevice::WriteOnly));
        outsideFile.close();

        QString linkPath = dataDir.filePath("link");
        REQUIRE(QFile::link(outsideDir.path(), linkPath));

        // "link/secret.dat" looks like it's inside dataDir but resolves outside
        QString result = validateDataPath("link/secret.dat", dataDir.path());
        REQUIRE(result.isEmpty());
    }

    SECTION("Dangling symlink inside data dir is rejected")
    {
        QString linkPath = dataDir.filePath("dangling");
        REQUIRE(QFile::link(dataDir.filePath("does_not_exist"), linkPath));

        QString result = validateDataPath("dangling", dataDir.path());
        REQUIRE(result.isEmpty());
    }
#endif

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    SECTION("Case-insensitive: different case path inside data dir is accepted")
    {
        // On case-insensitive filesystems the comparison should still pass
        // when only the casing differs
        QString upperName = "FRAME.PNG";
        QString result = validateDataPath(upperName, dataDir.path());
        // The file "frame.png" exists; on a case-insensitive FS this resolves to it
        REQUIRE(!result.isEmpty());
    }
#endif
}
