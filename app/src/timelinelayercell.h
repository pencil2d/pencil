#ifndef TIMELINELAYERCELL_H
#define TIMELINELAYERCELL_H

#include <QWidget>
#include <QPainter>
#include <QPalette>

#include "timeline.h"
#include "editor.h"

class Layer;
class LayerCamera;

class PreferenceManager;

class TimeLineLayerCell
{
public:
    TimeLineLayerCell(TimeLine* parent,
                      Editor* editor,
                      Layer* layer,
                      const QPalette& palette,
                      const QRect& rect);

    void paint(QPainter& painter, bool isSelected, const LayerVisibility& LayerVisibility) const;

    void editLayerProperties() const;
    void setGlobalPos(int x, int y) { mGlobalX = x; mGlobalY = y; }

    int posX() const { return mGlobalX + mLocalRect.x(); }
    int posY() const { return mGlobalY + mLocalRect.y(); }

private:

    void paintBackground(QPainter& painter, bool isSelected) const;
    void paintLayerVisibility(QPainter& painter, const LayerVisibility& layerVisibility, bool isSelected) const;
    void paintLabel(QPainter& painter, bool isSelected) const;

    void editLayerProperties(LayerCamera* cameraLayer) const;
    void editLayerName(Layer* layer) const;

    QRect mLocalRect;
    int mGlobalX = 0;
    int mGlobalY = 0;

    QSize labelIconSize = QSize(22,22);

    QPalette mPalette;

    Layer* mLayer = nullptr;
    TimeLine* mTimeLine = nullptr;
    Editor* mEditor = nullptr;
    PreferenceManager* mPrefs = nullptr;
};

#endif // TIMELINELAYERCELL_H
