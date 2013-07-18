
#include <QPixmap>
#include "handtool.h"

#include "layer.h"
#include "editor.h"
#include "scribblearea.h"
#include "layercamera.h"

HandTool::HandTool()
{

}

ToolType HandTool::type()
{
    return HAND;
}

void HandTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}

QCursor HandTool::cursor()
{
    return QPixmap(":icons/hand.png");
}

void HandTool::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void HandTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    m_pScribbleArea->bufferImg->clear();
    if (layer->type == Layer::CAMERA)
    {
        LayerCamera *layerCamera = (LayerCamera *)layer;
        QMatrix view = layerCamera->getViewAtFrame(m_pEditor->m_nCurrentFrameIndex);
        layerCamera->loadImageAtFrame(m_pEditor->m_nCurrentFrameIndex, view * m_pScribbleArea->transMatrix);
        //Camera* camera = ((LayerCamera*)layer)->getLastCameraAtFrame(editor->currentFrame, 0);
        //camera->view = camera->view * transMatrix;
    }
    else
    {
        m_pScribbleArea->myView =  m_pScribbleArea->myView * m_pScribbleArea->transMatrix;
    }
    m_pScribbleArea->transMatrix.reset();
    m_pScribbleArea->updateAllVectorLayers();
    m_pScribbleArea->updateAll = true;

    //---- stop the hand tool if this was mid button
    if (event->button() == Qt::MidButton)
    {
        //qDebug("Stop Hand Tool");
        m_pScribbleArea->setPrevMode();
    }

    //update();
}

void HandTool::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "hand tool move button";
    if (event->button() != Qt::NoButton)
    {
        qDebug() << "hand tool move";
        if (event->modifiers() & Qt::ControlModifier || event->modifiers() & Qt::AltModifier || event->buttons() & Qt::RightButton)
        {
            QPoint centralPixel(m_pScribbleArea->width() / 2, m_pScribbleArea->height() / 2);
            if (m_pScribbleArea->lastPixel.x() != centralPixel.x())
            {
                qreal scale = 1.0;
                qreal cosine = 1.0;
                qreal sine = 0.0;
                if (event->modifiers() & Qt::AltModifier)    // rotation
                {
                    QPointF V1 = m_pScribbleArea->lastPixel - centralPixel;
                    QPointF V2 = m_pScribbleArea->currentPixel - centralPixel;
                    cosine = (V1.x() * V2.x() + V1.y() * V2.y()) / (BezierCurve::eLength(V1) * BezierCurve::eLength(V2));
                    sine = (-V1.x() * V2.y() + V1.y() * V2.x()) / (BezierCurve::eLength(V1) * BezierCurve::eLength(V2));

                }
                if (event->modifiers() & Qt::ControlModifier || event->buttons() & Qt::RightButton)    // scale
                {
                    scale = exp(0.01 * (m_pScribbleArea->currentPixel.y() - m_pScribbleArea->lastPixel.y()));
                }
                m_pScribbleArea->transMatrix = QMatrix(
                                  scale * cosine, -scale * sine,
                                  scale * sine,  scale * cosine,
                                  0.0,
                                  0.0
                              );
            }
        }
        else     // translation
        {
            m_pScribbleArea->transMatrix.setMatrix(1.0, 0.0, 0.0,
                                                   1.0,
                                                   m_pScribbleArea->currentPixel.x() - m_pScribbleArea->lastPixel.x(),
                                                   m_pScribbleArea->currentPixel.y() - m_pScribbleArea->lastPixel.y());
        }
        m_pScribbleArea->update();
        m_pScribbleArea->updateAll = true;
    }
}
