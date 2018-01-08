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

bool MiniZ::compressFolder(QString sZipFilePath, QString sSrcPath)
{
    if (!sSrcPath.endsWith("/"))
    {
        sSrcPath.append("/");
    }

    mz_zip_archive* mz = new mz_zip_archive;
    OnScopeExit(delete mz);
    mz_zip_zero_struct(mz);

    mz_bool ok = mz_zip_writer_init_file(mz, sZipFilePath.toUtf8().data(), 0);

    QDirIterator it(sSrcPath, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString sFullPath = it.next();

        if (it.fileInfo().isDir())
        {
            continue;
        }

        QString sRelativePath = sFullPath;
        sRelativePath.replace(sSrcPath, "");

        ok = mz_zip_writer_add_file(mz,
                                    sRelativePath.toUtf8().data(),
                                    sFullPath.toUtf8().data(),
                                    "", 0, MZ_DEFAULT_COMPRESSION);
        if (!ok)
            break;
    }
    ok &= mz_zip_writer_finalize_archive(mz);
    mz_zip_writer_end(mz);

    return ok;
}

bool MiniZ::uncompressFolder(QString sZipFilePath, QString sDestPath)
{
    if (!QFile::exists(sZipFilePath))
    {
        return false;
    }

    QString sBaseDir = QFileInfo(sDestPath).absolutePath();
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

    mz_bool ok = mz_zip_reader_init_file(mz, sZipFilePath.toUtf8().data(), 0);
    if (!ok) return false;

    int num = mz_zip_reader_get_num_files(mz);

    mz_zip_archive_file_stat* stat = new mz_zip_archive_file_stat;
    OnScopeExit(delete stat);

    for (int i = 0; i < num; ++i)
    {
        ok &= mz_zip_reader_file_stat(mz, i, stat);

        if (stat->m_is_directory)
        {
            QString sFolderPath = QString::fromUtf8(stat->m_filename);
            bool b = baseDir.mkpath(sFolderPath);
            Q_ASSERT(b);
        }
    }

    for (int i = 0; i < num; ++i)
    {
        ok &= mz_zip_reader_file_stat(mz, i, stat);

        if (!stat->m_is_directory)
        {
            QString sFullPath = baseDir.filePath(QString::fromUtf8(stat->m_filename));
            bool b = QFileInfo(sFullPath).absoluteDir().mkpath(".");
            Q_ASSERT(b);

            ok &= mz_zip_reader_extract_to_file(mz, i, sFullPath.toUtf8(), 0);
        }
    }

    mz_zip_reader_end(mz);

    if (!ok)
    {
        qDebug() << "Unzip error!";
    }
    return ok;
}
