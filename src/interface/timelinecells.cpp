#include "timelinecells.h"

#include <QSettings>
#include "editor.h"
#include "timeline.h"

TimeLineCells::TimeLineCells(TimeLine* parent, Editor* editor, QString type)
    : QWidget(parent)
{
    this->timeLine = parent;
    this->editor = editor;
    this->type = type;

    cache = NULL;
    QSettings settings("Pencil","Pencil");

    frameLength = settings.value("length").toInt();
    if (frameLength==0) { frameLength=240; settings.setValue("length", frameLength); }

    shortScrub = settings.value("shortScrub").toBool();

    startY = 0;
    endY = 0;
    mouseMoveY = 0;
    startLayerNumber = -1;
    offsetX = 0;
    offsetY = 20;
    frameOffset = 0;
    layerOffset = 0;

    frameSize = (settings.value("frameSize").toInt());
    if (frameSize==0) { frameSize=12; settings.setValue("frameSize", frameSize); }

    fontSize = (settings.value("labelFontSize").toInt());
    if (fontSize==0) { fontSize=12; settings.setValue("labelFontSize", fontSize); }

    layerHeight = (settings.value("layerHeight").toInt());
    if(layerHeight==0) { layerHeight=20; settings.setValue("layerHeight", layerHeight); }

    setMinimumSize(500, 4*layerHeight);
    setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding) );
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

int TimeLineCells::getFrameNumber(int x)
{
    int frameNumber = frameOffset+1+(x-offsetX)/frameSize;
//	qDebug()<< frameNumber;
    return frameNumber;
}

int TimeLineCells::getFrameX(int frameNumber)
{
    int x = offsetX + (frameNumber-frameOffset)*frameSize;
    return x;
}

int TimeLineCells::getLayerNumber(int y)
{
    int layerNumber = layerOffset + (y-offsetY)/layerHeight;
    if (y < offsetY)
    {
        layerNumber = -1;
    }

    if (layerNumber >= editor->object->getLayerCount())
    {
        layerNumber = editor->object->getLayerCount();
    }
    return layerNumber;
}

int TimeLineCells::getLayerY(int layerNumber)
{
    return offsetY + (layerNumber-layerOffset)*layerHeight;
}

void TimeLineCells::updateFrame(int frameNumber)
{
    int x = getFrameX(frameNumber);
    update(x-frameSize,0,frameSize+1,height());
}

void TimeLineCells::updateContent()
{
    drawContent();
    update();
}

void TimeLineCells::drawContent()
{
    if (cache == NULL) { cache = new QPixmap(size()); }
    if (cache->isNull()) return;

    QPainter painter( cache );

    Object* object = editor->object;
    if (object == NULL) return;
    Layer* layer = object->getLayer(editor->m_nCurrentLayerIndex);
    if (layer == NULL) return;

    // grey background of the view
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::lightGray);
    painter.drawRect(QRect(0,0, width(), height()));

    // --- draw layers of the current object
    for(int i=0; i< object->getLayerCount(); i++)
    {
        if(i != editor->m_nCurrentLayerIndex)
        {
            Layer* layeri = object->getLayer(i);
            if(layeri != NULL)
            {
                if(type == "tracks") layeri->paintTrack(painter, this, offsetX, getLayerY(i), width()-offsetX, getLayerHeight(), false, frameSize);
                if(type == "layers") layeri->paintLabel(painter, this, 0, getLayerY(i), width()-1, getLayerHeight(), false, editor->allLayers());
            }
        }
    }
    if( abs(getMouseMoveY()) > 5 )
    {
        if(type == "tracks") layer->paintTrack(painter, this, offsetX, getLayerY(editor->m_nCurrentLayerIndex)+getMouseMoveY(), width()-offsetX, getLayerHeight(), true, frameSize);
        if(type == "layers") layer->paintLabel(painter, this, 0, getLayerY(editor->m_nCurrentLayerIndex)+getMouseMoveY(), width()-1, getLayerHeight(), true, editor->allLayers());
        painter.setPen( Qt::black );
        painter.drawRect(0, getLayerY( getLayerNumber(endY) ) -1, width(), 2);
    }
    else
    {
        if(type == "tracks") layer->paintTrack(painter, this, offsetX, getLayerY(editor->m_nCurrentLayerIndex), width()-offsetX, getLayerHeight(), true, frameSize);
        if(type == "layers") layer->paintLabel(painter, this, 0, getLayerY(editor->m_nCurrentLayerIndex), width()-1, getLayerHeight(), true, editor->allLayers());
    }

    // --- draw top
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(220,220,220));
    painter.drawRect(QRect(0,0, width()-1, offsetY-1));
    painter.setPen( Qt::gray );
    painter.drawLine(0,0, width()-1, 0);
    painter.drawLine(0,offsetY-2, width()-1, offsetY-2);
    painter.setPen( Qt::lightGray );
    painter.drawLine(0,offsetY-3, width()-1, offsetY-3);
    painter.drawLine(0,0, 0, offsetY-3);

    if(type == "layers")
    {
        // --- draw circle
        painter.setPen(Qt::black);
        if(editor->allLayers() == 0) { painter.setBrush(Qt::NoBrush); }
        if(editor->allLayers() == 1) { painter.setBrush(Qt::darkGray); }
        if(editor->allLayers() == 2) { painter.setBrush(Qt::black); }
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawEllipse(6, 4, 9, 9);
        painter.setRenderHint(QPainter::Antialiasing, false);
    }

    if(type == "tracks")
    {
        // --- draw ticks
        painter.setPen( QColor(70,70,70,255) );
        painter.setBrush(Qt::darkGray);
        painter.setFont(QFont("helvetica", 10));
        int incr = 0;
        int fps = editor->fps;
        for(int i=frameOffset; i<frameOffset+(width()-offsetX)/frameSize; i++)
        {
            if(i < 9) { incr = 4; }
            else { incr = 0; }
            if (i%fps==0) painter.drawLine( getFrameX(i), 1, getFrameX(i), 5 );
            else if (i%fps==fps/2) painter.drawLine( getFrameX(i), 1, getFrameX(i), 5);
            else painter.drawLine( getFrameX(i), 1, getFrameX(i), 3);
            if(i==0 || i%fps==fps-1) painter.drawText(QPoint(getFrameX(i)+incr, 15), QString::number(i+1));
        }

        // --- draw left border line
        painter.setPen( Qt::darkGray );
        painter.drawLine(0,0, 0, height());
    }
}

