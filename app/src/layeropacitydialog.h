#ifndef LAYEROPACITYDIALOG_H
#define LAYEROPACITYDIALOG_H

#include <QDialog>
#include "editor.h"

class QTimer;
class LayerManager;
class PlaybackManager;
class Layer;

namespace Ui {
class LayerOpacityDialog;
}

enum class OpacityFadeType {
    IN,
    OUT
};

class LayerOpacityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LayerOpacityDialog(QWidget *parent = nullptr);
    ~LayerOpacityDialog();

    void setCore(Editor* editor);
    void initUI();

private slots:
    void opacitySliderChanged(int value);
    void opacitySpinboxChanged(double value);
    void fadeInPressed();
    void fadeOutPressed();

    void onObjectLoaded();
    void onCurrentLayerChanged(int index);
    void onCurrentFrameChanged(int frame);
    void onSelectedFramesChanged();
    void onPlayStateChanged(bool isPlaying);

private:

    void fade(OpacityFadeType fadeType);
    void setEnabled(bool enabled);
    qreal getOpacityForKeyFrame(Layer* layer, const KeyFrame* keyframe) const;
    void setOpacityForKeyFrame(Layer* layer, KeyFrame* keyframe, qreal opacity);

    /** setOpacityForKeyFrames
     *  @param[in] opacity a value between 0 and 1
     *  @param[in] startPos Keyframe pos
     *  @param[in] endPos Keyframe pos, should always be higher than statPos
     * */
    void setOpacityForKeyFrames(qreal opacity, int startPos, int endPos);
    void updateValues(qreal opacity);
    void opacityValueChanged();
    void setOpacityForCurrentKeyframe();
    void setOpacityForSelectedKeyframes();
    void setOpacityForLayer();

    Ui::LayerOpacityDialog *ui;

    Editor* mEditor = nullptr;
    LayerManager* mLayerManager = nullptr;
    PlaybackManager* mPlayBack = nullptr;

    bool mPlayerIsPlaying = false;

    qreal mMultiplier = 500.0;
    qreal mSpinBoxMultiplier = 0.2;
    int mMinSelectedFrames = 3;
};

#endif // LAYEROPACITYDIALOG_H
