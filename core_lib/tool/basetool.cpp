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

#include "basetool.h"

#include <array>
#include "editor.h"
#include "toolmanager.h"
#include "scribblearea.h"
#include "strokemanager.h"


// ---- shared static variables ---- ( only one instance for all the tools )
ToolPropertyType BaseTool::assistedSettingType; // setting beeing changed
qreal BaseTool::OriginalSettingValue;  // start value (width, feather ..)
bool BaseTool::isAdjusting = false;


QString BaseTool::TypeName( ToolType type )
{
    static std::array< QString, TOOL_TYPE_COUNT > map;
    
    if ( map[ 0 ].isEmpty() )
    {
        map[ PENCIL ] = tr( "Pencil" );
        map[ ERASER ] = tr( "Eraser" );
        map[ SELECT ] = tr( "Select" );
        map[ MOVE ] = tr( "Move" );
        map[ HAND ] = tr( "Hand" );
        map[ SMUDGE ] = tr( "Smudge" );
        map[ PEN ] = tr( "Pen" );
        map[ POLYLINE ] = tr( "Polyline" );
        map[ BUCKET ] = tr( "Bucket" );
        map[ EYEDROPPER ] = tr( "Eyedropper" );
        map[ BRUSH ] = tr( "Brush" );
    }

    return map.at( type );
}

BaseTool::BaseTool( QObject *parent ) : QObject( parent )
{
    m_enabledProperties.insert( WIDTH,          false  );
    m_enabledProperties.insert( FEATHER,        false  );
    m_enabledProperties.insert( USEFEATHER,     false  );
    m_enabledProperties.insert( PRESSURE,       false  );
    m_enabledProperties.insert( INVISIBILITY,   false  );
    m_enabledProperties.insert( PRESERVEALPHA,  false  );
    m_enabledProperties.insert( BEZIER,         false  );
    m_enabledProperties.insert( ANTI_ALIASING,  false  );
    m_enabledProperties.insert( INTERPOLATION,  false  );
}

QCursor BaseTool::cursor()
{
    return Qt::ArrowCursor;
}

void BaseTool::initialize( Editor* editor )
{
    Q_ASSERT( editor );

    if ( editor == NULL )
    {
        qCritical( "ERROR: editor is null!" );
    }
    mEditor = editor;
    mScribbleArea = editor->getScribbleArea();


    Q_ASSERT( mScribbleArea );

    if ( mScribbleArea == NULL )
    {
        qCritical( "ERROR: mScribbleArea is null in editor!" );
    }


    m_pStrokeManager = mEditor->getScribbleArea()->getStrokeManager();

    loadSettings();
}

void BaseTool::mouseDoubleClickEvent( QMouseEvent* event )
{
    mousePressEvent( event );
}

/**
 * @brief precision circular cursor: used for drawing a cursor within scribble area.
 * @return QPixmap
 */