void TimeLineCells::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    Object* object = editor->object;
    if(object == NULL) return;
    Layer* layer = object->getLayer(editor->m_nCurrentLayerIndex);
    if(layer == NULL) return;

    QPainter painter( this );
    if( (!editor->playing && !timeLine->scrubbing) || cache == NULL) drawContent();
    if(cache) painter.drawPixmap(QPoint(0,0), *cache);

    if(type == "tracks")
    {
        // --- draw the position of the current frame
        if(editor->m_nCurrentFrameIndex > frameOffset)
        {
            painter.setBrush(QColor(255,0,0,128));
            painter.setPen(Qt::NoPen);
            painter.setFont(QFont("helvetica", 10));
            //painter.setCompositionMode(QPainter::CompositionMode_Source); // this causes the message: QPainter::setCompositionMode: PorterDuff modes not supported on device
            QRect scrubRect;
            scrubRect.setTopLeft(QPoint( getFrameX(editor->m_nCurrentFrameIndex-1), 0));
            scrubRect.setBottomRight(QPoint( getFrameX(editor->m_nCurrentFrameIndex), height()));
            if(shortScrub) scrubRect.setBottomRight(QPoint( getFrameX(editor->m_nCurrentFrameIndex), 19));
            painter.drawRect(scrubRect);
            painter.setPen( QColor(70,70,70,255) );
            int incr = 0;
            if(editor->m_nCurrentFrameIndex < 10) { incr = 4; }
            else { incr = 0; }
            painter.drawText(QPoint(getFrameX(editor->m_nCurrentFrameIndex-1)+incr, 15), QString::number(editor->m_nCurrentFrameIndex));
        }
    }
}

void TimeLineCells::resizeEvent(QResizeEvent* event)
{
    if(cache) delete cache;
    cache = new QPixmap(size());
    updateContent();
    event->accept();
}

