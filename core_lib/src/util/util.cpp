/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "util.h"
#include <QAbstractSpinBox>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

static inline bool clipLineToEdge(qreal& t0, qreal& t1, qreal p, qreal q)
{
    if (p < 0) { // Line entering the clipping window
        t0 = qMax(t0, q / p);
        return t0 < t1;
    }
    if (p > 0) { // Line leaving the clipping window
        t1 = qMin(t1, q / p);
        return t0 < t1;
    }
    return q >= 0;
}

QLineF clipLine(const QLineF& line, const QRect& clip, qreal t0, qreal t1)
{
    int left = clip.left(), right = left + clip.width(), top = clip.top(), bottom = top + clip.height();
    qreal x1 = line.x1(), x2 = line.x2(), dx = line.dx(), y1 = line.y1(), y2 = line.y2(), dy = line.dy();

    if ((t0 == 0 && t1 == 1 && ((x1 < left && x2 < left) ||
                                (x1 > right && x2 > right) ||
                                (y1 < top && y2 < top) ||
                                (y1 > bottom && y2 > bottom))) ||
        !clipLineToEdge(t0, t1, -dx, x1 - left) ||
        !clipLineToEdge(t0, t1,  dx, right - x1) ||
        !clipLineToEdge(t0, t1, -dy, y1 - top) ||
        !clipLineToEdge(t0, t1,  dy, bottom - y1)) {
        return {};
    }

    Q_ASSERT(t0 < t1);
    return {line.x1() + line.dx() * t0,
            line.y1() + line.dy() * t0,
            line.x1() + line.dx() * t1,
            line.y1() + line.dy() * t1};
}

void clearFocusOnFinished(QAbstractSpinBox *spinBox)
{
    QObject::connect(spinBox, &QAbstractSpinBox::editingFinished, spinBox, &QAbstractSpinBox::clearFocus);
}

QString ffprobeLocation()
{
#ifdef _WIN32
    return QApplication::applicationDirPath() + "/plugins/ffprobe.exe";
#elif __APPLE__
    return QApplication::applicationDirPath() + "/plugins/ffprobe";
#else
    QString ffprobePath = QStandardPaths::findExecutable(
        "ffprobe",
        QStringList()
        << QApplication::applicationDirPath() + "/plugins"
        << QApplication::applicationDirPath() + "/../plugins" // linuxdeployqt in FHS-like mode
    );
    if (!ffprobePath.isEmpty())
    {
        return ffprobePath;
    }
    return QStandardPaths::findExecutable("ffprobe"); // ffprobe is a standalone project.
#endif
}

QString ffmpegLocation()
{
#ifdef _WIN32
    return QApplication::applicationDirPath() + "/plugins/ffmpeg.exe";
#elif __APPLE__
    return QApplication::applicationDirPath() + "/plugins/ffmpeg";
#else
    QString ffmpegPath = QStandardPaths::findExecutable(
        "ffmpeg",
        QStringList()
        << QApplication::applicationDirPath() + "/plugins"
        << QApplication::applicationDirPath() + "/../plugins" // linuxdeployqt in FHS-like mode
    );
    if (!ffmpegPath.isEmpty())
    {
        return ffmpegPath;
    }
    return QStandardPaths::findExecutable("ffmpeg"); // ffmpeg is a standalone project.
#endif
}

quint64 imageSize(const QImage& img)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return img.sizeInBytes();
#else
    return img.byteCount();
#endif
}

QString uniqueString(int len)
{
    static const char alphanum[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    const int alphanumLen = sizeof(alphanum);

    if (len > 128) len = 128;

    char s[128 + 1];
    for (int i = 0; i < len; ++i)
    {
        s[i] = alphanum[rand() % (alphanumLen - 1)];
    }
    s[len] = 0;
    return QString::fromUtf8(s);
}

QString validateDataPath(QString filePath, QString dataDirPath)
{
    // Make sure src path is relative
    if (!QFileInfo(filePath).isRelative()) return QString();

    QFileInfo fi(dataDirPath, filePath);
    // Recursively resolve symlinks
    QString canonicalPath = fi.canonicalFilePath();

    QDir dataDir(dataDirPath);
    // Resolve symlinks in data dir path so it can be compared against file paths with resolved symlinks
    if (dataDir.exists())
    {
        dataDir.setPath(dataDir.canonicalPath());
    }
    // Iterate over parent directories of the file path to see if one of them equals the data directory
    if (canonicalPath.isEmpty())
    {
        // File does not exist, use absolute path and attempt to resolve symlinks again for each parent directory
        fi.setFile(fi.absoluteFilePath());
        QDir ancestor(fi.absoluteFilePath());
        while (ancestor != dataDir) {
            if (ancestor.isRoot())
            {
                // Reached root directory without finding data dir
                return QString();
            }
            QDir newAncestor = QFileInfo(ancestor.absolutePath()).dir();
            if (newAncestor.exists())
            {
                // Resolve directory symlinks
                newAncestor.setPath(newAncestor.canonicalPath());
            }
            ancestor = newAncestor;
        }
        // One of the parent directories of filePath matches dataDir
        return fi.absoluteFilePath();
    }
    else
    {
        // File exists and all symlinks have been resolved in canonicalPath so no further attempts to resolve symlinks are necessary
        fi.setFile(canonicalPath);
        QDir ancestor = fi.dir();
        while (ancestor != dataDir)
        {
            if (ancestor.isRoot()) {
                // Data dir was not found in ancestors of the src path
                return QString();
            }
            ancestor = QFileInfo(ancestor.absolutePath()).dir();
        }
        // One of the parent directories of filePath matches dataDir
        return fi.absoluteFilePath();
    }
}
