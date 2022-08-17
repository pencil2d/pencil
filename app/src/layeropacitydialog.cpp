#include "layeropacitydialog.h"
#include "ui_layeropacitydialog.h"

#include "layermanager.h"
#include "playbackmanager.h"
#include "layer.h"
#include "layerbitmap.h"
#include "bitmapimage.h"
#include "layervector.h"
#include "vectorimage.h"


LayerOpacityDialog::LayerOpacityDialog(QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::LayerOpacityDialog)
{
    ui->setupUi(this);
}

LayerOpacityDialog::~LayerOpacityDialog()
{
    delete ui;
}

void LayerOpacityDialog::setCore(Editor *editor)
{
    mEditor = editor;
    mLayerManager = mEditor->layers();
    mPlayBack = mEditor->playback();
}

void LayerOpacityDialog::initUI()
{

    connect(ui->chooseOpacitySlider, &QSlider::valueChanged, this, &LayerOpacityDialog::opacitySliderChanged);
    auto spinboxValueChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);
    connect(ui->chooseOpacitySpinBox, spinboxValueChanged, this, &LayerOpacityDialog::opacitySpinboxChanged);
    connect(ui->btnFadeIn, &QPushButton::pressed, this, &LayerOpacityDialog::fadeInPressed);
    connect(ui->btnFadeOut, &QPushButton::pressed, this, &LayerOpacityDialog::fadeOutPressed);
    connect(ui->btnClose, &QPushButton::pressed, this, &LayerOpacityDialog::close);
    connect(ui->rbActiveKeyframe, &QRadioButton::toggled, this, &LayerOpacityDialog::updateUI);
    connect(ui->rbSelectedKeyframes, &QRadioButton::toggled, this, &LayerOpacityDialog::updateUI);
    connect(ui->rbActiveLayer, &QRadioButton::toggled, this, &LayerOpacityDialog::updateUI);

    connect(this, &QDialog::finished, this, &LayerOpacityDialog::close);

    connect(mEditor, &Editor::objectLoaded, this, &LayerOpacityDialog::onObjectLoaded);
    connect(mEditor, &Editor::scrubbed, this, &LayerOpacityDialog::onCurrentFrameChanged);
    connect(mPlayBack, &PlaybackManager::playStateChanged, this, &LayerOpacityDialog::onPlayStateChanged);
    connect(mLayerManager, &LayerManager::currentLayerChanged, this, &LayerOpacityDialog::onCurrentLayerChanged);
    connect(mEditor, &Editor::selectedFramesChanged, this, &LayerOpacityDialog::onSelectedFramesChanged);

    onObjectLoaded();
}

void LayerOpacityDialog::updateUI()
{
    Layer* currentLayer = mLayerManager->currentLayer();
    if (currentLayer == nullptr) { return; }

    ui->labLayerInfo->setText(tr("Layer: %1").arg(currentLayer->name()));
    if (currentLayer->type() != Layer::BITMAP && currentLayer->type() != Layer::VECTOR) {
        setCanAdjust(false, false);
        return;
    }

    bool canAdjust = false;
    if (ui->rbActiveKeyframe->isChecked()) {
        KeyFrame* keyframe = currentLayer->getLastKeyFrameAtPosition(mEditor->currentFrame());
        canAdjust = keyframe != nullptr;
    } else if (ui->rbSelectedKeyframes->isChecked()) {
        canAdjust = !currentLayer->getSelectedFramesByPos().isEmpty();
    } else if (ui->rbActiveLayer->isChecked()) {
        canAdjust = true;
    }

    ui->chooseOpacitySlider->setEnabled(canAdjust);
    ui->chooseOpacitySpinBox->setEnabled(canAdjust);
}

void LayerOpacityDialog::onObjectLoaded()
{
    Layer* currentLayer = mLayerManager->currentLayer();
    if (currentLayer == nullptr) { return; }

    if (currentLayer->type() != Layer::BITMAP && currentLayer->type() != Layer::VECTOR) { return; }

    KeyFrame* keyframe = currentLayer->getLastKeyFrameAtPosition(mEditor->currentFrame());

    if (keyframe) {
        updateValues(getOpacityForKeyFrame(currentLayer, keyframe));
    } else {
        updateValues(100);
    }

    updateUI();
}

