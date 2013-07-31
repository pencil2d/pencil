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
    ColorManager(Editor* editor, QObject* parent = 0) : QObject( parent )
    {
        Q_ASSERT( editor != NULL );
        m_editor = editor;
    }

    QColor frontColor() { return m_editor->getObject()->getColour( m_frontColorIndex ).colour; }
    void   pickColorNumber( int n )
    {
        Q_ASSERT( n >= 0 );
        m_frontColorIndex = n;
    }

signals:
    void colorChanged(QColor);

private:
    Editor* m_editor;
    int m_frontColorIndex;
};

#endif // COLORMANAGER_H
