#include "selectionpainter.h"

#include "object.h"
#include "qpainter.h"
#include "layermanager.h"
#include "basetool.h"
#include <QDebug>


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

        if (tool->properties.showInfoIndex != 0)
        {
            int diffX, diffY;
            if (tool->properties.showInfoIndex == 1)
            {
                diffX = static_cast<int>(tParams.currentSelectionPolygonF.boundingRect().x() - mOriginalSelectionPolygonF.boundingRect().x());
                diffY = static_cast<int>(tParams.currentSelectionPolygonF.boundingRect().y() - mOriginalSelectionPolygonF.boundingRect().y());
            }
            else
            {
                diffX = static_cast<int>(tParams.currentSelectionPolygonF.boundingRect().x() - tParams.lastSelectionPolygonF.boundingRect().x());
                diffY = static_cast<int>(tParams.currentSelectionPolygonF.boundingRect().y() - tParams.lastSelectionPolygonF.boundingRect().y());
            }
            painter.drawText(static_cast<int>(tParams.currentSelectionPolygonF[0].x()),
                    static_cast<int>(tParams.currentSelectionPolygonF[0].y() - width),
                    QString("Size: %1x%2. Diff: %3, %4.").arg(QString::number(tParams.currentSelectionPolygonF.boundingRect().width()),
                                                               QString::number(tParams.currentSelectionPolygonF.boundingRect().height()),
                                                               QString::number(diffX),
                                                               QString::number(diffY)));
        }

        painter.setBrush(QColor(0, 255, 0, 50));
        painter.setPen(Qt::green);
    }
}
