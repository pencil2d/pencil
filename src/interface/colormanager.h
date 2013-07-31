#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QObject>
#include <QColor>
#include "object.h"
#include "editor.h"

class Editor;



class ColorManager : public QObject
{
    Q_OBJECT
public:
    ColorManager(Editor* editor, QObject* parent = 0);
    QColor frontColor();
    void   pickColorNumber( int n );
    
signals:
    void colorChanged(QColor);

private:
    Editor* m_editor;
    int m_frontColorIndex;
};

#endif // COLORMANAGER_H
