#ifndef TIMELINELAYERCELL_H
#define TIMELINELAYERCELL_H

#include <QWidget>
#include <QPainter>
#include <QPalette>

#include "timeline.h"
#include "editor.h"
#include "timelinebasecell.h"

class Layer;
class LayerCamera;

class PreferenceManager;

class TimeLineLayerCell : public TimeLineBaseCell
{
public:
    TimeLineLayerCell(TimeLine* parent,
                      Editor* editor,
                      Layer* layer,
                      const QPoint& origin, int width, int height);
    ~TimeLineLayerCell() override;

    TimeLineCellType type() const override { return TimeLineCellType::LAYER; }

    void editLayerProperties() const;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paint(QPainter& painter, const QPalette& palette) const override;

    int getLayerNumber(int posY) const;

private:

    void paintBackground(QPainter& painter, const QPalette& palette, bool isSelected) const;
    void paintLayerVisibility(QPainter& painter, const QPalette& palette, const LayerVisibility& layerVisibility, bool isSelected) const;
    void paintLabel(QPainter& painter, const QPalette& palette, bool isSelected) const;

    void editLayerProperties(LayerCamera* cameraLayer) const;
    void editLayerName(Layer* layer) const;

    QSize mLabelIconSize = QSize(22,22);
    Layer* mLayer = nullptr;
};

#endif // TIMELINELAYERCELL_H