QPixmap BaseTool::canvasCursor() // Todo: only one instance required: make fn static?
{
        Q_ASSERT( mEditor->getScribbleArea() );

        float scalingFac = mEditor->view()->scaling();
        propWidth = properties.width * scalingFac;
        propFeather = properties.feather * scalingFac;
        cursorWidth = propWidth + 0.5 * propFeather;

        if ( cursorWidth < 1 ) { cursorWidth = 1; }
        radius = cursorWidth / 2;
        xyA = 1 + propFeather / 2;
        xyB = 1 + propFeather / 8;
        whA = qMax( 0, propWidth - xyA - 1 );
        whB = qMax( 0, cursorWidth - propFeather / 4 - 2 );
        cursorPixmap = QPixmap( cursorWidth, cursorWidth );
        if ( !cursorPixmap.isNull() )
        {
            cursorPixmap.fill( QColor( 255, 255, 255, 0 ) );
            QPainter cursorPainter( &cursorPixmap );
            cursorPen = cursorPainter.pen();
            cursorPainter.setRenderHint(QPainter::HighQualityAntialiasing);

            // Draw cross in center
            cursorPen.setStyle( Qt::SolidLine );
            cursorPen.setColor( QColor( 0, 0, 0, 127 ) );
            cursorPainter.setPen(cursorPen);
            cursorPainter.drawLine( QPointF( radius - 2, radius ), QPointF( radius + 2, radius ) );
            cursorPainter.drawLine( QPointF( radius, radius - 2 ), QPointF( radius, radius + 2 ) );

            // Draw outer circle
            cursorPen.setStyle( Qt::DotLine );
            cursorPen.setColor( QColor( 0, 0, 0, 255 ) );
            cursorPainter.setPen(cursorPen);
            cursorPainter.drawEllipse( QRectF( xyB, xyB, whB, whB ) );
            cursorPen.setDashOffset( 4 );
            cursorPen.setColor( QColor( 255, 255, 255, 255 ) );
            cursorPainter.setPen(cursorPen);
            cursorPainter.drawEllipse( QRectF( xyB, xyB, whB, whB ) );

            // Draw inner circle
            cursorPen.setStyle( Qt::DotLine );
            cursorPen.setColor( QColor( 0, 0, 0, 255 ) );
            cursorPainter.setPen(cursorPen);
            cursorPainter.drawEllipse( QRectF( xyA, xyA, whA, whA ) );
            cursorPen.setDashOffset( 4 );
            cursorPen.setColor( QColor( 255, 255, 255, 255 ) );
            cursorPainter.setPen(cursorPen);
            cursorPainter.drawEllipse( QRectF( xyA, xyA, whA, whA ) );

            cursorPainter.end();
        }
        return cursorPixmap;
}

/**
 * @brief precision circular cursor: used for drawing stroke size while adjusting
 * @return QPixmap
 */
QPixmap BaseTool::quickSizeCursor() // Todo: only one instance required: make fn static?
{
    Q_ASSERT( mEditor->getScribbleArea() );

    float scalingFac = mEditor->view()->scaling();
    propWidth = properties.width * scalingFac;
    propFeather = properties.feather * scalingFac;
    cursorWidth = propWidth + 0.5 * propFeather;

    if ( cursorWidth < 1 ) { cursorWidth = 1; }
    radius = cursorWidth / 2;
    xyA = 1 + propFeather / 2;
    xyB = 1 + propFeather / 8;
    whA = qMax( 0, propWidth - xyA - 1 );
    whB = qMax( 0, cursorWidth - propFeather / 4 - 2 );
    cursorPixmap = QPixmap( cursorWidth, cursorWidth );
    if ( !cursorPixmap.isNull() )
    {
        cursorPixmap.fill( QColor( 255, 255, 255, 0 ) );
        QPainter cursorPainter( &cursorPixmap );
        cursorPainter.setPen( QColor( 0, 0, 0, 255 ) );
        cursorPainter.drawLine( QPointF( radius - 2, radius ), QPointF( radius + 2, radius ) );
        cursorPainter.drawLine( QPointF( radius, radius - 2 ), QPointF( radius, radius + 2 ) );
        cursorPainter.setRenderHints( QPainter::Antialiasing, true );
        cursorPainter.setPen( QColor( 0, 0, 0, 0 ) );
        cursorPainter.setBrush( QColor( 0, 255, 127, 64 ) );
        cursorPainter.setCompositionMode( QPainter::CompositionMode_Exclusion );
        cursorPainter.drawEllipse( QRectF( xyB, xyB, whB, whB ) ); // outside circle
        cursorPainter.setBrush( QColor( 255, 64, 0, 255 ) );
        cursorPainter.drawEllipse( QRectF( xyA, xyA, whA, whA ) ); // inside circle
        cursorPainter.end();
    }
    return cursorPixmap;
}

