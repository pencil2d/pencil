/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef CANVASRENDERER_H
#define CANVASRENDERER_H


#include <QObject>
#include <QTransform>
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

    void paint( Object* object, int layer, int frame );

private:
    void paintBackground( QPainter& painter );
    void paintOnionSkin( QPainter& painter );
    void paintCurrentFrame( QPainter& painter );


    void paintBitmapFrame( QPainter&, Layer* layer, int nFrame, bool colorize = false );
    void paintVectorFrame( QPainter&, Layer* layer, int nFrame, bool colorize = false );

private:
    QPixmap* mCanvas = nullptr;
    Object* mObject = nullptr;
    QTransform mViewTransform;

    int mLayerIndex = 0;
    int mFrameNumber = 0;

    bool bMultiLayerOnionSkin = false;
    
    RenderOptions mOptions;

    QLoggingCategory mLog;
};

#endif // CANVASRENDERER_H
