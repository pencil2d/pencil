#ifndef LAYEROPACITYDIALOG_H
#define LAYEROPACITYDIALOG_H

#include <QDialog>
#include "editor.h"

class QTimer;

namespace Ui {
class LayerOpacityDialog;
}

class LayerOpacityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LayerOpacityDialog(QWidget *parent = nullptr);
    ~LayerOpacityDialog();

    void setCore(Editor* editor);
    void init();

private slots:
    void previousKeyframePressed();
    void nextKeyframePressed();
    void opacitySliderChanged(int value);
    void opacitySpinboxChanged(int value);
    void allLayerOpacity();
    void fadeInPressed();
    void fadeOutPressed();

private:

    void setOpacityCurrentKeyframe();

    Ui::LayerOpacityDialog *ui;

    QTimer* mTimer = nullptr;
    int mTimeOut = 250;
    Editor* mEditor = nullptr;
};

#endif // LAYEROPACITYDIALOG_H
