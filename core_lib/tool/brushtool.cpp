/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "brushtool.h"

#include <cmath>
#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "layervector.h"
#include "layer.h"
#include "editor.h"
#include "pencilsettings.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "scribblearea.h"
#include "blitrect.h"



BrushTool::BrushTool( QObject *parent ) :
StrokeTool( parent )
{
}

ToolType BrushTool::type()
{
    return BRUSH;
}

void BrushTool::loadSettings()
{
    m_enabledProperties[WIDTH] = true;
    m_enabledProperties[FEATHER] = true;
    m_enabledProperties[USEFEATHER] = true;
    m_enabledProperties[PRESSURE] = true;
    m_enabledProperties[INVISIBILITY] = true;
    m_enabledProperties[INTERPOLATION] = true;
    m_enabledProperties[ANTI_ALIASING] = true;

    QSettings settings( PENCIL2D, PENCIL2D );

    properties.width = settings.value( "brushWidth" ).toDouble();
    properties.feather = settings.value( "brushFeather", 15.0 ).toDouble();
    properties.useFeather = settings.value( "brushUseFeather", true ).toBool();
    properties.pressure = settings.value( "brushPressure", false ).toBool();
    properties.invisibility = settings.value("brushInvisibility", true).toBool();
    properties.preserveAlpha = OFF;
    properties.inpolLevel = 0;
    properties.useAA = 1;

    if (properties.useFeather == true) {
        properties.useAA = -1;
    }

    // First run
    //
    if ( properties.width <= 0 )
    {
        setWidth(15);
    }

    if ( std::isnan( properties.feather ) )
    {
        setFeather( 15 );
    }
}

void BrushTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("brushWidth", width);
    settings.sync();
}

void BrushTool::setUseFeather( const bool usingFeather )
{
    // Set current property
    properties.useFeather = usingFeather;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("brushUseFeather", usingFeather);
    settings.sync();
}

void BrushTool::setFeather( const qreal feather )
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("brushFeather", feather);
    settings.sync();
}

void BrushTool::setInvisibility( const bool invisibility )
{
    // force value
    properties.invisibility = invisibility;

    QSettings settings ( PENCIL2D, PENCIL2D );
    settings.setValue("brushInvisibility",invisibility);
    settings.sync();
}

void BrushTool::setPressure( const bool pressure )
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("brushPressure", pressure);
    settings.sync();
}

void BrushTool::setInpolLevel(const int level)
{
    properties.inpolLevel = level;

    QSettings settings( PENCIL2D, PENCIL2D);
    settings.setValue("lineInpol", level);
    settings.sync();
}

void BrushTool::setAA( const int AA )
{
    // Set current property
    properties.useAA = AA;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("brushAA", AA);
    settings.sync();
}

QCursor BrushTool::cursor()
{
    if ( mEditor->preference()->isOn( SETTING::TOOL_CURSOR ) )
    {
        return QCursor( QPixmap( ":icons/brush.png" ), 0, 13 );
    }
    return Qt::CrossCursor;
}

void BrushTool::adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice )
{
//    Layer* layer = mEditor->layers()->currentLayer();

//    // In Bitmap mode, the brush tool pressure only handles opacity while the Pen tool
//    // only handles size. Pencil tool handles both.

//    QColor currentColor = mEditor->color()->frontColor();
//    currentPressuredColor = currentColor;

//    if ( layer->type() == Layer::BITMAP && properties.pressure && !mouseDevice )
//    {
//        currentPressuredColor.setAlphaF( currentColor.alphaF() * pressure * pressure );
//    }

//    mCurrentWidth = properties.width;

    if ( properties.pressure && !mouseDevice )
    {
        mCurrentPressure = pressure;
    }
    else
    {
        mCurrentPressure = 1.0;
    }
}

void BrushTool::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton )
    {
        mEditor->backup( typeName() );
        mScribbleArea->setAllDirty();
    }

    mMouseDownPoint = getCurrentPoint();

    mLastBrushPoint = getCurrentPoint();
    startStroke();

    if ( !mEditor->preference()->isOn(SETTING::INVISIBLE_LINES) )
    {
        mScribbleArea->toggleThinLines();
    }

}

void BrushTool::mouseReleaseEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton )
    {
        Layer* layer = mEditor->layers()->currentLayer();
        if ( mScribbleArea->isLayerPaintable() )
        {
            qreal distance = QLineF( getCurrentPoint(), mMouseDownPoint ).length();
            if (distance < 1)
            {
                paintAt(mMouseDownPoint);
            }
            else
            {
                drawStroke();
            }
        }

        if ( layer->type() == Layer::BITMAP ) {
            paintBitmapStroke();
        }
        else if (layer->type() == Layer::VECTOR )
        {
            paintVectorStroke();
        }
    }
    endStroke();
}

void BrushTool::mouseMoveEvent( QMouseEvent *event )
{
    if ( mScribbleArea->isLayerPaintable() )
    {
        if ( event->buttons() & Qt::LeftButton )
        {
            drawStroke();
            if (properties.inpolLevel != m_pStrokeManager->getInpolLevel()) {
                m_pStrokeManager->setInpolLevel(properties.inpolLevel);
            }
        }
    }
}

