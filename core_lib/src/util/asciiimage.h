#ifndef ASCIIIMAGE_H
#define ASCIIIMAGE_H

#include <QtGlobal>

class QImage;
class QColor;
class QChar;

class AsciiImage
{
public:
    AsciiImage();

    static QString convert(QImage img);
    static QChar getPixel(QColor c, ushort alpha);

private:
    static const QList<QChar> ASCII_LIST;
};

#endif // ASCIIIMAGE_H
