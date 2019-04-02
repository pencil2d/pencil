#include "asciiimage.h"

#include <QImage>
#include <QColor>
#include <QtMath>
#include <QtDebug>

const QList<QChar> AsciiImage::ASCII_LIST = {'#','#','X','X','x','x','x','+','+','+','=','=','=','-','-','-',';',';',',',',','.','.','.'};

AsciiImage::AsciiImage()
{

}

QString AsciiImage::convert(QImage img)
{
    QString output = "";

    for(int y = 0; y < img.height(); y++)
    {
        QRgb *rgbLine = (QRgb *) img.scanLine(y);
        for(int x = 0; x < img.width(); x++)
        {
            output.append(getPixel(rgbLine[x], qAlpha(rgbLine[x])));
        }
        output.append('\n');
    }
    return output;
}

QChar AsciiImage::getPixel(QColor c, ushort alpha)
{
    if(!c.isValid() || alpha == 0)
    {
        return QChar::Nbsp;
    }

    // Algorithm and mappings from https://github.com/zachwill/asciifi/blob/master/static/js/application.js
    return ASCII_LIST[qMin(qFloor(c.valueF() * ASCII_LIST.size()), ASCII_LIST.size()-1)];
}

