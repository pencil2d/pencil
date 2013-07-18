#include <QSettings>
#include <QPixmap>
#include <QMouseEvent>

#include "editor.h"
#include "scribblearea.h"

#include "pencilsettings.h"
#include "penciltool.h"

#include "layer.h"

PencilTool::PencilTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType PencilTool::type()
{
    return PENCIL;
}

void PencilTool::loadSettings()
{
    QSettings settings("pencil", "pencil");

    properties.width = settings.value("pencilWidth").toDouble();    
    properties.colourNumber = 0;
    properties.feather = -1;
    properties.opacity = 0.8;
    properties.pressure = 1;
    properties.invisibility = 1;
    properties.preserveAlpha = 0;

    if (properties.width == 0)
    {
        properties.width = 1;
        settings.setValue("pencilWidth", properties.width);
    }
}

QCursor PencilTool::cursor()
{
    if ( pencilSettings()->value( kSettingToolCursor ).toBool() )
    {
        return QCursor(QPixmap(":icons/pencil2.png"), 0, 16);
    }
    else
    {
        return Qt::CrossCursor;
    }
}

void PencilTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_pEditor->backup(typeName(type()));

        if (!m_pScribbleArea->showThinLines)
        {
            m_pScribbleArea->toggleThinLines();
        }
        m_pScribbleArea->mousePath.append(m_pScribbleArea->lastPoint);
        m_pScribbleArea->updateAll = true;
    }
}

void PencilTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
    {
        if (event->buttons() & Qt::LeftButton)
        {
            m_pScribbleArea->drawLineTo(m_pScribbleArea->currentPixel, m_pScribbleArea->currentPoint);
        }
    }
}

void PencilTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP)
        {
            m_pScribbleArea->paintBitmapBuffer();
            m_pScribbleArea->updateAll = true;
        }
        else if (layer->type == Layer::VECTOR &&  m_pScribbleArea->mousePath.size() > -1)
        {
            // Clear the temporary pixel path
            m_pScribbleArea->bufferImg->clear();
            qreal tol = m_pScribbleArea->curveSmoothing / qAbs(m_pScribbleArea->myView.m11());
            BezierCurve curve(m_pScribbleArea->mousePath, m_pScribbleArea->mousePressure, tol);
            curve.setWidth(0);
            curve.setFeather(0);
            curve.setInvisibility(true);
            curve.setVariableWidth(false);
            curve.setColourNumber(properties.colourNumber);
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);

            //curve.setSelected(true);
            //qDebug() << "this curve has " << curve.getVertexSize() << "vertices";

            vectorImage->addCurve(curve, qAbs(m_pScribbleArea->myView.m11()));
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->updateAll = true;
        }
    }

}
