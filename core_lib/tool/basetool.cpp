#include "editor.h"
#include "basetool.h"
#include "toolmanager.h"
#include "scribblearea.h"
#include "strokemanager.h"


// ---- shared static variables ---- ( only one instance for all the tools )
ToolPropertyType BaseTool::assistedSettingType; // setting beeing changed
qreal BaseTool::OriginalSettingValue;  // start value (width, feather ..)
bool BaseTool::isAdjusting = false;

QString BaseTool::TypeName( ToolType type )
{
    static QMap<ToolType, QString>* map = NULL;

    if ( map == NULL )
    {
        map = new QMap<ToolType, QString>();
        map->insert( PENCIL, "Pencil" );
        map->insert( ERASER, "Eraser" );
        map->insert( SELECT, "Select" );
        map->insert( MOVE, "Move" );
        map->insert( HAND, "Hand" );
        map->insert( SMUDGE, "Smudge" );
        map->insert( PEN, "Pen" );
        map->insert( POLYLINE, "Polyline" );
        map->insert( BUCKET, "Bucket" );
        map->insert( EYEDROPPER, "Eyedropper" );
        map->insert( BRUSH, "Brush" );
    }

    return map->value( type );
}

BaseTool::BaseTool( QObject *parent ) : QObject( parent )
{
    m_enabledProperties.insert( WIDTH,          false  );
    m_enabledProperties.insert( FEATHER,        false  );
    m_enabledProperties.insert( OPACITY,        false  );
    m_enabledProperties.insert( COLOURNUMBER,   false  );
    m_enabledProperties.insert( PRESSURE,       false  );
    m_enabledProperties.insert( INVISIBILITY,   false  );
    m_enabledProperties.insert( PRESERVEALPHA,  false  );
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
    m_pStrokeManager = mEditor->getScribbleArea()->getStrokeManager();

    loadSettings();
}

/*
void BaseTool::mousePressEvent( QMouseEvent* )
{
}

void BaseTool::mouseMoveEvent( QMouseEvent* )
{
}

void BaseTool::mouseReleaseEvent( QMouseEvent* )
{
}
*/

void BaseTool::mouseDoubleClickEvent( QMouseEvent *event )
{
    mousePressEvent( event );
}

QCursor BaseTool::circleCursors() // Todo: only one instance required: make fn static?
{
    Q_ASSERT( mEditor->getScribbleArea() );

    qreal zoomFactor = editor()->view()->scaling(); //scale factor

    //qDebug() << "--->" << zoomFactor;
    qreal propWidth = properties.width * zoomFactor;
    qreal propFeather = properties.feather * zoomFactor;
    qreal width = propWidth + 0.5 * propFeather;

    if ( width < 1 ) { width = 1; }
    qreal radius = width / 2;
    qreal xyA = 1 + propFeather / 2;
    qreal xyB = 1 + propFeather / 8;
    qreal whA = qMax( 0.0, propWidth - xyA - 1 );
    qreal whB = qMax( 0.0, width - propFeather / 4 - 2 );
    QPixmap pixmap( width, width );
    if ( !pixmap.isNull() )
    {
        pixmap.fill( QColor( 255, 255, 255, 0 ) );
        QPainter painter( &pixmap );
        painter.setPen( QColor( 0, 0, 0, 255 ) );
        painter.drawLine( QPointF( radius - 2, radius ), QPointF( radius + 2, radius ) );
        painter.drawLine( QPointF( radius, radius - 2 ), QPointF( radius, radius + 2 ) );
        painter.setRenderHints( QPainter::Antialiasing, true );
        painter.setPen( QColor( 0, 0, 0, 0 ) );
        painter.setBrush( QColor( 0, 255, 127, 64 ) );
        painter.setCompositionMode( QPainter::CompositionMode_Exclusion );
        painter.drawEllipse( QRectF( xyB, xyB, whB, whB ) ); // outside circle
        painter.setBrush( QColor( 255, 64, 0, 255 ) );
        painter.drawEllipse( QRectF( xyA, xyA, whA, whA ) ); // inside circle
        painter.end();
    }
    return QCursor( pixmap );
}

void BaseTool::startAdjusting( ToolPropertyType argSettingType, qreal argStep )
{
    isAdjusting = true;
    assistedSettingType = argSettingType;
    adjustmentStep = argStep;
    if ( argSettingType == WIDTH )
    {
        OriginalSettingValue = properties.width;
    }
    else if ( argSettingType == FEATHER )
    {
        OriginalSettingValue = properties.feather;
    }
    mEditor->getScribbleArea()->setCursor( cursor() ); // cursor() changes in brushtool, erasertool, ...
}

void BaseTool::stopAdjusting()
{
    isAdjusting = false;
    adjustmentStep = 0;
    OriginalSettingValue = 0;
    mEditor->getScribbleArea()->setCursor( cursor() );
}

void BaseTool::adjustCursor( qreal argOffsetX, qreal argOffsetY ) //offsetx x-lastx ...
{
    qreal incx = pow( OriginalSettingValue * 100, 0.5 );
    qreal incy = incx;
    qreal newValueX = incx + argOffsetX;
    qreal newValueY = incy + argOffsetY;

    if ( newValueX < 0 )
    {
        newValueX = 0;
    }
    if ( newValueY < 0 )
    {
        newValueY = 0;
    }

    newValueX = pow( newValueX, 2 ) / 100;
    newValueY = pow( newValueY, 2 ) / 100;

    if ( adjustmentStep > 0 )
    {
        int tempValueX = ( int )( newValueX / adjustmentStep ); // + 0.5 ?
        int tempValueY = ( int )( newValueY / adjustmentStep ); // + 0.5 ?
        newValueX = tempValueX * adjustmentStep;
        newValueY = tempValueY * adjustmentStep;
    }

    if ( newValueX < 1 ) // can be optimized for size: min(200,max(0.2,newValueX))
    {
        newValueX = 1;
    }
    else if ( newValueX > 200 )
    {
        newValueX = 200;
    }

    if ( newValueY < 1 ) // can be optimized for size: min(200,max(0.2,newValueX))
    {
        newValueY = 1;
    }
    else if ( newValueY > 200 )
    {
        newValueY = 200;
    }

    mEditor->tools()->setWidth( newValueX );

    if ( ( this->type() == BRUSH ) || ( this->type() == ERASER ) || ( this->type() == SMUDGE ) )
    {
        mEditor->tools()->setFeather( newValueY );
    }
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

void BaseTool::setInvisibility( const qreal invisibility )
{
    properties.invisibility = invisibility;
}

void BaseTool::setPressure( const bool pressure )
{
    properties.pressure = pressure;
}

void BaseTool::setPreserveAlpha( const bool preserveAlpha )
{
    properties.preserveAlpha = preserveAlpha;
}
