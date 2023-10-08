#ifndef BITMAPCOLORING_H
#define BITMAPCOLORING_H

#include "basedockwidget.h"

class BitmapImage;
class Editor;
class LayerBitmap;
class ScribbleArea;

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
    void onVisibilityChanged(bool visibility);

private slots:
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
    void prepareAndTraceLines();
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

private:
    void prepareLines();
    void trace();
    void thin();
    void blend(LayerBitmap* artLayer);
    void nonValidBitmap(int frame);
    void updateTraceButtons();

    Ui::BitmapColoringWidget* ui = nullptr;
    Editor* mEditor = nullptr;
    ScribbleArea* mScribblearea = nullptr;
    LayerBitmap* mLayerBitmap = nullptr;
    BitmapImage* mBitmapImage = nullptr;
    int mAnimLayer = 0; // Animation layer index
    int mColLayer = 0;  // Coloring layer index
    bool mRedChecked = false;
    bool mGreenChecked = false;
    bool mBlueChecked = false;

};

#endif // BITMAPCOLORING_H
