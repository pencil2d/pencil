#ifndef BASETOOL_H
#define BASETOOL_H

#include <QObject>

enum ToolType { PENCIL, ERASER, SELECT, MOVE, EDIT, HAND, SMUDGE, PEN, POLYLINE, BUCKET, EYEDROPPER, COLOURING };


class BaseTool : public QObject
{
    Q_OBJECT
public:
    explicit BaseTool(QObject *parent = 0);
    virtual ToolType type() = 0;
signals:
    
public slots:
    
};

#endif // BASETOOL_H
