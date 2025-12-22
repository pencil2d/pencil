/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "selectionpainter.h"

#include "object.h"
#include "qpainter.h"

SelectionPainter::SelectionPainter()
{
}

void SelectionPainter::paint(QPainter& painter,
                             const Object* object,
                             int layerIndex,
                             const TransformToolProperties& toolProperties,
                             TransformParameters& tParams)
{
    Layer* layer = object->getLayer(layerIndex);

    if (layer == nullptr) { return; }

    QTransform transform = tParams.selectionTransform * tParams.viewTransform;
    QPolygonF projectedSelectionPolygon = transform.map(tParams.originalSelectionRectF);

    if (layer->type() == Layer::BITMAP)
    {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::DashLine));

        // Draw current selection
        painter.drawPolygon(projectedSelectionPolygon.toPolygon());

    }
    if (layer->type() == Layer::VECTOR)
    {
        painter.setBrush(QColor(0, 0, 0, 20));
        painter.setPen(Qt::gray);
        painter.drawPolygon(projectedSelectionPolygon);
    }

    painter.setPen(Qt::SolidLine);
    painter.setBrush(QBrush(Qt::gray));
    int radius = HANDLE_WIDTH / 2;

    const QRectF topLeftCorner = QRectF(projectedSelectionPolygon[0].x() - radius,
                                        projectedSelectionPolygon[0].y() - radius,
                                        HANDLE_WIDTH, HANDLE_WIDTH);
    painter.drawRect(topLeftCorner);

    const QRectF topRightCorner = QRectF(projectedSelectionPolygon[1].x() - radius,
                                         projectedSelectionPolygon[1].y() - radius,
                                         HANDLE_WIDTH, HANDLE_WIDTH);
    painter.drawRect(topRightCorner);

    const QRectF bottomRightCorner = QRectF(projectedSelectionPolygon[2].x() - radius,
                                            projectedSelectionPolygon[2].y() - radius,
                                            HANDLE_WIDTH, HANDLE_WIDTH);
    painter.drawRect(bottomRightCorner);

    const QRectF bottomLeftCorner = QRectF(projectedSelectionPolygon[3].x() - radius,
                                           projectedSelectionPolygon[3].y() - radius,
                                           HANDLE_WIDTH, HANDLE_WIDTH);
    painter.drawRect(bottomLeftCorner);

    if (toolProperties.showSelectionInfoEnabled()) {
        paintSelectionInfo(painter, transform, tParams.viewTransform, tParams.originalSelectionRectF, projectedSelectionPolygon);
    }
}

void SelectionPainter::paintSelectionInfo(QPainter& painter, const QTransform& mergedTransform, const QTransform& viewTransform, const QRectF& selectionRect, const QPolygonF& projectedPolygonF)
{
    QRect projectedSelectionRect = mergedTransform.mapRect(selectionRect).toAlignedRect();
    QRect originalSelectionRect = viewTransform.mapRect(selectionRect).toAlignedRect();
    QPolygon projectedPolygon = projectedPolygonF.toPolygon();

    QPoint projectedCenter = projectedSelectionRect.center();
    QPoint originalCenter = originalSelectionRect.center();
    int diffX = static_cast<int>(projectedCenter.x() - originalCenter.x());
    int diffY = static_cast<int>(originalCenter.y() - projectedCenter.y());
    painter.drawText(projectedPolygon[0] - QPoint(HANDLE_WIDTH, HANDLE_WIDTH),
                    QString("Size: %1x%2. Diff: %3, %4.").arg(QString::number(selectionRect.width()),
                                                      QString::number(selectionRect.height()),
                                                      QString::number(diffX),
                                                      QString::number(diffY)));
}
