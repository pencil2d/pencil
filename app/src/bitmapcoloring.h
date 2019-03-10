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
    void colorMethodChanged();
    void checkBlackBoxes();
    void checkRedBoxes();
    void checkGreenBoxes();
    void checkBlueBoxes();
    void checkAllKeyframesBoxes();
    void tabWidgetClicked(int index);
    // 1: Trace
    void updateTraceBoxes();
    void updateBtnSelect();
    void activateSelectTool();
    void setThreshold(int threshold);
    void traceLines();
    // 2: Thin
    void updateThinBoxes();
    void setSpotArea(int size);
    void thinLines();
    // 3: Blend
    void updateBlendBoxes();
    void blendLines();

protected:
    void prepareLines();
    void trace();
    void thin();
    void blend(LayerBitmap* artLayer);

private:
    Ui::BitmapColoringWidget* ui = nullptr;
    Editor* mEditor = nullptr;
    ScribbleArea* mScribblearea = nullptr;
    LayerBitmap* mLayerBitmap = nullptr;
    BitmapImage* mBitmapImage = nullptr;
    bool mSelectAreas = false;

};

#endif // BITMAPCOLORING_H
