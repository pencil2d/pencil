

#include "editor.h"
#include "toolmanager.h"
#include "scribblearea.h"
#include "layervector.h"
#include "layermanager.h"

#include "movetool.h"

MoveTool::MoveTool()
{
}

ToolType MoveTool::type()
{
    return MOVE;
}

void MoveTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}

QCursor MoveTool::cursor()
{
    return Qt::ArrowCursor;
}

void MoveTool::mousePressEvent( QMouseEvent *event )
{
    Layer *layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }

    if ( event->button() == Qt::LeftButton )
    {
        // ----------------------------------------------------------------------
        if ( (layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR) )
        {
            mEditor->backup( tr( "Move" ) );
            mScribbleArea->setMoveMode( ScribbleArea::MIDDLE );

            if ( mScribbleArea->somethingSelected )      // there is an area selection
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
            }

            if ( mScribbleArea->getMoveMode() == ScribbleArea::MIDDLE )
            {
                if ( layer->type() == Layer::BITMAP )
                {
                    if ( !(mScribbleArea->myTransformedSelection.contains( getLastPoint() )) )    // click is outside the transformed selection with the MOVE tool
                    {
                        mScribbleArea->paintTransformedSelection();
                        mScribbleArea->deselectAll();
                    }
                    else if ( event->modifiers() == Qt::ControlModifier ) // --- rotation
                    {
                        mScribbleArea->setMoveMode( ScribbleArea::ROTATION );
                        //qDebug() << "ROTATION";
                    }
                    else if (event->modifiers() == Qt::AltModifier ) // --- symmetry
                    {
                        mScribbleArea->setMoveMode(ScribbleArea::SYMMETRY );
                        //qDebug() << "SYMMETRY";
                    }
                }
                else if ( layer->type() == Layer::VECTOR )
                {
                    VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
                    if ( mScribbleArea->mClosestCurves.size() > 0 )     // the user clicks near a curve
                    {
                        //  editor->backup();
                        if ( !vectorImage->isSelected( mScribbleArea->mClosestCurves ) )
                        {
                            mScribbleArea->paintTransformedSelection();
                            if ( event->modifiers() != Qt::ShiftModifier )
                            {
                                mScribbleArea->deselectAll();
                            }
                            vectorImage->setSelected( mScribbleArea->mClosestCurves, true );
                            mScribbleArea->setSelection( vectorImage->getSelectionRect(), true );
                            mScribbleArea->update();
                        }
                    }
                    else
                    {
                        int areaNumber = vectorImage->getLastAreaNumber( getLastPoint() );
                        if ( areaNumber != -1 )   // the user clicks on an area
                        {
                            if ( !vectorImage->isAreaSelected( areaNumber ) )
                            {
                                if ( event->modifiers() != Qt::ShiftModifier )
                                {
                                    mScribbleArea->deselectAll();
                                }
                                vectorImage->setAreaSelected( areaNumber, true );
                                //setSelection( vectorImage->getSelectionRect() );
                                mScribbleArea->setSelection( QRectF( 0, 0, 0, 0 ), true );
                                mScribbleArea->update();
                            }
                        }
                        else     // the user doesn't click near a curve or an area
                        {
                            if ( !(mScribbleArea->myTransformedSelection.contains( getLastPoint() )) )    // click is outside the transformed selection with the MOVE tool
                            {
                                mScribbleArea->paintTransformedSelection();
                                mScribbleArea->deselectAll();
                            }
                        }
                    }
                }
            }
        }
    }
}

void MoveTool::mouseReleaseEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }

    if ( event->button() == Qt::LeftButton )
    {
        if ( layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR )
        {
            mScribbleArea->mOffset.setX( 0 );
            mScribbleArea->mOffset.setY( 0 );
            mScribbleArea->calculateSelectionTransformation();

            mScribbleArea->myTransformedSelection = mScribbleArea->myTempTransformedSelection;

            if ( layer->type() == Layer::VECTOR )
            {
                auto pLayerVector = static_cast< LayerVector* >( layer );
                VectorImage* vectorImage = pLayerVector->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
                vectorImage->setSelectionTransformation( mScribbleArea->getSelectionTransformation() );
            }

            mScribbleArea->setModified( mEditor->layers()->currentLayerIndex(), mEditor->currentFrame() );
            mScribbleArea->setAllDirty();
        }
    }
}

