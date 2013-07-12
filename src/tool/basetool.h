#ifndef BASETOOL_H
#define BASETOOL_H

#include <QObject>
#include <QColor>
#include <QMap>
#include <QString>
#include <QCursor>
#include "pencildef.h"


class Editor;
class QMouseEvent;


QString typeName( ToolType );

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
    explicit BaseTool(QObject *parent = 0);
    virtual ToolType type() = 0;
    virtual void loadSettings() = 0;
    virtual QCursor cursor();

    void setEditor(Editor* editor);
    Properties properties;

    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

signals:

public slots:

private:
    Editor* m_pEditor;
};

#endif // BASETOOL_H
