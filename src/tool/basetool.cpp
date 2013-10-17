#include "editor.h"
#include "basetool.h"

#include "scribblearea.h"
#include "strokemanager.h"

// ---- shared static variables ---- ( only one instance for all the tools )
ToolPropertyType BaseTool::assistedSettingType; // setting beeing changed
qreal BaseTool::OriginalSettingValue;  // start value (width, feather ..)
bool BaseTool::isAdjusting = false;

QString BaseTool::TypeName(ToolType type)
{
    static QMap<ToolType, QString>* map = NULL;

    if ( map == NULL )
    {
        map = new QMap<ToolType, QString>();
        map->insert(PENCIL, "Pencil");
        map->insert(ERASER, "Eraser");
        map->insert(SELECT, "Select");
        map->insert(MOVE, "Move");
        map->insert(HAND, "Hand");
        map->insert(SMUDGE, "Smudge");
        map->insert(PEN, "Pen");
        map->insert(POLYLINE, "Polyline");
        map->insert(BUCKET, "Bucket");
        map->insert(EYEDROPPER, "Eyedropper");
        map->insert(BRUSH, "Brush");
    }

    return map->value( type );
}

BaseTool::BaseTool(QObject *parent) :
QObject(parent),
adjustmentStep(0)
{
}


QCursor BaseTool::cursor()
{
    return Qt::ArrowCursor;
}

void BaseTool::initialize(Editor* editor, ScribbleArea *scribbleArea)
{
    if (editor == NULL)
    {
        qCritical("ERROR: editor is null!");
    }
    m_pEditor = editor;
    m_pScribbleArea = scribbleArea;
    m_pStrokeManager = scribbleArea->getStrokeManager();

    loadSettings();
}

void BaseTool::mousePressEvent( QMouseEvent* )
{

}

void BaseTool::mouseMoveEvent( QMouseEvent* )
{

}

void BaseTool::mouseReleaseEvent( QMouseEvent* )
{

}

void BaseTool::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePressEvent(event);
}

QCursor BaseTool::circleCursors() // Todo: only one instance required: make fn static?
{
    qreal zoomFactor= m_pScribbleArea->getCentralViewScale(); //scale factor
    //qDebug() << "--->" << zoomFactor;
    qreal propWidth = properties.width * zoomFactor;
    qreal propFeather = properties.feather * zoomFactor;
    qreal width = propWidth + 0.5 * propFeather;

    if (width < 1) { width = 1; }
    qreal radius = width/2;
    qreal xyA = 1 + propFeather/2;
    qreal xyB = 1 + propFeather/8;
    qreal whA = qMax(0.0, propWidth-xyA - 1);
    qreal whB = qMax(0.0, width-propFeather/4 - 2);
    QPixmap pixmap(width, width);
    if (!pixmap.isNull())
    {
        pixmap.fill( QColor(255,255,255,0) );
        QPainter painter(&pixmap);
        painter.setPen( QColor(0,0,0,190) );
        painter.drawLine( QPointF(radius-2,radius), QPointF(radius+2,radius) );
        painter.drawLine( QPointF(radius,radius-2), QPointF(radius,radius+2) );
        painter.setRenderHints(QPainter::Antialiasing, true);
        painter.setPen( QColor(0,0,0,255) );
        painter.setBrush( QColor(192,192,192,64) );
        painter.setCompositionMode(QPainter::CompositionMode_Exclusion);
        painter.drawEllipse( QRectF(xyB, xyB, whB, whB) ); // outside circle
        painter.setBrush( QColor(255,255,255,127) );
        painter.drawEllipse( QRectF( xyA, xyA, whA, whA) ); // inside circle
        painter.end();
    }
    return QCursor(pixmap);

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
    m_pScribbleArea->setCursor(cursor()); // cursor() changes in brushtool, erasertool, ...

}

void BaseTool::stopAdjusting()
{
    isAdjusting = false;
    adjustmentStep = 0;
    OriginalSettingValue = 0;
    m_pScribbleArea->setCursor(cursor());
}

void BaseTool::adjustCursor(qreal argOffsetX ) //offsetx x-lastx
{
    qreal incx = pow(OriginalSettingValue*100,0.5);
    qreal newValue = incx + argOffsetX;

    if (newValue < 0)
    {
        newValue = 0;
    }
    newValue = pow(newValue, 2) / 100;

    if (adjustmentStep>0) {
        int tempValue = (int)(newValue/adjustmentStep); // + 0.5 ?
        newValue = tempValue * adjustmentStep;
    }

    if (newValue < 0.2) // can be optimized for size: min(200,max(0.2,newValue))
    {
        newValue = 0.2;
    }
    else if (newValue > 200)
    {
        newValue = 200;
    }

    if ( assistedSettingType == WIDTH ) // can be optimized (not necessarily)
    {
        m_pEditor->applyWidth( newValue );
    }
    else if ( assistedSettingType == FEATHER )
    {
        m_pEditor->applyFeather( newValue );
    }

}


void BaseTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
    Q_UNUSED(pressure);
    Q_UNUSED(mouseDevice);
}

QPointF BaseTool::getCurrentPixel()
{
    return m_pStrokeManager->getCurrentPixel();
}

QPointF BaseTool::getCurrentPoint()
{
    return m_pScribbleArea->pixelToPoint(getCurrentPixel());
}

QPointF BaseTool::getLastPixel()
{
    return m_pStrokeManager->getLastPixel();
}

QPointF BaseTool::getLastPoint()
{
    return m_pScribbleArea->pixelToPoint(getLastPixel());
}

QPointF BaseTool::getLastPressPixel()
{
    return m_pStrokeManager->getLastPressPixel();
}

QPointF BaseTool::getLastPressPoint()
{
    return m_pScribbleArea->pixelToPoint(getLastPressPixel());
}
