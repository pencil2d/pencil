
#include <cmath>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QtCore/qmath.h>
#include <QDebug>

#include "colorwheel.h"

ColorWheel::ColorWheel(QWidget *parent) : QWidget(parent),
    initSize(200, 200),
    wheelWidth(30),
    currentColor(Qt::red),
    inWheel(false),
    inSquare(false)
{
    currentColor = currentColor.toHsv();
    setCursor(Qt::CrossCursor);
}

QColor ColorWheel::color()
{
    return currentColor;
}

void ColorWheel::setColor(const QColor &color)
{
    if (color == currentColor)
    {
        return;
    }
    if (color.hue() != currentColor.hue()) 
    {
        hueChanged(color.hue());
    }

    if (color.saturation() != currentColor.saturation() ||
        color.value() != currentColor.value() )
    {
        svChanged(color);
    }

    update();
    emit colorChanged(color);
}


QColor ColorWheel::pickColor(const QPoint& point)
{
    if ( ! wheel.rect().contains(point) ) 
    {
        return QColor();
    }
    if (inWheel)
    {
        qreal hue = 0;
        int r = qMin(width(), height()) / 2;
        QString strDebug = "";
        strDebug += QString("Radius=%1").arg(r);

        QPoint center(width() / 2, height() / 2);

        QPoint diff = point - center;
        strDebug += QString(" Atan2=%1").arg(qAtan2(diff.x(), diff.y()));               
        
        hue = qAtan2( diff.x(), diff.y() ) / M_PI * 180;
        
        hue = hue + 180; // shift -180~180 to 0~360
        
        strDebug += QString(" Hue=%1").arg(hue);
        //qDebug() << strDebug;

        hue = (hue > 359) ? 359 : hue;
        hue = (hue < 0) ? 0 : hue;

        return QColor::fromHsv(hue,
                               currentColor.saturation(),
                               currentColor.value());
    }
    if (inSquare) 
    {
        // region of the widget
        int w = qMin(width(), height());
        // radius of outer circle
        qreal r = w / 2 ;
        // radius of inner circle
        qreal ir = r - wheelWidth;
        // left corner of square
        qreal m = (w / 2.0) - (ir / qSqrt(2));
        QPoint p = point - QPoint(m, m);
        qreal SquareWidth = 2 * ir / qSqrt(2);
        return QColor::fromHsvF( currentColor.hueF(),
                                 p.x() / SquareWidth,
                                 1.0 - (p.y() / SquareWidth));
    }
    return QColor();
}

QSize ColorWheel::sizeHint () const
{
    return QSize(height(),height());
}

QSize ColorWheel::minimumSizeHint () const
{
    return initSize;
}

void ColorWheel::mousePressEvent(QMouseEvent *event)
{
    QPoint lastPos = event->pos();
    if (squareRegion.contains(lastPos))
    {
        inWheel = false;
        inSquare = true;
        QColor color = pickColor(lastPos);
        svChanged(color);
    }
    else if (wheelRegion.contains(lastPos))
    {
        inWheel = true;
        inSquare = false;
        QColor color = pickColor(lastPos);
        hueChanged(color.hue());
    }
    
}

void ColorWheel::mouseMoveEvent(QMouseEvent* event)
{
    QPoint lastPos = event->pos();
    if ( event->buttons() == Qt::NoButton )    
    {
        return;
    }
    if(squareRegion.contains(lastPos) && inSquare)
    {
        QColor color = pickColor(lastPos);
        svChanged(color);
    }
    else if (wheelRegion.contains(lastPos) && inWheel)
    {
        QColor color = pickColor(lastPos);
        hueChanged(color.hue());
    }          
}

void ColorWheel::mouseReleaseEvent(QMouseEvent *)
{    
    inWheel = false;
    inSquare = false;
}

void ColorWheel::resizeEvent(QResizeEvent *event)
{
    wheel = QPixmap(event->size());
    wheel.fill(palette().background().color());
    drawWheelImage(event->size());
    drawSquareImage(currentColor.hue());
    update();
}