qreal LayerOpacityDialog::getOpacityForKeyFrame(Layer* layer, const KeyFrame* keyframe) const
{
    if (layer->type() == Layer::BITMAP) {
        const BitmapImage* bitmap = static_cast<const BitmapImage*>(keyframe);
        return bitmap->getOpacity();
    } else if (layer->type() == Layer::VECTOR) {
        const VectorImage* vector = static_cast<const VectorImage*>(keyframe);
        return vector->getOpacity();
    } else {
        return -1;
    }
}

void LayerOpacityDialog::setOpacityForKeyFrame(Layer* layer, KeyFrame* keyframe, qreal opacity)
{
    if (layer->type() == Layer::BITMAP) {
        BitmapImage* bitmap = static_cast<BitmapImage*>(keyframe);
        bitmap->setOpacity(opacity);
        layer->markFrameAsDirty(bitmap->pos());
    } else if (layer->type() == Layer::VECTOR) {
        VectorImage* vector = static_cast<VectorImage*>(keyframe);
        vector->setOpacity(opacity);
        layer->markFrameAsDirty(vector->pos());
    }
}

void LayerOpacityDialog::opacitySliderChanged(int value)
{
    ui->chooseOpacitySpinBox->setValue(value * mSpinBoxMultiplier);
    opacityValueChanged();
}

void LayerOpacityDialog::opacitySpinboxChanged(double value)
{
    ui->chooseOpacitySlider->setValue(static_cast<int>(value * 5.0));
    opacityValueChanged();
}

void LayerOpacityDialog::fade(OpacityFadeType fadeType)
{
    QSignalBlocker b1(ui->chooseOpacitySlider);
    QSignalBlocker b2(ui->chooseOpacitySpinBox);

    Layer* currentLayer = mLayerManager->currentLayer();
    if (currentLayer == nullptr) { return; }

    if (currentLayer->type() != Layer::BITMAP && currentLayer->type() != Layer::VECTOR) { return; }

    QList<int> selectedKeys = currentLayer->getSelectedFramesByPos();

    if (selectedKeys.count() < mMinSelectedFrames) { return; }

    // OUT
    int fadeFromPos = selectedKeys.first();
    int fadeStart = 1;
    int fadeEnd = selectedKeys.count();

    if (fadeType == OpacityFadeType::IN) {
        fadeFromPos = selectedKeys.last();
        fadeStart = 0;
        fadeEnd = selectedKeys.count() - 1;
    }

    KeyFrame* keyframe = currentLayer->getLastKeyFrameAtPosition(fadeFromPos);
    if (keyframe == nullptr) { return; }

    qreal initialOpacity = getOpacityForKeyFrame(currentLayer, keyframe);

    qreal imageCount = static_cast<qreal>(selectedKeys.count());
    for (int i = fadeStart; i < fadeEnd; i++)
    {
        keyframe = currentLayer->getLastKeyFrameAtPosition(selectedKeys.at(i));
        if (keyframe == nullptr) { continue; }

        qreal newOpacity = 0;
        if (fadeType == OpacityFadeType::IN) {
            newOpacity = static_cast<qreal>((i + 1) / imageCount) * initialOpacity;
        } else {
            newOpacity = static_cast<qreal>(initialOpacity - (i / imageCount) * initialOpacity);
        }
        setOpacityForKeyFrame(currentLayer, keyframe, newOpacity);
    }

    keyframe = currentLayer->getLastKeyFrameAtPosition(mEditor->currentFrame());
    if (keyframe == nullptr) { return; }

    qreal imageOpacity = getOpacityForKeyFrame(currentLayer, keyframe);
    updateValues(imageOpacity);

    emit mEditor->framesModified();
}

void LayerOpacityDialog::fadeInPressed()
{
    fade(OpacityFadeType::IN);
}

void LayerOpacityDialog::fadeOutPressed()
{
    fade(OpacityFadeType::OUT);
}

void LayerOpacityDialog::onCurrentLayerChanged(int)
{
    onCurrentFrameChanged(mEditor->currentFrame());
    updateUI();
}

