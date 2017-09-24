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

#ifndef CANVASRENDERER_H
#define CANVASRENDERER_H


#include <QObject>
#include <QTransform>
#include <QPainter>
#include <memory>
#include "log.h"


class Object;
class Layer;


struct RenderOptions
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
};


class CanvasRenderer : public QObject
{
    Q_OBJECT

public:
    explicit CanvasRenderer( QObject* parent = 0 );
    virtual ~CanvasRenderer();

    void setCanvas( QPixmap* canvas );
    void setViewTransform( QTransform viewTransform );
    void setOptions( RenderOptions p ) { mOptions = p; }
    void setTransformedSelection( QRect selection, QTransform transform );
    void ignoreTransformedSelection();
    QRect getCameraRect();

    void paint( Object* object, int layer, int frame, QRect rect );
    void renderGrid(QPainter& painter);

private:
    void paintBackground();
    void paintOnionSkin( QPainter& painter );
    void paintCurrentFrame( QPainter& painter );

    void paintBitmapFrame( QPainter&, int layerId, int nFrame, bool colorize = false , bool useLastKeyFrame = true );
    void paintVectorFrame(QPainter&, int layerId, int nFrame, bool colorize = false , bool useLastKeyFrame = true );

    void paintTransformedSelection( QPainter& painter );
    void paintGrid( QPainter& painter );
    void paintCameraBorder(QPainter& painter);
    void paintAxis( QPainter& painter );

private:
    QPixmap* mCanvas = nullptr;
    Object* mObject = nullptr;
    QTransform mViewTransform;
    QRect mCameraRect;

    int mCurrentLayerIndex = 0;
    int mFrameNumber = 0;

    bool bMultiLayerOnionSkin = false;
    
    RenderOptions mOptions;

    // Handle selection transformation
    //
    bool mRenderTransform = false;
    QRect mSelection;
    QTransform mSelectionTransform;

    QLoggingCategory mLog;

};

#endif // CANVASRENDERER_H
