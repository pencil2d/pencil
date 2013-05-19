#ifndef BASETOOL_H
#define BASETOOL_H

#include <QObject>
#include <QColor>

enum ToolType { PENCIL, ERASER, SELECT, MOVE, EDIT, HAND, SMUDGE, PEN, POLYLINE, BUCKET, EYEDROPPER, COLOURING };

class Properties
{
public:
    qreal width;
    qreal feather;
    qreal opacity;
    QColor colour;
    int colourNumber;
    bool pressure;
    bool invisibility;
    bool preserveAlpha;
};



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
