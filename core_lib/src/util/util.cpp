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
#include <QStandardPaths>

QTransform RectMapTransform( QRectF source, QRectF target )
{
    qreal x1 = source.left();
    qreal y1 = source.top();
    qreal x2 = source.right();
    qreal y2 = source.bottom();
    qreal x1P = target.left();
    qreal y1P = target.top();
    qreal x2P = target.right();
    qreal y2P = target.bottom();

    QTransform matrix;
    if ( ( x1 != x2 ) && ( y1 != y2 ) )
    {
        matrix = QTransform( ( x2P - x1P ) / ( x2 - x1 ), // scale x
                             0,
                             0,
                             ( y2P - y1P ) / ( y2 - y1 ), // scale y
                             ( x1P * x2 - x2P * x1 ) / ( x2 - x1 ),    // dx
                             ( y1P * y2 - y2P * y1 ) / ( y2 - y1 ) );  // dy
    }
    else
    {
        matrix.reset();
    }
    return matrix;
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
    const int alphanum_len = sizeof(alphanum);

    if (len > 128) len = 128;

    char s[128 + 1];
    for (int i = 0; i < len; ++i)
    {
        s[i] = alphanum[rand() % (alphanum_len - 1)];
    }
    s[len] = 0;
    return QString::fromUtf8(s);
}
