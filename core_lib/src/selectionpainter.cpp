#include "selectionpainter.h"

#include "object.h"
#include "qpainter.h"
#include "layermanager.h"
#include "basetool.h"


SelectionPainter::SelectionPainter()
{

}

void SelectionPainter::paint(QPainter& painter,
                             const Object* object,
                             int layerIndex,
                             BaseTool* tool,
                             TransformParameters& tParams)
{
    Layer* layer = object->getLayer(layerIndex);

    if (layer == nullptr) { return; }

    if (layer->type() == Layer::BITMAP)
    {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::DashLine));

        // Draw previous selection
        painter.drawPolygon(tParams.lastSelectionPolygonF.toPolygon());

        // Draw current selection
        painter.drawPolygon(tParams.currentSelectionPolygonF.toPolygon());

    }
    if (layer->type() == Layer::VECTOR)
    {
        painter.setBrush(QColor(0, 0, 0, 20));
        painter.setPen(Qt::gray);
        painter.drawPolygon(tParams.currentSelectionPolygonF);
    }

    if (layer->type() != Layer::VECTOR || tool->type() != SELECT)
    {
        painter.setPen(Qt::SolidLine);
        painter.setBrush(QBrush(Qt::gray));
        int width = 6;
        int radius = width / 2;

        const QRectF topLeftCorner = QRectF(tParams.currentSelectionPolygonF[0].x() - radius,
                                            tParams.currentSelectionPolygonF[0].y() - radius,
                                            width, width);
        painter.drawRect(topLeftCorner);

        const QRectF topRightCorner = QRectF(tParams.currentSelectionPolygonF[1].x() - radius,
                                             tParams.currentSelectionPolygonF[1].y() - radius,
                                             width, width);
        painter.drawRect(topRightCorner);

        const QRectF bottomRightCorner = QRectF(tParams.currentSelectionPolygonF[2].x() - radius,
                                                tParams.currentSelectionPolygonF[2].y() - radius,
                                                width, width);
        painter.drawRect(bottomRightCorner);

        const QRectF bottomLeftCorner = QRectF(tParams.currentSelectionPolygonF[3].x() - radius,
                                               tParams.currentSelectionPolygonF[3].y() - radius,
                                               width, width);
        painter.drawRect(bottomLeftCorner);

        painter.setBrush(QColor(0, 255, 0, 50));
        painter.setPen(Qt::green);
    }
}
