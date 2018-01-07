#include "qminiz.h"

#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "miniz.h"
#include "util.h"


bool MiniZ::compressFolder(const QString& sZipFilePath, const QString& sSrcPath)
{
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
        qDebug(" item=%s\n", stat->m_filename);
        if (!ok) return false;

        if (stat->m_is_directory)
        {
            QString sFolderPath = QString::fromUtf8(stat->m_filename);

            ok = baseDir.mkpath(sFolderPath);
            qDebug() << "mkdirOK=" << ok;
            if (!ok) return false;
        }
        else
        {
            QString sFullPath = baseDir.filePath(QString::fromUtf8(stat->m_filename));
            ok = mz_zip_reader_extract_to_file(mz, i, sFullPath.toUtf8(), 0);
            qDebug("extract ok=%d\n", ok);
            if (!ok) return false;
        }
    }
    return true;
}
