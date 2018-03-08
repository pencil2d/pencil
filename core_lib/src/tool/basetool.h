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

#ifndef BASETOOL_H
#define BASETOOL_H

#include <QObject>
#include <QString>
#include <QCursor>
#include <QPointF>
#include <QPixmap>
#include <QHash>
#include "pencildef.h"

class Editor;
class ScribbleArea;
class QKeyEvent;
class QMouseEvent;
class StrokeManager;

class Properties
{
public:
    qreal width       = 1.f;
    qreal feather     = 1.f;
    bool pressure     = 1;
    int invisibility  = 0;
    int preserveAlpha = 0;
    bool vectorMergeEnabled = false;
    bool bezier_state = false;
    bool useFeather   = true;
    int useAA        = 0;
    int inpolLevel    = 0;
    qreal tolerance = 0;
    bool useFillContour = 0;
};

const int ON = 1;
const int OFF = 0;
const int DISABLED = -1;


class BaseTool : public QObject
{
    Q_OBJECT
public:
    static QString TypeName( ToolType );

    static ToolPropertyType assistedSettingType; // dynamic cursor adjustment
    static qreal OriginalSettingValue;  // start from previous value (width, or feather ...)

    explicit BaseTool( QObject* parent = 0 );
    void initialize( Editor* editor );

    QString typeName() { return TypeName( type() ); }

    virtual ToolType type() = 0;
    virtual void loadSettings() = 0;
    virtual QCursor cursor();

    virtual void mousePressEvent( QMouseEvent* ) = 0;
    virtual void mouseMoveEvent( QMouseEvent* ) = 0;
    virtual void mouseReleaseEvent( QMouseEvent* ) = 0;
    virtual void mouseDoubleClickEvent( QMouseEvent* );

    // return true if handled
    virtual bool keyPressEvent( QKeyEvent* ) { return false; }
    virtual bool keyReleaseEvent( QKeyEvent* ) { return false; }

    // dynamic cursor adjustment
    virtual void startAdjusting( ToolPropertyType argSettingType, qreal argStep );
    virtual void stopAdjusting();
    virtual void adjustCursor( qreal argOffsetX, ToolPropertyType type );

    virtual void adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice );

    virtual void clear() {}

    static bool isAdjusting;
    static QPixmap canvasCursor(float brushWidth, float brushFeather, bool useFeather, float scalingFac, int windowWidth);
    static QPixmap quickSizeCursor(float brushWidth, float brushFeather, float scalingFac);

    virtual void setWidth( const qreal width );
    virtual void setFeather( const qreal feather );
    virtual void setInvisibility( const bool invisibility );
    virtual void setBezier( const bool bezier_state );
    virtual void setPressure( const bool pressure );
    virtual void setUseFeather( const bool usingFeather );
    virtual void setPreserveAlpha( const bool preserveAlpha );
    virtual void setVectorMergeEnabled( const bool vectorMergeEnabled );
    virtual void setAA(const int useAA );
    virtual void setInpolLevel( const int level );
    virtual void setTolerance(const int tolerance);
    virtual void setUseFillContour(const bool useFillContour);

    virtual void leavingThisTool(){}
    virtual void switchingLayers(){}

    Properties properties;

    QPointF getCurrentPixel();
    QPointF getCurrentPoint();
    QPointF getLastPixel();
    QPointF getLastPoint();
    QPointF getLastPressPixel();
    QPointF getLastPressPoint();

    bool isPropertyEnabled( ToolPropertyType t ) { return m_enabledProperties[ t ]; }

protected:
    QHash<ToolPropertyType, bool> m_enabledProperties;

    Editor* editor() { return mEditor; }
    Editor* mEditor = nullptr;
    ScribbleArea* mScribbleArea = nullptr;
    StrokeManager* m_pStrokeManager = nullptr;
    qreal mAdjustmentStep = 0.0f;

private:
};

#endif // BASETOOL_H