void ColorWheel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOption opt;
    opt.initFrom(this);
    composeWheel(wheel);
    painter.drawPixmap(0, 0, wheel);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ColorWheel::drawWheelImage(const QSize &newSize)
{
    int r = qMin(newSize.width(), newSize.height());

    QStyleOption option;
    option.initFrom(this);

    QBrush background = option.palette.window();

    wheelImage = QImage(newSize, QImage::Format_ARGB32_Premultiplied);
    wheelImage.fill(background.color());
    QPainter painter(&wheelImage);
    painter.setRenderHint(QPainter::Antialiasing);

    QConicalGradient conicalGradient(0, 0, 0);

    for (int hue = 0; hue < 360; hue +=1)
    {        
        conicalGradient.setColorAt( hue / 360.0,  QColor::fromHsv(hue, 255, 255));
    }

    /* outer circle */
    painter.translate(r / 2, r / 2);

    QBrush brush(conicalGradient);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.rotate( -90 );
    painter.drawEllipse(QPoint(0, 0), r/2, r/2);
    /* inner circle */
    
    painter.setBrush(background);
    painter.drawEllipse(QPoint(0, 0), r/2 - wheelWidth, r/2 - wheelWidth);

    //caculate wheel region
    wheelRegion = QRegion(0, 0, r, r, QRegion::Ellipse);        
}

void ColorWheel::drawSquareImage(const int &hue)
{
    // region of the widget
    int w = qMin(width(), height());
    // radius of outer circle
    qreal r = w / 2;
    // radius of inner circle
    qreal ir = r - wheelWidth;
    // left corner of square
    qreal m = (w /2.0) - (ir / qSqrt(2));

    QImage square(255, 255, QImage::Format_ARGB32_Premultiplied);
    QColor color;
    QRgb vv;
    for (int i = 0; i < 255; ++i)
    {
        for (int j = 0; j < 255; ++j)
        {
            color = QColor::fromHsv(hue, i, 255 - j);
            vv = qRgb(color.red(),color.green(),color.blue());
            square.setPixel(i, j, vv);
        }
    }
    qreal SquareWidth = 2 * ir / qSqrt(2);
    squareImage = square.scaled(SquareWidth, SquareWidth);
    squareRegion = QRegion(m, m, SquareWidth, SquareWidth);
}

void ColorWheel::drawIndicator(const int &hue)
{
    QPainter painter(&wheel);
    painter.setRenderHint(QPainter::Antialiasing);
    if(hue > 20 && hue < 200)
    {
        painter.setPen(Qt::black);
    }
    else
    {
        painter.setPen(Qt::white);
    }
    painter.setBrush(Qt::NoBrush);

    QPen pen = painter.pen();
    pen.setWidth(3);
    painter.setPen(pen);
    qreal r = qMin(height(), width()) / 2.0;
    painter.translate(r, r);
    painter.rotate( -hue - 90 );

    r = qMin(height(), width()) / 2.0 - wheelWidth/2;
    painter.drawEllipse(QPointF(r, 0.0), 7, 7);
}

void ColorWheel::drawPicker(const QColor &color)
{
    QPainter painter(&wheel);
    painter.setRenderHint(QPainter::Antialiasing);

    QPoint squareTopLeft = squareRegion.boundingRect().topLeft();

    painter.translate(squareTopLeft.x(), squareTopLeft.y());
    
    QSize squareSize = squareRegion.boundingRect().size();

    qreal S = color.saturationF() * squareSize.width();
    qreal V = squareSize.height() - (color.valueF() * squareSize.height());
 
    QPen pen;
    pen.setWidth(3);
    if (color.saturation() > 30 || color.value() < 50)
    {
        pen.setColor(Qt::white);
    }
    painter.setPen(pen);
    
    painter.drawEllipse(S - 5, V - 5, 10, 10);
}

void ColorWheel::composeWheel( QPixmap& pixmap )
{
    QPainter composePainter(&pixmap);
    composePainter.drawImage(0, 0, wheelImage);
    composePainter.drawImage(squareRegion.boundingRect().topLeft(), squareImage);
    composePainter.end();
    drawIndicator(currentColor.hue());
    drawPicker(currentColor);
}

void ColorWheel::hueChanged(const int &hue)
{
    if ( hue < 0 || hue > 359)
    {
        return;
    }
    int s = currentColor.saturation();
    int v = currentColor.value();

    currentColor.setHsv(hue, s, v);

    if(!isVisible())
    {
        return;
    }

    drawSquareImage(hue);

    repaint();
    emit colorChanged(currentColor);
}

void ColorWheel::svChanged(const QColor &newcolor)
{
    int hue = currentColor.hue();
    currentColor.setHsv(hue, 
                        newcolor.saturation(), 
                        newcolor.value());
    if( !isVisible() )
    {
        return;
    }

    repaint();
    emit colorChanged(currentColor);
}
