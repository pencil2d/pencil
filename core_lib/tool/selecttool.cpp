#include "editor.h"
#include "layervector.h"
#include "scribblearea.h"
#include "layermanager.h"
#include "toolmanager.h"
#include "selecttool.h"

SelectTool::SelectTool()
{
}

ToolType SelectTool::type()
{
    return SELECT;
}

void SelectTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}

QCursor SelectTool::cursor()
{
    return Qt::CrossCursor;
}

void SelectTool::mousePressEvent( QMouseEvent *event )
{
    Layer *layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }

    mScribbleArea->myFlipX = 1.0;
    mScribbleArea->myFlipY = 1.0;
    mScribbleArea->myRotatedAngle = 0;

    if ( event->button() == Qt::LeftButton )
    {
        if ( layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR )
        {
            if ( layer->type() == Layer::VECTOR )
            {
                ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->deselectAll();
            }
            mScribbleArea->setMoveMode( ScribbleArea::MIDDLE );
            mEditor->backup( typeName() );

            if ( mScribbleArea->somethingSelected )      // there is something selected
            {
                if ( BezierCurve::mLength( getLastPoint() - mScribbleArea->myTransformedSelection.topLeft() ) < 6 )
                {
                    mScribbleArea->setMoveMode( ScribbleArea::TOPLEFT );
                }
                if ( BezierCurve::mLength( getLastPoint() - mScribbleArea->myTransformedSelection.topRight() ) < 6 )
                {
                    mScribbleArea->setMoveMode( ScribbleArea::TOPRIGHT );
                }
                if ( BezierCurve::mLength( getLastPoint() - mScribbleArea->myTransformedSelection.bottomLeft() ) < 6 )
                {
                    mScribbleArea->setMoveMode( ScribbleArea::BOTTOMLEFT );
                }
                if ( BezierCurve::mLength( getLastPoint() - mScribbleArea->myTransformedSelection.bottomRight() ) < 6 )
                {
                    mScribbleArea->setMoveMode( ScribbleArea::BOTTOMRIGHT );
                }
                if ( mScribbleArea->getMoveMode() == ScribbleArea::MIDDLE )
                {
                    mScribbleArea->paintTransformedSelection();
                    mScribbleArea->deselectAll();
                } // the user did not click on one of the corners
            }
            else     // there is nothing selected
            {
                mScribbleArea->mySelection.setTopLeft( getLastPoint() );
                mScribbleArea->mySelection.setBottomRight( getLastPoint() );
                mScribbleArea->setSelection( mScribbleArea->mySelection, true );
            }
            mScribbleArea->update();
        }
    }
}

void SelectTool::mouseReleaseEvent( QMouseEvent *event )
{
    Layer *layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }

    if ( event->button() == Qt::LeftButton )
    {
        if ( layer->type() == Layer::VECTOR )
        {
            if ( mScribbleArea->somethingSelected )
            {
                mEditor->tools()->setCurrentTool( MOVE );

                VectorImage *vectorImage = ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
                mScribbleArea->setSelection( vectorImage->getSelectionRect(), true );
                if ( mScribbleArea->mySelection.width() <= 0 && mScribbleArea->mySelection.height() <= 0 )
                {
                    mScribbleArea->deselectAll();
                }
            }
            mScribbleArea->updateCurrentFrame();
            mScribbleArea->setAllDirty();
        }
        else if ( layer->type() == Layer::BITMAP )
        {
            if ( mScribbleArea->mySelection.width() <= 0 && mScribbleArea->mySelection.height() <= 0 )
            {
                mScribbleArea->deselectAll();
            }
            mScribbleArea->updateCurrentFrame();
            mScribbleArea->setAllDirty();
        }
    }
}

void SelectTool::mouseMoveEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }

    if ( ( event->buttons() & Qt::LeftButton ) && mScribbleArea->somethingSelected && ( layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR ) )
    {
        switch ( mScribbleArea->getMoveMode() )
        {
        case ScribbleArea::MIDDLE:
            mScribbleArea->mySelection.setBottomRight( getCurrentPoint() );
            break;

        case ScribbleArea::TOPLEFT:
            mScribbleArea->mySelection.setTopLeft( getCurrentPoint() );
            break;

        case ScribbleArea::TOPRIGHT:
            mScribbleArea->mySelection.setTopRight( getCurrentPoint() );
            break;

        case ScribbleArea::BOTTOMLEFT:
            mScribbleArea->mySelection.setBottomLeft( getCurrentPoint() );
            break;

        case ScribbleArea::BOTTOMRIGHT:
            mScribbleArea->mySelection.setBottomRight( getCurrentPoint() );
            break;
        }

        mScribbleArea->myTransformedSelection = mScribbleArea->mySelection.adjusted( 0, 0, 0, 0 );
        mScribbleArea->myTempTransformedSelection = mScribbleArea->mySelection.adjusted( 0, 0, 0, 0 );

        if ( layer->type() == Layer::VECTOR )
        {
            ( ( LayerVector * )layer )->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 )->select( mScribbleArea->mySelection );
        }
        mScribbleArea->update();
    }
}

bool SelectTool::keyPressEvent(QKeyEvent *event)
{
    switch ( event->key() ) {
    case Qt::Key_Alt:
        mScribbleArea->setTemporaryTool( MOVE );
        break;
    default:
        break;
    }

    // Follow the generic behaviour anyway
    //
    return false;
}
