#include "colorwheel.h"
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QtCore/qmath.h>
#include <QDebug>

ColorWheel::ColorWheel(QWidget *parent) : QWidget(parent),
    initSize(200, 200),
    mouseDown(false),
    margin(0),
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


QColor ColorWheel::posColor(const QPoint &point)
{
    if ( ! wheel.rect().contains(point) ) 
    {
        return QColor();
    }
    if (inWheel)
    {
        qreal hue = 0;
        int r = qMin(width(), height()) / 2;
        if ( point.x() > r )
        {
            if(point.y() < r )
            {
                //1
                hue = 90 - (qAtan2( (point.x() - r) , (r - point.y()) )  / 3.14 / 2 * 360);
            }
            else
            {
                //4
                hue = 270 + (qAtan2( (point.x() - r) , (point.y() - r ) )  / 3.14 / 2 * 360);
            }
        }
        else
        {
            if(point.y() < r )
            {
                //2
                hue =  90 + (qAtan2( (r - point.x()) , (r - point.y()) )  / 3.14 / 2 * 360);
            }
            else
            {
                //3
                hue =  270 - (qAtan2( (r - point.x()) , (point.y() - r ))  / 3.14 / 2 * 360);
            }
        }
        
        hue = (hue > 359) ?359 : hue;
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
        qreal r = w/2-margin;
        // radius of inner circle
        qreal ir = r - wheelWidth;
        // left corner of square
        qreal m = (w / 2.0) - (ir / qSqrt(2));
        QPoint p = point - QPoint(m, m);
        qreal SquareWidth = 2 * ir / qSqrt(2);
        return QColor::fromHsvF( currentColor.hueF(),
                                 p.x() / SquareWidth,
                                 p.y() / SquareWidth);
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
    lastPos = event->pos();
    if (wheelRegion.contains(lastPos))
    {
        inWheel = true;
        inSquare = false;
        QColor color = posColor(lastPos);
        hueChanged(color.hue());
    }
    else if (squareRegion.contains(lastPos))
    {
        inWheel = false;
        inSquare = true;
        QColor color = posColor(lastPos);
        svChanged(color);
    }
    mouseDown = true;
}

void ColorWheel::mouseMoveEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    if ( !mouseDown )
    {
        return;
    }
    if (wheelRegion.contains(lastPos) && inWheel)
    {
        QColor color = posColor(lastPos);
        hueChanged(color.hue());
    } 
    else if(squareRegion.contains(lastPos) && inSquare)
    {
        QColor color = posColor(lastPos);
        svChanged(color);
    } 
    else 
    {
        // TODO: due with cursor out of region after press
        //        int length = qMin(width(), height());
        //        QPoint center(length/2, length/2);
        //        int R = qSqrt(qPow(qAbs(lastPos.x()), 2)
        //                      + qPow(qAbs(lastPos.y()), 2));
        //        if(inWheel){
        //            int r =  length / 2;
        //            r += qSqrt(qPow(center.x(), 2) + qPow(center.y(), 2));
        //            int x0 = r/R * qAbs(lastPos.x());
        //            int y0 = r/R * qAbs(lastPos.y());
        //            QColor color = posColor(QPoint(x0, y0));
        //            hueChanged(color.hue());
        //        }else if(inSquare){
        //            //
        //        }
    }
}

void ColorWheel::mouseReleaseEvent(QMouseEvent *)
{
    mouseDown = false;
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
    composeWheel();
    painter.drawPixmap(0,0,wheel);
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
    conicalGradient.setColorAt(0.0, Qt::red);
    conicalGradient.setColorAt(60.0 / 360.0, Qt::yellow);
    conicalGradient.setColorAt(120.0 / 360.0, Qt::green);
    conicalGradient.setColorAt(180.0 / 360.0, Qt::cyan);
    conicalGradient.setColorAt(240.0 / 360.0, Qt::blue);
    conicalGradient.setColorAt(300.0 / 360.0, Qt::magenta);
    conicalGradient.setColorAt(1.0, Qt::red);

    /* outer circle */
    painter.translate(r / 2, r / 2);

    QBrush brush(conicalGradient);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(0, 0), r/2 - margin, r/2 - margin);
    /* inner circle */
    painter.setBrush(background);
    painter.drawEllipse(QPoint(0, 0), r/2 - margin - wheelWidth, r/2-margin-wheelWidth);

    //caculate wheel region
    wheelRegion = QRegion(r/2, r/2, r-2*margin, r-2*margin, QRegion::Ellipse);
    wheelRegion.translate(-(r-2*margin)/2, -(r-2*margin)/2);

    int tmp = 2*(margin+wheelWidth);
    QRegion subRe( r/2, r/2, r-tmp, r-tmp, QRegion::Ellipse );
    subRe.translate(-(r-tmp)/2, -(r-tmp)/2);
    wheelRegion -= subRe;
}

void ColorWheel::drawSquareImage(const int &hue)
{
    // region of the widget
    int w = qMin(width(), height());
    // radius of outer circle
    qreal r = w / 2-margin;
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
    painter.rotate( -hue );
    r = qMin(height(), width()) / 2.0 - margin - wheelWidth/2;
    painter.drawEllipse(QPointF(r,0.0),5,5);
}

void ColorWheel::drawPicker(const QColor &color)
{
    QPainter painter(&wheel);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;

    // region of the widget
    int w = qMin(width(), height());
    // radius of outer circle
    qreal r = w / 2 - margin;
    // radius of inner circle
    qreal ir = r - wheelWidth;
    // left corner of square
    qreal m = (w / 2.0) - (ir/qSqrt(2));
    painter.translate(m - 5, m - 5);
    qreal SquareWidth = 2 * ir / qSqrt(2);
    qreal S = color.saturationF()*SquareWidth;
    qreal V = color.valueF()*SquareWidth;

    if(color.saturation() > 30 ||color.value() < 50){
        pen.setColor(Qt::white);
    }
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawEllipse(S,V,10,10);
}

void ColorWheel::composeWheel()
{
    QPainter composePainter(&wheel);
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
    if(!isVisible())
    {
        return;
    }

    repaint();
    emit colorChanged(currentColor);
}