void BaseTool::startAdjusting( ToolPropertyType argSettingType, qreal argStep )
{
    isAdjusting = true;
    assistedSettingType = argSettingType;
    mAdjustmentStep = argStep;
    if ( argSettingType == WIDTH )
    {
        OriginalSettingValue = properties.width;
    }
    else if ( argSettingType == FEATHER )
    {
        OriginalSettingValue = properties.feather;
    }
    mEditor->getScribbleArea()->updateCanvasCursor();
}

void BaseTool::stopAdjusting()
{
    isAdjusting = false;
    mAdjustmentStep = 0;
    OriginalSettingValue = 0;
    mEditor->getScribbleArea()->updateCanvasCursor();
}

void BaseTool::adjustCursor( qreal argOffsetX, ToolPropertyType type ) //offsetx x-lastx ...
{
    qreal inc = pow( OriginalSettingValue * 100, 0.5 );
    qreal newValue = inc + argOffsetX;
    int max = type == FEATHER ? 64 : 200;
    int min = type == FEATHER ? 2 : 1;

    if ( newValue < 0 )
    {
        newValue = 0;
    }

    newValue = pow( newValue, 2 ) / 100;
    if ( mAdjustmentStep > 0 )
    {
        int tempValue = ( int )( newValue / mAdjustmentStep ); // + 0.5 ?
        newValue = tempValue * mAdjustmentStep;
    }
    if ( newValue < min ) // can be optimized for size: min(200,max(0.2,newValueX))
    {
        newValue = min;
    }
    else if ( newValue > max )
    {
        newValue = max;
    }

    switch (type){
        case FEATHER:
            if ( ( this->type() == BRUSH ) || ( this->type() == ERASER ) || ( this->type() == SMUDGE ) )
            {
                mEditor->tools()->setFeather( newValue );
            }
            break;
        case WIDTH:
            mEditor->tools()->setWidth( newValue );
            break;
        default:
            break;
    };
}

void BaseTool::adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice )
{
    Q_UNUSED( pressure );
    Q_UNUSED( mouseDevice );
}

QPointF BaseTool::getCurrentPixel()
{
    return m_pStrokeManager->getCurrentPixel();
}

QPointF BaseTool::getCurrentPoint()
{
    return mEditor->view()->mapScreenToCanvas( getCurrentPixel() );
}

QPointF BaseTool::getLastPixel()
{
    return m_pStrokeManager->getLastPixel();
}

QPointF BaseTool::getLastPoint()
{
    return mEditor->view()->mapScreenToCanvas( getLastPixel() );
}

QPointF BaseTool::getLastPressPixel()
{
    return m_pStrokeManager->getLastPressPixel();
}

QPointF BaseTool::getLastPressPoint()
{
    return mEditor->view()->mapScreenToCanvas( getLastPressPixel() );
}

void BaseTool::setWidth( const qreal width )
{
    properties.width = width;
}

void BaseTool::setFeather( const qreal feather )
{
    properties.feather = feather;
}

void BaseTool::setUseFeather( const bool usingFeather )
{
    properties.useFeather = usingFeather;
}

void BaseTool::setInvisibility( const bool invisibility )
{
    properties.invisibility = invisibility;
}

void BaseTool::setBezier( const bool _bezier_state )
{
    properties.bezier_state = _bezier_state;
}

void BaseTool::setPressure( const bool pressure )
{
    properties.pressure = pressure;
}

void BaseTool::setPreserveAlpha( const bool preserveAlpha )
{
    properties.preserveAlpha = preserveAlpha;
}

void BaseTool::setVectorMergeEnabled(const bool vectorMergeEnabled)
{
    properties.vectorMergeEnabled = vectorMergeEnabled;
}

void BaseTool::setAA(const int useAA)
{
    properties.useAA = useAA;
}

void BaseTool::setInpolLevel(const int level)
{
    properties.inpolLevel = level;
}

void BaseTool::setTolerance(const int tolerance)
{
    properties.tolerance = tolerance;
}

void BaseTool::setUseFillContour(const bool useFillContour)
{
    properties.useFillContour = useFillContour;
}
