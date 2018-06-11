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
#include "qminiz.h"

#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QDirIterator>
#include "miniz.h"
#include "util.h"


bool MiniZ::isZip(const QString& sZipFilePath)
{
    mz_zip_archive* mz = new mz_zip_archive;
    OnScopeExit(delete mz);
    mz_zip_zero_struct(mz);

    mz_bool ok = mz_zip_reader_init_file(mz, sZipFilePath.toUtf8().data(), 0);
    if (!ok) return false;

    int num = mz_zip_reader_get_num_files(mz);

    mz_zip_reader_end(mz);
    return (num > 0);
}

// ReSharper disable once CppInconsistentNaming
Status MiniZ::compressFolder(QString zipFilePath, QString srcFolderPath, const QStringList& fileList)
{
    DebugDetails dd;
    dd << QString("Zip the folder %1 to %2").arg(zipFilePath).arg(srcFolderPath);

    if (!srcFolderPath.endsWith("/"))
    {
        srcFolderPath.append("/");
    }


    mz_zip_archive* mz = new mz_zip_archive;
    OnScopeExit(delete mz);
    mz_zip_zero_struct(mz);

    mz_bool ok = mz_zip_writer_init_file(mz, zipFilePath.toUtf8().data(), 0);

    if (!ok)
    {
        dd << "Miniz writer init failed.";
    }

    qDebug() << "SrcFolder=" << srcFolderPath;
    for (QString filePath : fileList)
    {
        QString sRelativePath = filePath;
        sRelativePath.replace(srcFolderPath, "");

        dd << QString("Add file to zip: ").append(sRelativePath);

        ok = mz_zip_writer_add_file(mz,
                                    sRelativePath.toUtf8().data(),
                                    filePath.toUtf8().data(),
                                    "", 0, MZ_DEFAULT_COMPRESSION);
        qDebug() << "Zip: " << filePath;
        if (!ok)
        {
            dd << QString("  Cannot add %1 to zip").arg(sRelativePath);
        }
    }
    ok &= mz_zip_writer_finalize_archive(mz);
    mz_zip_writer_end(mz);

    if (!ok)
    {
        dd << "Miniz finalize archive failed";
        return Status(Status::FAIL, dd);
    }
    return Status::OK;
}

Status MiniZ::uncompressFolder(QString zipFilePath, QString destPath)
{
    DebugDetails dd;
    dd << QString("Unzip file %1 to folder %2").arg(zipFilePath).arg(destPath);

    if (!QFile::exists(zipFilePath))
    {
        return Status::FILE_NOT_FOUND;
    }

    QString sBaseDir = QFileInfo(destPath).absolutePath();
    QDir baseDir(sBaseDir);
    if (!baseDir.exists())
    {
        bool ok = baseDir.mkpath(".");
        Q_ASSERT(ok);
    }

    baseDir.makeAbsolute();

    mz_zip_archive* mz = new mz_zip_archive;
    OnScopeExit(delete mz);
    mz_zip_zero_struct(mz);

    mz_bool ok = mz_zip_reader_init_file(mz, zipFilePath.toUtf8().data(), 0);
    if (!ok)
        return Status(Status::FAIL, dd);

    int num = mz_zip_reader_get_num_files(mz);

    mz_zip_archive_file_stat* stat = new mz_zip_archive_file_stat;
    OnScopeExit(delete stat);

    for (int i = 0; i < num; ++i)
    {
        ok &= mz_zip_reader_file_stat(mz, i, stat);

        if (stat->m_is_directory)
        {
            QString sFolderPath = QString::fromUtf8(stat->m_filename);
            dd << QString("Make Dir: ").append(sFolderPath);

            bool mkDirOK = baseDir.mkpath(sFolderPath);
            Q_ASSERT(mkDirOK);
            if (!mkDirOK)
                dd << "  Make Dir failed.";
        }
    }

    for (int i = 0; i < num; ++i)
    {
        ok &= mz_zip_reader_file_stat(mz, i, stat);

        if (!stat->m_is_directory)
        {
            QString sFullPath = baseDir.filePath(QString::fromUtf8(stat->m_filename));
            dd << QString("Unzip file: ").append(sFullPath);
            bool b = QFileInfo(sFullPath).absoluteDir().mkpath(".");
            Q_ASSERT(b);

            bool extractOK = mz_zip_reader_extract_to_file(mz, i, sFullPath.toUtf8(), 0);
            if (!extractOK)
            {
                ok = false;
                dd << "  File extraction failed.";
            }
        }
    }

    mz_zip_reader_end(mz);

    if (!ok)
    {
        dd << "Unzip error!";
    }
    return Status::OK;
}
