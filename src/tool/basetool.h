#ifndef BASETOOL_H
#define BASETOOL_H

#include <QObject>
#include <QColor>
#include <QMap>
#include <QString>

enum ToolType { PENCIL, ERASER, SELECT, MOVE, EDIT, HAND, SMUDGE, PEN, POLYLINE, BUCKET, EYEDROPPER, BRUSH };

QString typeName( ToolType );

class Properties
{
public:
    qreal width;
    qreal feather;
    qreal opacity;
    QColor colour;
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

    Properties properties;
signals:

    public slots:

};

#endif // BASETOOL_H
