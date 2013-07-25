#ifndef BASETOOL_H
#define BASETOOL_H

#include <QObject>
#include <QColor>
#include <QMap>
#include <QString>
#include <QCursor>
#include "pencildef.h"

#include <QPointF>

class Editor;
class QMouseEvent;
class ScribbleArea;
class QKeyEvent;
class StrokeManager;

class Properties
{
public:
    qreal width;
    qreal feather;
    qreal opacity;    
    int colourNumber;
    int pressure;
    int invisibility;
    int preserveAlpha;
};

const int ON = 1;
const int OFF = 0;
const int DISABLED = -1;

class BaseTool : public QObject
{
    Q_OBJECT
public:
    static QString TypeName( ToolType );

    explicit BaseTool(QObject *parent = 0);
    virtual ToolType type() = 0;
    QString typeName() { return TypeName(type()); }
    virtual void loadSettings() = 0;
    virtual QCursor cursor();

    virtual void initialize(Editor* editor, ScribbleArea *scribbleArea);

    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void mouseDoubleClickEvent(QMouseEvent*);
    // return true if handled
    virtual bool keyPressEvent(QKeyEvent *) { return false; }

    virtual void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);

    virtual void clear() { }

    void setColour(const int i);
    void setColour(const QColor colour);

    void setWidth(const qreal width);
    void setFeather(const qreal feather);
    void setOpacity(const qreal opacity);
    void setInvisibility(const qreal invisibility);
    void setPressure(const bool pressure);
    void setPreserveAlpha(const bool preserveAlpha);

    Properties properties;

    QPointF getCurrentPixel();
    QPointF getCurrentPoint();
    QPointF getLastPixel();
    QPointF getLastPoint();
    QPointF getLastPressPixel();
    QPointF getLastPressPoint();

signals:

public slots:

protected:
    Editor* m_pEditor;
    ScribbleArea* m_pScribbleArea;
    StrokeManager* m_pStrokeManager;
};

#endif // BASETOOL_H
