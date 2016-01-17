
#include "eyedroppertool.h"
#include <QPainter>
#include <QPixmap>
#include <QBitmap>

#include "pencilsettings.h"
#include "layer.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "colormanager.h"
#include "object.h"
#include "editor.h"
#include "layermanager.h"
#include "scribblearea.h"


EyedropperTool::EyedropperTool(QObject *parent) :
    BaseTool(parent)
{
}

void EyedropperTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
    properties.useFeather = -1;
}

QCursor EyedropperTool::cursor()
{
    if ( mEditor->preference()->isOn( SETTING::TOOL_CURSOR ) )
    {
        return QCursor(QPixmap(":icons/eyedropper.png"), 0, 15);
    } 
    else
    {
        return Qt::CrossCursor;
    }
}

QCursor EyedropperTool::cursor(const QColor colour)
{
    QPixmap icon(":icons/eyedropper.png");

    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.drawPixmap(0, 0, icon);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(colour);
    painter.drawRect(16, 16, 15, 15);
    painter.end();

    return QCursor(pixmap, 0, 15);
}

void EyedropperTool::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void EyedropperTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type() == Layer::BITMAP)
        {
            BitmapImage* targetImage = ((LayerBitmap *)layer)->getLastBitmapImageAtFrame( mEditor->currentFrame(), 0);
            //QColor pickedColour = targetImage->pixel(getLastPoint().x(), getLastPoint().y());
            QColor pickedColour;
            pickedColour.setRgba( targetImage->pixel( getLastPoint().x(), getLastPoint().y() ) );
            int transp = 255 - pickedColour.alpha();
            pickedColour.setRed( pickedColour.red() + transp );
            pickedColour.setGreen( pickedColour.green() + transp );
            pickedColour.setBlue( pickedColour.blue() + transp );
            if (pickedColour.alpha() != 0)
            {
                mEditor->color()->setColor(pickedColour);
            }
        }
        else if (layer->type() == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
            int colourNumber = vectorImage->getColourNumber(getLastPoint());
            if (colourNumber != -1)
            {
                mEditor->color()->setColorNumber(colourNumber);
            }
        }
    }
}

void EyedropperTool::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) { return; }

    if (layer->type() == Layer::BITMAP)
    {
        BitmapImage *targetImage = ((LayerBitmap *)layer)->getLastBitmapImageAtFrame(mEditor->currentFrame(), 0);
        if (targetImage->contains(getCurrentPoint()))
        {
            QColor pickedColour;
            //pickedColour.setRgba(targetImage->pixel(getCurrentPoint().x(), getCurrentPoint().y()));
            pickedColour.setRgba( targetImage->pixel( getCurrentPoint().x(), getCurrentPoint().y() ) );
            int transp = 255 - pickedColour.alpha();
            pickedColour.setRed( pickedColour.red() + transp );
            pickedColour.setGreen( pickedColour.green() + transp );
            pickedColour.setBlue( pickedColour.blue() + transp );
            if (pickedColour.alpha() != 0)
            {
                mScribbleArea->setCursor(cursor(pickedColour));
            }
            else
            {
                mScribbleArea->setCursor(cursor());
            }
        }
        else
        {
            mScribbleArea->setCursor(cursor());
        }
    }
    if (layer->type() == Layer::VECTOR)
    {
        VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
        int colourNumber = vectorImage->getColourNumber(getCurrentPoint());
        if (colourNumber != -1)
        {
            mScribbleArea->setCursor(cursor(mEditor->object()->getColour(colourNumber).colour));
        }
        else
        {
            mScribbleArea->setCursor(cursor());
        }
    }
}
