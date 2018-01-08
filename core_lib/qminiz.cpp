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

bool MiniZ::compressFolder(const QString& sZipFilePath, const QString& sSrcPath)
{
    mz_zip_archive* mz = new mz_zip_archive;
    OnScopeExit(delete mz);
    mz_zip_zero_struct(mz);

    bool b = mz_zip_writer_init_file(mz, sZipFilePath.toUtf8().data(), 0);
    //qDebug("Writer init ok = %d", b);

    QDirIterator it(sSrcPath, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString sFullPath = it.next();

        if (it.fileInfo().isDir())
        {
            //qDebug() << "skip " << it.fileName();
            continue;
        }

        QString sRelativePath = sFullPath;
        sRelativePath.replace(sSrcPath, "");
        //qDebug() << "  Add: " << it.fileName();

        b = mz_zip_writer_add_file(mz,
                                   sRelativePath.toUtf8().data(),
                                   sFullPath.toUtf8().data(),
                                   "", 0, MZ_DEFAULT_COMPRESSION);

        //qDebug("Writer add file ok = %d", b);
    }
    b = mz_zip_writer_finalize_archive(mz);
    qDebug("Writer finalize = %d", b);
    mz_zip_writer_end(mz);

    return true;
}

bool MiniZ::uncompressFolder(const QString& sZipFilePath, const QString& sDestPath)
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
    qDebug("Open ok=%d\n", ok);
    if (!ok) return false;

    int num = mz_zip_reader_get_num_files(mz);
    qDebug("num=%d\n", num);

    mz_zip_archive_file_stat* stat = new mz_zip_archive_file_stat;
    OnScopeExit(delete stat);

    for (int i = 0; i < num; ++i)
    {
        ok = mz_zip_reader_file_stat(mz, i, stat);
        //qDebug(" item=%s\n", stat->m_filename);
        if (!ok) break;

        if (stat->m_is_directory)
        {
            QString sFolderPath = QString::fromUtf8(stat->m_filename);

            ok = baseDir.mkpath(sFolderPath);
            //qDebug() << "mkdirOK=" << ok;
            if (!ok) break;
        }
        else
        {
            QString sFullPath = baseDir.filePath(QString::fromUtf8(stat->m_filename));
            ok = mz_zip_reader_extract_to_file(mz, i, sFullPath.toUtf8(), 0);
            //qDebug("extract ok=%d\n", ok);
            if (!ok) break;
        }
    }

    mz_zip_reader_end(mz);

    if (!ok)
    {
        qDebug() << "Unzip error!";
    }
    return ok;
}