void MoveTool::mouseMoveEvent( QMouseEvent *event )
{
    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer == NULL ) { return; }

    if ( layer->type() != Layer::BITMAP && layer->type() != Layer::VECTOR )
    {
        return;
    }
    if ( event->buttons() & Qt::LeftButton )   // the user is also pressing the mouse (dragging)
    {
        if ( mScribbleArea->somethingSelected )     // there is something selected
        {
            if ( event->modifiers() != Qt::ShiftModifier )    // (and the user doesn't press shift)
            {
                switch ( mScribbleArea->mMoveMode )
                {
                case ScribbleArea::MIDDLE:
                    if ( QLineF( getLastPressPixel(), getCurrentPixel() ).length() > 4 )
                    {
                        mScribbleArea->myTempTransformedSelection = mScribbleArea->myTransformedSelection.translated( mScribbleArea->mOffset );
                    }
                    break;

                case ScribbleArea::TOPRIGHT:
                    mScribbleArea->myTempTransformedSelection =
                            mScribbleArea->myTransformedSelection.adjusted( 0, mScribbleArea->mOffset.y(), mScribbleArea->mOffset.x(), 0 );
                    break;


                case ScribbleArea::TOPLEFT:
                    mScribbleArea->myTempTransformedSelection =
                            mScribbleArea->myTransformedSelection.adjusted( mScribbleArea->mOffset.x(), mScribbleArea->mOffset.y(), 0, 0 );
                    break;

                    // TOPRIGHT XXX

                case ScribbleArea::BOTTOMLEFT:
                    mScribbleArea->myTempTransformedSelection =
                            mScribbleArea->myTransformedSelection.adjusted( mScribbleArea->mOffset.x(), 0, 0, mScribbleArea->mOffset.y() );
                    break;

                case ScribbleArea::BOTTOMRIGHT:
                    mScribbleArea->myTempTransformedSelection =
                            mScribbleArea->myTransformedSelection.adjusted( 0, 0, mScribbleArea->mOffset.x(), mScribbleArea->mOffset.y() );
                    break;
                case ScribbleArea::ROTATION:
                    mScribbleArea->myTempTransformedSelection =
                            mScribbleArea->myTransformedSelection; // @ necessary?
                    mScribbleArea->myRotatedAngle = getCurrentPixel().x() - getLastPressPixel().x();
                    //qDebug() << "rotation" << m_pScribbleArea->myRotatedAngle;
                    break;
                }

                mScribbleArea->calculateSelectionTransformation();
                mScribbleArea->update();
                mScribbleArea->setAllDirty();
            }
        }
        else     // there is nothing selected
        {
            // we switch to the select tool
            mEditor->tools()->setCurrentTool( SELECT );
            mScribbleArea->mMoveMode = ScribbleArea::MIDDLE;
            mScribbleArea->mySelection.setTopLeft( getLastPoint() );
            mScribbleArea->mySelection.setBottomRight( getLastPoint() );
            mScribbleArea->setSelection( mScribbleArea->mySelection, true );
        }
    }
    else     // the user is moving the mouse without pressing it
    {
        if ( layer->type() == Layer::VECTOR )
        {
            auto layerVector = static_cast< LayerVector* >( layer );
            VectorImage* pVecImg = layerVector->getLastVectorImageAtFrame( mEditor->currentFrame(), 0 );
            mScribbleArea->mClosestCurves = pVecImg->getCurvesCloseTo( getCurrentPoint(), mScribbleArea->tol / mEditor->view()->scaling() );
        }
        mScribbleArea->update();
    }
}
