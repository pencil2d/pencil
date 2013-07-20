#include <QPixmap>

#include "pentool.h"

#include "pencilsettings.h"
#include "editor.h"
#include "scribblearea.h"

PenTool::PenTool()
{

}

ToolType PenTool::type()
{
    return PEN;
}

void PenTool::loadSettings()
{
    QSettings settings("Pencil","Pencil");
    properties.width = settings.value("penWidth").toDouble();    
    properties.colourNumber = 0;
    properties.feather = 0;    
    properties.pressure = ON;
    properties.invisibility = OFF;
    properties.preserveAlpha = OFF;

    if ( properties.width <= 0 )
    {
        properties.width = 1.5;
        settings.setValue("penWidth", properties.width);
    }

    if ( properties.feather < 0 )
    {
        properties.feather = 0;
    }
}

QCursor PenTool::cursor()
{
    if ( pencilSettings()->value( kSettingToolCursor ).toBool() )
    {
        return QCursor(QPixmap(":icons/pen.png"), 7, 0);
    }
    else
    {
        return Qt::CrossCursor;
    }
}

void PenTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
    //editor->currentColor = getTool( PEN )->properties.colour;
    //editor->currentColor.setAlphaF(pen.colour.alphaF());
    if (m_pScribbleArea->usePressure && !mouseDevice)
    {
        m_pScribbleArea->currentWidth = 2.0 * properties.width * pressure;
    }
    else
    {
        m_pScribbleArea->currentWidth = properties.width;
    }
    // we choose the "normal" width to correspond to a pressure 0.5
}

void PenTool::mousePressEvent(QMouseEvent *event)
{
    // sanity checks
    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type == Layer::VECTOR)
    {
        m_pEditor->selectVectorColourNumber(properties.colourNumber);
    }

    if (event->button() == Qt::LeftButton)
    {
        m_pEditor->backup(typeName());
        m_pScribbleArea->updateAll = true;
    }

    startStroke();
}

void PenTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (event->button() == Qt::LeftButton)
    {
        drawStroke();

        if (layer->type == Layer::BITMAP)
        {
            m_pScribbleArea->paintBitmapBuffer();
            m_pScribbleArea->updateAll = true;
        }
        else if (layer->type == Layer::VECTOR && strokePoints.size() > -1)
        {
            // Clear the temporary pixel path
            m_pScribbleArea->bufferImg->clear();
            qreal tol = m_pScribbleArea->curveSmoothing / qAbs(m_pScribbleArea->myView.m11());
            BezierCurve curve(strokePoints, strokePressures, tol);
            curve.setWidth(properties.width);
            curve.setFeather(0);
            curve.setInvisibility(false);
            curve.setVariableWidth(m_pScribbleArea->usePressure);
            curve.setColourNumber(properties.colourNumber);

            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);

            vectorImage->addCurve(curve, qAbs(m_pScribbleArea->myView.m11()));
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->updateAll = true;
        }
    }

    endStroke();
}
