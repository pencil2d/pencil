#include "colormanager.h"


ColorManager::ColorManager(Editor* editor, QObject* parent = 0) 
    : QObject( parent )
{
    Q_ASSERT( editor != NULL );
    m_editor = editor;
}

QColor ColorManager::frontColor()
{    
    return m_editor->getObject()->getColour( m_frontColorIndex ).colour;
}

void ColorManager::pickColorNumber( int n )
{
    Q_ASSERT( n >= 0 );

    if ( m_frontColorIndex != n )
    {
        m_frontColorIndex = n;

        QColor currentColor = m_editor->getObject()->getColour( m_frontColorIndex ).colour;
        emit colorChanged(currentColor);
    }
}