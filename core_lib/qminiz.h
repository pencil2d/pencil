#ifndef QMINIZ_H
#define QMINIZ_H

#include <QString>

namespace MiniZ
{
    bool isZip(const QString& sZipFilePath);
    bool compressFolder(QString sZipFilePath, QString sSrcPath);
    bool uncompressFolder(QString sZipFilePath, QString sDestPath);
}
#endif
