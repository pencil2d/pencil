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
#include "erasertool.h"

#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "editor.h"
#include "blitrect.h"
#include "scribblearea.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "layervector.h"
#include "vectorimage.h"


EraserTool::EraserTool( QObject *parent ) 
	: StrokeTool( parent )
{
}

ToolType EraserTool::type()
{
    return ERASER;
}

void EraserTool::loadSettings()
{
    m_enabledProperties[WIDTH] = true;
    m_enabledProperties[FEATHER] = true;
    m_enabledProperties[PRESSURE] = true;
    m_enabledProperties[STABILIZATION] = true;


    QSettings settings( PENCIL2D, PENCIL2D );

    properties.width = settings.value( "eraserWidth" ).toDouble();
    properties.feather = settings.value( "eraserFeather" ).toDouble();

    properties.pressure = settings.value( "eraserPressure" ).toBool();
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;
    properties.stabilizerLevel = settings.value("stabilizerLevel").toInt();

    // First run
    if ( properties.width <= 0 )
    {
        setWidth(25);
        setFeather(50);
        setPressure(true);
    }
}

void EraserTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("eraserWidth", width);
    settings.sync();
}

void EraserTool::setFeather( const qreal feather )
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("eraserFeather", feather);
    settings.sync();
}

void EraserTool::setPressure( const bool pressure )
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("eraserPressure", pressure);
    settings.sync();
}

void EraserTool::setStabilizerLevel(const int level)
{
    properties.stabilizerLevel = level;

    QSettings settings( PENCIL2D, PENCIL2D);
    settings.setValue("stabilizerLevel", level);
    settings.sync();
}


QCursor EraserTool::cursor()
{
    return Qt::CrossCursor;
}

void EraserTool::adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice )
{
    if ( properties.pressure && !mouseDevice )
    {
        mCurrentPressure = pressure;
    }
    else
    {
        mCurrentPressure = 1.0;
    }
}

void EraserTool::tabletPressEvent(QTabletEvent *)
{
    mScribbleArea->setAllDirty();

    startStroke();
    mLastBrushPoint = getCurrentPoint();
    mMouseDownPoint = getCurrentPoint();
}

void EraserTool::tabletMoveEvent(QTabletEvent *)
{
    updateStrokes();
    if (properties.stabilizerLevel != m_pStrokeManager->getStabilizerLevel())
        m_pStrokeManager->setStabilizerLevel(properties.stabilizerLevel);
}

void EraserTool::tabletReleaseEvent(QTabletEvent *)
{
    mEditor->backup(typeName());

    qreal distance = QLineF( getCurrentPoint(), mMouseDownPoint ).length();
    if (distance < 1)
    {
        paintAt(mMouseDownPoint);
    }
    else
    {
        drawStroke();
    }
    removeVectorPaint();
    endStroke();
}

void EraserTool::mousePressEvent( QMouseEvent *)
{
    mScribbleArea->setAllDirty();

    startStroke();
    mLastBrushPoint = getCurrentPoint();
    mMouseDownPoint = getCurrentPoint();
}

void EraserTool::mouseReleaseEvent(QMouseEvent *)
{
    mEditor->backup(typeName());

    qreal distance = QLineF( getCurrentPoint(), mMouseDownPoint ).length();
    if (distance < 1)
    {
        paintAt(mMouseDownPoint);
    }
    else
    {
        drawStroke();
    }
    removeVectorPaint();
    endStroke();
}

void EraserTool::mouseMoveEvent(QMouseEvent *)
{
    updateStrokes();
    if (properties.stabilizerLevel != m_pStrokeManager->getStabilizerLevel())
        m_pStrokeManager->setStabilizerLevel(properties.stabilizerLevel);
}

// draw a single paint dab at the given location
void EraserTool::paintAt( QPointF point )
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer->type() == Layer::BITMAP )
    {
        qreal opacity = 1.0;
        mCurrentWidth = properties.width;
        if (properties.pressure == true)
        {
            opacity = m_pStrokeManager->getPressure();
            mCurrentWidth = (mCurrentWidth + ( m_pStrokeManager->getPressure() * mCurrentWidth)) * 0.5;
        }

        qreal brushWidth = mCurrentWidth;

        BlitRect rect;

        rect.extend( point.toPoint() );
        mScribbleArea->drawBrush( point,
                                  brushWidth,
                                  properties.feather,
                                  QColor(255, 255, 255, 255),
                                  opacity,
                                  properties.useFeather,
                                  properties.useAA );

        int rad = qRound( brushWidth ) / 2 + 2;

        //continously update buffer to update stroke behind grid.
        mScribbleArea->paintBitmapBufferRect(rect);

        mScribbleArea->refreshBitmap( rect, rad );
    }
}

void EraserTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if ( layer->type() == Layer::BITMAP )
    {
        for ( int i = 0; i < p.size(); i++ )
        {
            p[ i ] = mEditor->view()->mapScreenToCanvas( p[ i ] );
        }

        qreal opacity = 1.0;
        mCurrentWidth = properties.width;
        if (properties.pressure)
        {
            opacity = m_pStrokeManager->getPressure();
            mCurrentWidth = (mCurrentWidth + ( m_pStrokeManager->getPressure() * mCurrentWidth)) * 0.5;
        }

        qreal brushWidth = mCurrentWidth;
        qreal brushStep = (0.5 * brushWidth) - ((properties.feather/100.0) * brushWidth * 0.5);
        brushStep = qMax( 1.0, brushStep );

        BlitRect rect;

        QPointF a = mLastBrushPoint;
        QPointF b = getCurrentPoint();

        qreal distance = 4 * QLineF( b, a ).length();
        int steps = qRound( distance ) / brushStep;

        for ( int i = 0; i < steps; i++ )
        {
            QPointF point = mLastBrushPoint + ( i + 1 ) * ( brushStep )* ( b - mLastBrushPoint ) / distance;
            rect.extend( point.toPoint() );
            mScribbleArea->drawBrush( point,
                                      brushWidth,
                                      properties.feather,
                                      QColor(255, 255, 255, 255),
                                      opacity,
                                      properties.useFeather,
                                      properties.useAA );

            if ( i == ( steps - 1 ) )
            {
                mLastBrushPoint = point;
            }
        }

        int rad = qRound( brushWidth ) / 2 + 2;

        //continously update buffer to update stroke behind grid.
        mScribbleArea->paintBitmapBufferRect(rect);

        mScribbleArea->refreshBitmap( rect, rad );
    }
    else if ( layer->type() == Layer::VECTOR )
    {
        mCurrentWidth = properties.width;
        if (properties.pressure)
        {
            mCurrentWidth = (mCurrentWidth + ( m_pStrokeManager->getPressure() * mCurrentWidth)) * 0.5;
        }
        qreal brushWidth = mCurrentWidth;

        QPen pen( Qt::white, brushWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
        int rad = qRound(brushWidth) / 2 + 2;

        if ( p.size() == 4 )
        {
            QPainterPath path( p[ 0 ] );
            path.cubicTo( p[ 1 ],
                          p[ 2 ],
                          p[ 3 ] );
            qDebug() << path;
            mScribbleArea->drawPath( path, pen, Qt::NoBrush, QPainter::CompositionMode_Source );
            mScribbleArea->refreshVector( path.boundingRect().toRect(), rad );
        }
    }
}

void EraserTool::removeVectorPaint()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer->type() == Layer::BITMAP )
    {
        mScribbleArea->paintBitmapBuffer();
        mScribbleArea->setAllDirty();
        mScribbleArea->clearBitmapBuffer();
    }
    else if ( layer->type() == Layer::VECTOR )
    {
        VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
        // Clear the area containing the last point
        //vectorImage->removeArea(lastPoint);
        // Clear the temporary pixel path
        mScribbleArea->clearBitmapBuffer();
        vectorImage->deleteSelectedPoints();
        //update();
        mScribbleArea->setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
        mScribbleArea->setAllDirty();
    }
}

void EraserTool::updateStrokes()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR )
    {
        drawStroke();
    }

    if ( layer->type() == Layer::VECTOR )
    {
        qreal radius = properties.width / 2;
        QList<VertexRef> nearbyVertices = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )
            ->getVerticesCloseTo( getCurrentPoint(), radius );
        for ( int i = 0; i < nearbyVertices.size(); i++ )
        {
            ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->setSelected( nearbyVertices.at( i ), true );
        }
        //update();
        mScribbleArea->setAllDirty();
    }
}
