#ifndef TIMELINELAYERCELLEDITORWIDGET_H
#define TIMELINELAYERCELLEDITORWIDGET_H

#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>

#include "pencildef.h"
#include "timelinedef.h"

class TimeLine;
class Editor;
class Layer;
class LayerCamera;
class PreferenceManager;

class TimeLineLayerCellEditorWidget : public QWidget
{
    Q_OBJECT
public:
    TimeLineLayerCellEditorWidget(QWidget* parent,
                                  Editor* editor, Layer* layer);

    TimeLineCellType type() const { return TimeLineCellType::LAYER; }

    void editLayerProperties() const;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    int getLayerNumber(int posY) const;

    bool didDetach() const { return mDidDetach; }
    const Layer* layer() const { return mLayer; }

signals:
    void drag(const DragEvent& dragEvent, TimeLineLayerCellEditorWidget* cell, int x, int y);
    void layerVisibilityChanged();

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
    bool mDidDetach = false;
    bool mIsDraggable = false;

    QPixmap mIconPixmap;

    int mDetachThreshold = 5;

    Editor* mEditor = nullptr;
    Layer* mLayer = nullptr;

    PreferenceManager* mPrefs = nullptr;

    int mDragFromY = 0;
    int mOldY = 0;
};

#endif // TIMELINELAYERCELLEDITORWIDGET_H
