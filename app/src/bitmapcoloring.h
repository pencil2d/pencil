#ifndef BITMAPCOLORING_H
#define BITMAPCOLORING_H

#include "basedockwidget.h"
#include "editor.h"
#include "layerbitmap.h"
#include "bitmapimage.h"
#include <QTabWidget>
#include "scribblearea.h"

class Layer;

namespace Ui
{
class BitmapColoringWidget;
}

class BitmapColoring : public BaseDockWidget
{
    Q_OBJECT

public:
    explicit BitmapColoring(Editor* editor, QWidget *parent);
    ~BitmapColoring() override;

    void initUI() override;
    void updateUI() override;
    void visibilityChanged(bool visibility);

signals:

public slots:
    void checkRedBoxes();
    void checkGreenBoxes();
    void checkBlueBoxes();
    void checkAllKeyframesBoxes();
    void tabWidgetClicked(int index);
    void resetColoringDock();
    void infoBox();
    void enableTabs(int index);
    // 1: Trace
    void updateTraceBoxes();
    void setThreshold(int threshold);
    void traceLines();
    // 2: Thin
    void updateFillSpotsButton();
    void fillSpotAreas();
    void updateThinBoxes();
    void setSpotArea(int size);
    void thinLines();
    // 3: Blend
    void updateBlendBoxes();
    void blendLines();

protected:
    void prepareAndTraceLines();
    void prepareLines();
    void trace();
    void thin();
    void blend(LayerBitmap* artLayer);
    void nonValidBitmap(int frame);
    void updateTraceButtons();

private:
    Ui::BitmapColoringWidget* ui = nullptr;
    Editor* mEditor = nullptr;
    ScribbleArea* mScribblearea = nullptr;
    LayerBitmap* mLayerBitmap = nullptr;
    BitmapImage* mBitmapImage = nullptr;
    bool mSelectAreas = false;
    int mAnimLayer = 0; // Animation layer index
    int mColLayer = 0;  // Coloring layer index
    bool mRedChecked = false;
    bool mGreenChecked = false;
    bool mBlueChecked = false;

};

#endif // BITMAPCOLORING_H
