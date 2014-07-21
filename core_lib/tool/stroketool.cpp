#include "stroketool.h"

#include "scribblearea.h"
#include "strokemanager.h"
#include "editor.h"

#ifdef Q_OS_MAC
extern "C" {
    void disableCoalescing();
    void enableCoalescing();
}
#else
extern "C" {
    void disableCoalescing() {}
    void enableCoalescing() {}
}
#endif

StrokeTool::StrokeTool( QObject *parent ) :
BaseTool( parent )
{
}

void StrokeTool::startStroke()
{
    mFirstDraw = true;
    lastPixel = getCurrentPixel();
    mStrokePoints.clear();
    mStrokePoints << mEditor->view()->mapScreenToCanvas( lastPixel );
    mStrokePressures.clear();
    mStrokePressures << m_pStrokeManager->getPressure();
    disableCoalescing();
}

void StrokeTool::endStroke()
{
    mStrokePoints.clear();
    mStrokePressures.clear();
    enableCoalescing();
}

void StrokeTool::drawStroke()
{
    QPointF pixel = getCurrentPixel();
    if ( pixel != lastPixel || !mFirstDraw )
    {
        lastPixel = pixel;
        mStrokePoints << mEditor->view()->mapScreenToCanvas( pixel );
        mStrokePressures << m_pStrokeManager->getPressure();
    }
    else
    {
        mFirstDraw = false;
    }
}