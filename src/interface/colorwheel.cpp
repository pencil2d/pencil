
#include <cmath>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QtCore/qmath.h>
#include <QRect>
#include <QDebug>

#include "colorwheel.h"

ColorWheel::ColorWheel(QWidget *parent) : QWidget(parent),
    m_initSize(200, 200),
    m_wheelWidth(25),
    m_currentColor(Qt::red),
    m_isInWheel(false),
    m_isInSquare(false)
{
    m_currentColor = m_currentColor.toHsv();
    setCursor(Qt::CrossCursor);
}

QColor ColorWheel::color()
{
    return m_currentColor;
}

void ColorWheel::setColor(const QColor &color)
{
    if (color == m_currentColor)
    {
        return;
    }
    if (color.hue() != m_currentColor.hue()) 
    {
        hueChanged(color.hue());
    }

    if (color.saturation() != m_currentColor.saturation() ||
        color.value() != m_currentColor.value() )
    {
        svChanged(color);
    }

    update();
    emit colorChanged(color);
}


QColor ColorWheel::pickColor(const QPoint& point)
{
    if ( ! m_wheelPixmap.rect().contains(point) ) 
    {
        return QColor();
    }
    if (m_isInWheel)
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
                               m_currentColor.saturation(),
                               m_currentColor.value());
    }
    else if (m_isInSquare) 
    {
        QRect rect = m_squareRegion.boundingRect();
        QPoint p = point - rect.topLeft();
        //qDebug("TopRight(%d, %d) Point(%d, %d)", rect.topRight().x(), rect.topRight().y(), point.x(), point.y());
        QSizeF regionSize = rect.size() - QSizeF(1, 1);

        //qDebug("p(%d, %d), Region(%.1f, %.1f)", p.x(), p.y(), regionSize.width(), regionSize.height());
        return QColor::fromHsvF( m_currentColor.hueF(),
                                 p.x() / regionSize.width(),
                                 1.0 - (p.y() / regionSize.height()));
    }
    return QColor();
}

QSize ColorWheel::sizeHint () const
{
    return QSize(height(),height());
}

QSize ColorWheel::minimumSizeHint () const
{
    return m_initSize;
}

void ColorWheel::mousePressEvent(QMouseEvent *event)
{
    QPoint lastPos = event->pos();
    if (m_squareRegion.contains(lastPos))
    {
        m_isInWheel = false;
        m_isInSquare = true;
        QColor color = pickColor(lastPos);
        svChanged(color);
    }
    else if (m_wheelRegion.contains(lastPos))
    {
        m_isInWheel = true;
        m_isInSquare = false;
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
    if (m_isInSquare)
    {
        QRect rect = m_squareRegion.boundingRect();

        if ( lastPos.x() < rect.topLeft().x() )
        {
            lastPos.setX( rect.topLeft().x() );
        } 
        else if ( lastPos.x() > rect.bottomRight().x() )
        {
            lastPos.setX( rect.bottomRight().x() );
        }

        if ( lastPos.y() < rect.topLeft().y() )
        {
            lastPos.setY( rect.topLeft().y() );
        }
        else if ( lastPos.y() > rect.bottomRight().y() )
        {
            lastPos.setY( rect.bottomRight().y() );
        }

        QColor color = pickColor(lastPos);
        svChanged(color);
    }
    else if (m_wheelRegion.contains(lastPos) && m_isInWheel)
    {
        QColor color = pickColor(lastPos);
        hueChanged(color.hue());
    }          
}

void ColorWheel::mouseReleaseEvent(QMouseEvent *)
{    
    m_isInWheel = false;
    m_isInSquare = false;
}

void ColorWheel::resizeEvent(QResizeEvent *event)
{
    m_wheelPixmap = QPixmap(event->size());
    m_wheelPixmap.fill(palette().background().color());
    drawWheelImage(event->size());
    drawSquareImage(m_currentColor.hue());
    update();
}

void ColorWheel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOption opt;
    opt.initFrom(this);
    composeWheel(m_wheelPixmap);
    painter.drawPixmap(0, 0, m_wheelPixmap);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ColorWheel::drawWheelImage(const QSize &newSize)
{
    int r = qMin(newSize.width(), newSize.height());

    QStyleOption option;
    option.initFrom(this);

    QBrush backgroundBrush = option.palette.window();

    m_wheelImage = QImage(newSize, QImage::Format_ARGB32_Premultiplied);

    //m_wheelImage.fill(background.color());  // Only in 4.8

    QPainter painter(&m_wheelImage);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(m_wheelImage.rect(), backgroundBrush);

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

    painter.setBrush(backgroundBrush);
    painter.drawEllipse(QPoint(0, 0), r/2 - m_wheelWidth, r/2 - m_wheelWidth);

    //caculate wheel region
    m_wheelRegion = QRegion(0, 0, r, r, QRegion::Ellipse);
}

void ColorWheel::drawSquareImage(const int &hue)
{
    // region of the widget
    int w = qMin(width(), height());
    // radius of outer circle
    qreal r = w / 2;
    // radius of inner circle
    qreal ir = r - m_wheelWidth;
    // left corner of square
    qreal m = (w /2.0) - (ir / qSqrt(2));

    QImage square(255, 255, QImage::Format_ARGB32_Premultiplied);
    QColor color;
    
    for (int i = 0; i < 255; ++i)
    {
        for (int j = 0; j < 255; ++j)
        {
            color = QColor::fromHsv(hue, i, 255 - j);
            QRgb rgb = qRgb(color.red(), color.green(), color.blue());
            square.setPixel(i, j, rgb);
        }
    }
    qreal SquareWidth = 2 * ir / qSqrt(2);
    m_squareImage = square.scaled(SquareWidth, SquareWidth);
    m_squareRegion = QRegion(m, m, SquareWidth, SquareWidth);
}

void ColorWheel::drawHueIndicator(const int &hue)
{
    QPainter painter(&m_wheelPixmap);
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

    r = qMin(height(), width()) / 2.0 - m_wheelWidth/2;
    painter.drawEllipse(QPointF(r, 0.0), 7, 7);
}

void ColorWheel::drawPicker(const QColor &color)
{
    QPainter painter(&m_wheelPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPoint squareTopLeft = m_squareRegion.boundingRect().topLeft();

    painter.translate(squareTopLeft.x(), squareTopLeft.y());
    
    QSize squareSize = m_squareRegion.boundingRect().size();

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
    composePainter.drawImage(0, 0, m_wheelImage);
    composePainter.drawImage(m_squareRegion.boundingRect().topLeft(), m_squareImage);
    composePainter.end();
    drawHueIndicator(m_currentColor.hue());
    drawPicker(m_currentColor);
}

void ColorWheel::hueChanged(const int &hue)
{
    if ( hue < 0 || hue > 359)
    {
        return;
    }
    int s = m_currentColor.saturation();
    int v = m_currentColor.value();

    m_currentColor.setHsv(hue, s, v);

    if(!isVisible())
    {
        return;
    }

    drawSquareImage(hue);

    repaint();
    emit colorChanged(m_currentColor);
}

void ColorWheel::svChanged(const QColor &newcolor)
{
    int hue = m_currentColor.hue();
    m_currentColor.setHsv(hue, 
                        newcolor.saturation(), 
                        newcolor.value());
    if( !isVisible() )
    {
        return;
    }

    repaint();
    emit colorChanged(m_currentColor);
}
