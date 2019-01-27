#ifndef BITMAPCOLORING_H
#define BITMAPCOLORING_H

#include "basedockwidget.h"
#include "editor.h"
#include "layerbitmap.h"

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

signals:

public slots:

private:
    Ui::BitmapColoringWidget* ui = nullptr;
    Editor* mEditor = nullptr;
    LayerBitmap* mLayerBitmap = nullptr;

};

#endif // BITMAPCOLORING_H
