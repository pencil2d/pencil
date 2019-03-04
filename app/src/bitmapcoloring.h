#ifndef BITMAPCOLORING_H
#define BITMAPCOLORING_H

#include "basedockwidget.h"
#include "editor.h"
#include "layerbitmap.h"
#include "bitmapimage.h"
#include <QTabWidget>

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
    void updateSelectButtonIcon();
    void checkBlackBoxes();
    void checkRedBoxes();
    void checkGreenBoxes();
    void checkBlueBoxes();
    void checkAllKeyframes();
    // Prepare
    void updateTraceBoxes();
    void updateBtnSelect();
    void setThreshold(int threshold);
    void selectAreas();
    void cancelSelectAreas();
    void traceLines();
    void selectFromScans();
    void prepareLines();
    // Thin
    void updateThinBoxes();
    void setSpotArea(int size);
    void thinLines();
    // Finish
    void updateFinishBoxes();
    void blendLines();

private:
    Ui::BitmapColoringWidget* ui = nullptr;
    Editor* mEditor = nullptr;
    LayerBitmap* mLayerBitmap = nullptr;
    BitmapImage* mBitmapImage = nullptr;
    bool mSelectAreas = false;

};

#endif // BITMAPCOLORING_H