void TimeLineCells::mousePressEvent(QMouseEvent* event)
{
    int frameNumber = getFrameNumber(event->pos().x());
    int layerNumber = getLayerNumber(event->pos().y());
    startY = event->pos().y();
    startLayerNumber = layerNumber;
    endY = event->pos().y();

    if(type == "layers")
    {
        if(layerNumber != -1 && layerNumber < editor->object->getLayerCount() )
        {
            if(event->pos().x() < 15)
            {
                editor->switchVisibilityOfLayer(layerNumber);
            }
            else
            {
                editor->setCurrentLayer(layerNumber);
                update();
            }
        }
        if(layerNumber == -1)
        {
            if(event->pos().x() < 15)
            {
                editor->toggleShowAllLayers();
            }
        }
    }

    if(type == "tracks")
    {
        if(frameNumber == editor->m_nCurrentFrameIndex && (!shortScrub || (shortScrub && startY < 20)) )
        {
            timeLine->scrubbing = true;
        }
        else
        {
            if( (layerNumber != -1) && layerNumber < editor->object->getLayerCount())
            {
                editor->object->getLayer(layerNumber)->mousePress(event, frameNumber);
                //if(event->pos().x() > 15) editor->setCurrentLayer(layerNumber);
                editor->setCurrentLayer(layerNumber);
                update();
            }
            else
            {
                if(frameNumber > 0)
                {
                    editor->scrubTo(frameNumber);
                    timeLine->scrubbing = true;
                }
            }
        }
    }
}

void TimeLineCells::mouseMoveEvent(QMouseEvent* event)
{
    if(type == "layers")
    {
        endY = event->pos().y();
        emit mouseMovedY(endY-startY);
    }
    int frameNumber = getFrameNumber(event->pos().x());
    int layerNumber = getLayerNumber(event->pos().y());
    if ( type == "tracks")
    {
        if (timeLine->scrubbing)
        {
            editor->scrubTo(frameNumber);
        }
        else
        {
            if(layerNumber != -1 && layerNumber < editor->object->getLayerCount())
            {
                editor->object->getLayer(layerNumber)->mouseMove(event, frameNumber);
            }
        }
    }
    timeLine->update();
}

void TimeLineCells::mouseReleaseEvent(QMouseEvent* event)
{
    endY = startY;
    emit mouseMovedY(0);
    timeLine->scrubbing = false;
    int frameNumber = getFrameNumber(event->pos().x());
    if(frameNumber < 1) frameNumber = -1;
    int layerNumber = getLayerNumber(event->pos().y());
    if(type == "tracks" && layerNumber != -1 && layerNumber < editor->object->getLayerCount() )
    {
        editor->object->getLayer(layerNumber)->mouseRelease(event, frameNumber);
    }
    if(type == "layers" && layerNumber != startLayerNumber && startLayerNumber != -1 && layerNumber != -1)
    {
        editor->moveLayer(startLayerNumber, layerNumber);
    }
    update();
}

void TimeLineCells::mouseDoubleClickEvent(QMouseEvent* event)
{
    int frameNumber = getFrameNumber(event->pos().x());
    int layerNumber = getLayerNumber(event->pos().y());

    // -- short scrub --
    if(event->pos().y() < 20)
    {
        if(shortScrub) scrubChange(0);
        else scrubChange(1);
    }

    // -- layer --
    Layer* layer = editor->object->getLayer( layerNumber );
    //if(layerNumber != -1 && layerNumber < editor->object->getLayerCount() ) {
    if(layer)
    {
        if(type == "tracks" && (layerNumber != -1) && (frameNumber > 0) && layerNumber < editor->object->getLayerCount())
        {
            editor->object->getLayer(layerNumber)->mouseDoubleClick(event, frameNumber);
        }
        if(type == "layers")
        {
            layer->editProperties();
            update();
        }
    }
}

void TimeLineCells::fontSizeChange(int x)
{
    fontSize=x;
    QSettings settings("Pencil","Pencil");
    settings.setValue("labelFontSize", x);
    updateContent();
}

void TimeLineCells::frameSizeChange(int x)
{
    frameSize = x;
    QSettings settings("Pencil","Pencil");
    settings.setValue("frameSize", x);
    updateContent();
}

void TimeLineCells::scrubChange(int x)
{
    QSettings settings("Pencil","Pencil");
    if (x==0) { shortScrub=false; settings.setValue("shortScrub","false"); }
    else { shortScrub=true; settings.setValue("shortScrub","true"); }
    update();
}

void TimeLineCells::labelChange(int x)
{
    QSettings settings("Pencil","Pencil");
    if (x==0) { drawFrameNumber=false; settings.setValue("drawLabel","false"); }
    else { drawFrameNumber=true; settings.setValue("drawLabel","true"); }
    updateContent();
}

void TimeLineCells::lengthChange(QString x)
{
    bool ok;
    int dec = x.toInt(&ok, 10);
    frameLength=dec;
    timeLine->updateLength(frameLength);
    updateContent();
    QSettings settings("Pencil","Pencil");
    settings.setValue("length", dec);
}

void TimeLineCells::hScrollChange(int x)
{
    frameOffset = x;
    update();
}

void TimeLineCells::vScrollChange(int x)
{
    layerOffset = x;
    update();
}

