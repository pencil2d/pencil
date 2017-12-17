/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef CANVASPAINTER_H
#define CANVASPAINTER_H


#include <QObject>
#include <QTransform>
#include <QPainter>
#include <memory>
#include "log.h"


class Object;
class Layer;
class BitmapImage;
class ViewManager;

struct CanvasPainterOptions
{
    bool  bPrevOnionSkin = false;
    bool  bNextOnionSkin = false;
    int   nPrevOnionSkinCount = 3;
    int   nNextOnionSkinCount = 3;
    float fOnionSkinMaxOpacity = 0.5f;
    float fOnionSkinMinOpacity = 0.1f;
    bool  bColorizePrevOnion = false;
    bool  bColorizeNextOnion = false;
    bool  bAntiAlias = false;
    bool  bGrid = false;
    int   nGridSize = 50; /* This is the grid size IN PIXELS. The grid will scale with the image, though */
    bool  bAxis = false;
    bool  bThinLines = false;
    bool  bOutlines = false;
    int   nShowAllLayers = 3;
    bool  bIsOnionAbsolute = false;
    float scaling = 1.0f;
};


class CanvasPainter : public QObject
{
    Q_OBJECT

public:
    explicit CanvasPainter(QObject* parent = 0);
    virtual ~CanvasPainter();

    void setCanvas(QPixmap* canvas);
    void setViewTransform(const QTransform view, const QTransform viewInverse);
    void setOptions(const CanvasPainterOptions& p) { mOptions = p; }
    void setTransformedSelection(QRect selection, QTransform transform);
    void ignoreTransformedSelection();
    QRect getCameraRect();

    void paint(const Object* object, int layer, int frame, QRect rect);
    void renderGrid(QPainter& painter);

private:
    void paintBackground();
    void paintOnionSkin(QPainter& painter);

    void paintCurrentFrame(QPainter& painter);

    void paintBitmapFrame(QPainter&, int layerId, int nFrame, bool colorize = false, bool useLastKeyFrame = true);
    void paintVectorFrame(QPainter&, int layerId, int nFrame, bool colorize = false, bool useLastKeyFrame = true);

    void paintTransformedSelection(QPainter& painter);
    void paintGrid(QPainter& painter);
    void paintCameraBorder(QPainter& painter);
    void paintAxis(QPainter& painter);
    void prescale(BitmapImage* bitmapImage);

private:
    const Object* mObject = nullptr;
    QPixmap* mCanvas = nullptr;
    QTransform mViewTransform;
    QTransform mViewInverse;

    QRect mCameraRect;

    int mCurrentLayerIndex = 0;
    int mFrameNumber = 0;

    QImage mScaledBitmap;

    bool bMultiLayerOnionSkin = false;

    CanvasPainterOptions mOptions;

    // Handle selection transformation
    //
    bool mRenderTransform = false;
    QRect mSelection;
    QTransform mSelectionTransform;

    QLoggingCategory mLog;

};

#endif // CANVASRENDERER_H
