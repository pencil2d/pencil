#include "polylinetool.h"

#include "editor.h"
#include "scribblearea.h"

#include "strokemanager.h"
#include "layermanager.h"

#include "layervector.h"
#include "layerbitmap.h"


PolylineTool::PolylineTool( QObject *parent ) :
BaseTool( parent )
{
}

ToolType PolylineTool::type()
{
    return POLYLINE;
}

void PolylineTool::loadSettings()
{
    m_enabledProperties[WIDTH] = true;
    m_enabledProperties[BEZIER] = true;

    QSettings settings( PENCIL2D, PENCIL2D );

    properties.width = settings.value( "polyLineWidth" ).toDouble();
    properties.feather = -1;
    properties.pressure = 0;
    properties.invisibility = OFF;
    properties.preserveAlpha = OFF;

    // First run
    if ( properties.width <= 0 )
    {
        setWidth(1.5);
    }
}

void PolylineTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("polyLineWidth", width);
    settings.sync();
}

void PolylineTool::setFeather( const qreal feather )
{
    properties.feather = -1;
}

QCursor PolylineTool::cursor() //Not working this one, any guru to fix it?
{
    if ( isAdjusting ) { // being dynamically resized
        qDebug() << "adjusting";
        return QCursor( this->circleCursors() ); // two circles cursor
    }
    return Qt::CrossCursor;
}

void PolylineTool::clear()
{
    mPoints.clear();
}

void PolylineTool::mousePressEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();

    if ( event->button() == Qt::LeftButton )
    {
        if ( layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR )
        {
            if ( mPoints.size() == 0 )
            {
                mEditor->backup( tr( "Line" ) );
            }

            if ( layer->type() == Layer::VECTOR )
            {
                ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->deselectAll();
                if ( mScribbleArea->makeInvisible() && !mEditor->preference()->isOn(SETTING::INVISIBLE_LINES) )
                {
                    mScribbleArea->toggleThinLines();
                }
            }
            mPoints << getCurrentPoint();
            mScribbleArea->setAllDirty();
        }
    }
}

void PolylineTool::mouseReleaseEvent( QMouseEvent *event )
{
    Q_UNUSED( event );
}

void PolylineTool::mouseMoveEvent( QMouseEvent *event )
{
    Q_UNUSED( event );
    Layer* layer = mEditor->layers()->currentLayer();

    if ( layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR )
    {
        mScribbleArea->drawPolyline( mPoints, getCurrentPoint() );
    }
}

void PolylineTool::mouseDoubleClickEvent( QMouseEvent *event )
{
    // XXX highres position ??
    if ( BezierCurve::eLength( m_pStrokeManager->getLastPressPixel() - event->pos() ) < 2.0 )
    {
        mScribbleArea->endPolyline( mPoints );
        clear();
    }
}

bool PolylineTool::keyPressEvent( QKeyEvent *event )
{
    switch ( event->key() ) {
    case Qt::Key_Return:
        if ( mPoints.size() > 0 )
        {
            mScribbleArea->endPolyline( mPoints );
            clear();
            return true;
        }
        break;

    case Qt::Key_Escape:
        if ( mPoints.size() > 0 ) {
            clear();
            return true;
        }
        break;

    default:
        return false;
    }

    return false;
}
