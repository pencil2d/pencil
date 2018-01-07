#ifndef QMINIZ_H
#define QMINIZ_H

#include <QString>

namespace MiniZ
{
    bool compressFolder(const QString& sZipFilePath, const QString& sSrcPath);
    bool uncompressFolder(const QString& sZipFilePath, const QString& sDestPath);
}
#endif