// draw a single paint dab at the given location
void BrushTool::paintAt( QPointF point )
{
    qDebug() << "Made a single dab at " << point;
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer->type() == Layer::BITMAP )
    {
        qreal opacity = 1.0;
        mCurrentWidth = properties.width;
        qreal brushWidth = mCurrentWidth;

        BlitRect rect;

        rect.extend( point.toPoint() );
        mScribbleArea->drawBrush( QPoint( qRound(point.x() ), qRound(point.y() )),
                                  brushWidth,
                                  properties.feather,
                                  mEditor->color()->frontColor(),
                                  opacity,
                                  properties.useFeather,
                                  properties.useAA );

        int rad = qRound( brushWidth ) / 2 + 2;
        mScribbleArea->refreshBitmap( rect, rad );
    }
}

void BrushTool::drawStroke()
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
        if (properties.pressure == true) {
            opacity = mCurrentPressure / 2;
        }

        mCurrentWidth = properties.width;
        qreal brushWidth = mCurrentWidth;

        qreal brushStep = (0.5 * brushWidth);
        brushStep = qMax( 1.0, brushStep );

        BlitRect rect;

        QPointF a = mLastBrushPoint;
        QPointF b = getCurrentPoint();

        qreal distance = 4 * QLineF( b, a ).length();
        int steps = qRound( distance / brushStep);

        for ( int i = 0; i < steps; i++ )
        {
            QPointF point = mLastBrushPoint + ( i + 1 ) * brushStep * ( getCurrentPoint() - mLastBrushPoint ) / distance;

            rect.extend( point.toPoint() );
            mScribbleArea->drawBrush( QPoint( qRound(point.x() ),qRound(point.y() )),
                                      brushWidth,
                                      properties.feather,
                                      mEditor->color()->frontColor(),
                                      opacity,
                                      properties.useFeather,
                                      properties.useAA );
            if ( i == ( steps - 1 ) )
            {
                mLastBrushPoint = getCurrentPoint();
            }
        }

        int rad = qRound( brushWidth  / 2 + 2);

        mScribbleArea->paintBitmapBufferRect( rect );
        mScribbleArea->refreshBitmap( rect, rad );

          // Line visualizer
          // for debugging
//        QPainterPath tempPath;

//        QPointF mappedMousePos = mEditor->view()->mapScreenToCanvas(m_pStrokeManager->getMousePos());
//        tempPath.moveTo(getCurrentPoint());
//        tempPath.lineTo(mappedMousePos);

//        QPen pen( Qt::black,
//                   1,
//                   Qt::SolidLine,
//                   Qt::RoundCap,
//                   Qt::RoundJoin );
//        mScribbleArea->drawPolyline(tempPath, pen, true);

    }
    else if ( layer->type() == Layer::VECTOR )
    {
        qreal brushWidth = 0;
        if (properties.pressure ) {
            brushWidth = properties.width * mCurrentPressure;
        }
        else {
            brushWidth = properties.width;
        }

        int rad = qRound( ( brushWidth / 2 + 2 ) * mEditor->view()->scaling() );

        QPen pen( mEditor->color()->frontColor(),
                  brushWidth * mEditor->view()->scaling(),
                  Qt::SolidLine,
                  Qt::RoundCap,
                  Qt::RoundJoin );

        if ( p.size() == 4 )
        {
            QPainterPath path( p[ 0 ] );
            path.cubicTo( p[ 1 ],
                          p[ 2 ],
                          p[ 3 ] );

            mScribbleArea->drawPath( path, pen, Qt::NoBrush, QPainter::CompositionMode_Source );
            mScribbleArea->refreshVector( path.boundingRect().toRect(), rad );
        }
    }
}

void BrushTool::paintBitmapStroke()
{
    mScribbleArea->paintBitmapBuffer();
    mScribbleArea->setAllDirty();
    mScribbleArea->clearBitmapBuffer();
}

// This function uses the points from DrawStroke
// and turns them into vector lines.
void BrushTool::paintVectorStroke()
{
    Layer* layer = mEditor->layers()->currentLayer();

    if ( layer->type() == Layer::VECTOR && mStrokePoints.size() > -1 )
    {

        // Clear the temporary pixel path
        mScribbleArea->clearBitmapBuffer();
        qreal tol = mScribbleArea->getCurveSmoothing() / mEditor->view()->scaling();

        BezierCurve curve( mStrokePoints, mStrokePressures, tol );
                    curve.setWidth( properties.width );
                    curve.setFeather( properties.feather );
                    curve.setFilled( false );
                    curve.setInvisibility( properties.invisibility );
                    curve.setVariableWidth( properties.pressure );
                    curve.setColourNumber( mEditor->color()->frontColorNumber() );

        auto pLayerVector = static_cast< LayerVector* >( layer );
        VectorImage* vectorImage = pLayerVector->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
        vectorImage->addCurve( curve, mEditor->view()->scaling(), false );

        if (vectorImage->isAnyCurveSelected() || mScribbleArea->somethingSelected) {
            mScribbleArea->deselectAll();
        }

        vectorImage->setSelected(vectorImage->getLastCurveNumber(), true);
        mScribbleArea->somethingSelected = true;

        mScribbleArea->setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
        mScribbleArea->setAllDirty();
    }
}
