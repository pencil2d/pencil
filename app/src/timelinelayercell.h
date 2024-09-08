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
    Q_OBJECT
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
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paint(QPainter& painter, const QPalette& palette) const override;

    int getLayerNumber(int posY) const;

    bool didDetach() const { return mDidDetach; }
    const Layer* layer() const { return mLayer; }

signals:
    void drag(const DragEvent& dragEvent, const TimeLineLayerCell* cell, int x, int y);

private:
    void handleDragStarted(QMouseEvent* event);
    void handleDragging(QMouseEvent* event);
    void handleDragEnded(QMouseEvent* event);

    bool isInsideLayerVisibilityArea(QMouseEvent* event) const;

    bool hasDetached(int yOffset) const { return abs(yOffset) > mDetachThreshold; }

    void paintBackground(QPainter& painter, const QPalette& palette, bool isSelected) const;
    void paintLayerVisibility(QPainter& painter, const QPalette& palette, const LayerVisibility& layerVisibility, bool isSelected) const;
    void paintLabel(QPainter& painter, const QPalette& palette, bool isSelected) const;
    void paintLayerGutter(QPainter& painter, const QPalette& palette) const;

    void editLayerProperties(LayerCamera* cameraLayer) const;
    void editLayerName(Layer* layer) const;

    QSize mLabelIconSize = QSize(22,22);
    Layer* mLayer = nullptr;
    bool mDidDetach = false;
    bool mIsDraggable = false;

    QPixmap mIconPixmap;

    int mDetachThreshold = 5;
    
    QRect mOldBounds = QRect();
};

#endif // TIMELINELAYERCELL_H