void LayerOpacityDialog::onCurrentFrameChanged(int frame)
{
    if (mPlayerIsPlaying) { return; }

    Layer* currentLayer = mLayerManager->currentLayer();
    if (currentLayer == nullptr) { return; }

    if (currentLayer->type() != Layer::BITMAP && currentLayer->type() != Layer::VECTOR) {
        setCanAdjust(false, false);
        return;
    }

    KeyFrame* keyframe = currentLayer->getLastKeyFrameAtPosition(frame);
    if (keyframe) {
        updateValues(getOpacityForKeyFrame(currentLayer, keyframe));
    }
    updateUI();
}

void LayerOpacityDialog::onSelectedFramesChanged()
{
    Layer* currentLayer = mLayerManager->currentLayer();
    if (currentLayer == nullptr) { return; }

    QList<int> frames = currentLayer->getSelectedFramesByPos();

    ui->groupBoxFade->setEnabled(frames.count() >= mMinSelectedFrames);
    updateUI();
}

void LayerOpacityDialog::onPlayStateChanged(bool isPlaying)
{
    mPlayerIsPlaying = isPlaying;

    if (!mPlayerIsPlaying) {
        onCurrentFrameChanged(mEditor->currentFrame());
    }
    updateUI();
}

void LayerOpacityDialog::updateValues(qreal opacity)
{
    QSignalBlocker b1(ui->chooseOpacitySlider);
    QSignalBlocker b2(ui->chooseOpacitySpinBox);

    int newOpacity = static_cast<int>(opacity * mMultiplier);
    ui->chooseOpacitySlider->setValue(newOpacity);
    ui->chooseOpacitySpinBox->setValue(newOpacity * mSpinBoxMultiplier);
}

void LayerOpacityDialog::opacityValueChanged()
{
    if (ui->rbActiveKeyframe->isChecked()) {
        setOpacityForCurrentKeyframe();
    } else if (ui->rbActiveLayer->isChecked()) {
        setOpacityForLayer();
    } else if (ui->rbSelectedKeyframes->isChecked()) {
        setOpacityForSelectedKeyframes();
    }
}

void LayerOpacityDialog::setOpacityForCurrentKeyframe()
{
    Layer* currentLayer = mLayerManager->currentLayer();
    if (currentLayer == nullptr) { return; }

    if (currentLayer->type() != Layer::BITMAP && currentLayer->type() != Layer::VECTOR) { return; }

    KeyFrame* keyframe = currentLayer->getLastKeyFrameAtPosition(mEditor->currentFrame());
    if (keyframe == nullptr) { return; }

    qreal opacity = ui->chooseOpacitySlider->value() / mMultiplier;
    setOpacityForKeyFrame(currentLayer, keyframe, opacity);

    emit mEditor->framesModified();
}

void LayerOpacityDialog::setOpacityForSelectedKeyframes()
{
    Layer* currentLayer = mLayerManager->currentLayer();
    if (currentLayer == nullptr) { return; }

    QList<int> frames = currentLayer->getSelectedFramesByPos();

    if (frames.isEmpty()) { return; }

    qreal opacity = static_cast<qreal>(ui->chooseOpacitySlider->value()) / mMultiplier;

    for (int pos : frames)
    {
        KeyFrame* keyframe = currentLayer->getKeyFrameAt(pos);
        Q_ASSERT(keyframe);

        setOpacityForKeyFrame(currentLayer, keyframe, opacity);
    }

    emit mEditor->framesModified();
}

void LayerOpacityDialog::setOpacityForLayer()
{
    Layer* currentLayer = mLayerManager->currentLayer();
    if (currentLayer == nullptr) { return; }

    qreal opacity = static_cast<qreal>(ui->chooseOpacitySlider->value()) / mMultiplier;

    currentLayer->foreachKeyFrame([this, currentLayer, opacity](KeyFrame* keyframe) {
        Q_ASSERT(keyframe);

        setOpacityForKeyFrame(currentLayer, keyframe, opacity);
    });

    emit mEditor->framesModified();
}

void LayerOpacityDialog::setCanAdjust(bool opacity, bool fade)
{
    ui->groupBoxFade->setEnabled(fade);
    ui->chooseOpacitySlider->setEnabled(opacity);
    ui->chooseOpacitySpinBox->setEnabled(opacity);
}
