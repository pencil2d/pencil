/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang
Copyright (C) 2026-2099 Oliver S. Larsen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "catch.hpp"
#include "qminiz.h"

#include <cstring>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

// https://github.com/pencil2d/pencil/issues/2009
TEST_CASE("QMiniZ::CompressFolder")
{
    SECTION("Verify robustness against paths with different roots")
    {
        QTemporaryDir tempDir;
        REQUIRE(tempDir.isValid());

        QString subFolder = tempDir.path() + "/bar/";
        QDir().mkpath(subFolder);
        QFile file(subFolder + "biz.mp3");
        file.open(QIODevice::WriteOnly);
        file.close();

        QString zipPath = tempDir.path() + "/test.pclx";

        // Simulate root mismatch by using canonical path for file
        // but non-canonical path for the base
        QString canonicalFile = QFileInfo(subFolder + "biz.mp3").canonicalFilePath();
        QString nonCanonicalBase = tempDir.path();

        QList<QString> filesToZip = { canonicalFile };

        Status st = MiniZ::compressFolder(zipPath, nonCanonicalBase, filesToZip, "application/x-pencil2d-pclx");
        REQUIRE(st.ok());

        mz_zip_archive zip;
        memset(&zip, 0, sizeof(zip));
        REQUIRE(mz_zip_reader_init_file(&zip, zipPath.toUtf8().data(), 0));

        int fileIndex = mz_zip_reader_locate_file(&zip, "bar/biz.mp3", nullptr, 0);
        REQUIRE(fileIndex >= 0);

        mz_zip_reader_end(&zip);
    }
}
