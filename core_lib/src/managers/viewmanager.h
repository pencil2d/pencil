/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QTransform>
#include "basemanager.h"

class Layer;
class LayerCamera;
class Camera;


class ViewManager : public BaseManager
{
    Q_OBJECT

public:
    explicit ViewManager(Editor* editor);

    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;
    void workingLayerChanged(Layer* layer) override;

    QTransform getView();
    QTransform getViewInverse();
    void resetView();

    QPointF mapCanvasToScreen(QPointF p);
    QPointF mapScreenToCanvas(QPointF p);

    QRectF mapCanvasToScreen(const QRectF& rect);
    QRectF mapScreenToCanvas(const QRectF& rect);

    QPainterPath mapCanvasToScreen(const QPainterPath& path);
    QPainterPath mapScreenToCanvas(const QPainterPath& path);

    QPointF translation();
    void translate(float dx, float dy);
    void translate(QPointF offset);

    float rotation();
    void rotate(float degree);

    float scaling();
    void scale(float scaleValue);
    void scaleUp();
    void scaleDown();

    void flipHorizontal(bool b);
    void flipVertical(bool b);

    bool isFlipHorizontal() { return mIsFlipHorizontal; }
    bool isFlipVertical() { return mIsFlipVertical; }

    void setCanvasSize(QSize size);
    void setCameraLayer(Layer* layer);

    void updateViewTransforms();

    Q_SIGNAL void viewChanged();
    Q_SIGNAL void viewFlipped();

private:

    void onCurrentFrameChanged();

    QTransform mView;
    QTransform mViewInverse;
    QTransform mViewCanvas;
    QTransform mViewCanvasInverse;
    QTransform mCentre;

    Camera* mDefaultEditorCamera = nullptr;
    Camera* mCurrentCamera = nullptr;

    QSize mCanvasSize = { 1, 1 };

    bool mIsFlipHorizontal = false;
    bool mIsFlipVertical = false;

    const std::vector<float> mZoomLevels{ 0.01f, 0.02f, 0.04f, 0.06f,  0.08f, 0.12f,
                                          0.16f, 0.25f, 0.33f,  0.5f,  0.75f,  1.0f,
                                          1.5f,   2.0f,  3.0f,  4.0f,   5.0f,  6.0f,
                                          8.0f,  16.0f,  32.f,  48.f,   64.f, 96.0f };

    LayerCamera* mCameraLayer = nullptr;
};

#endif // VIEWMANAGER_H
