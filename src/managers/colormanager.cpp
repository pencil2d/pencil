
#include "object.h"
#include "editor.h"
#include "colormanager.h"


ColorManager::ColorManager(Editor* pEditor, QObject* parent) : QObject( parent ),
    m_frontColorIndex( 0 )
{
    Q_ASSERT( pEditor != NULL );
    m_pEditor = pEditor;
}

QColor ColorManager::frontColor()
{
    return m_pEditor->object()->getColour( m_frontColorIndex ).colour;
}

void ColorManager::pickColorNumber( int n )
{
    Q_ASSERT( n >= 0 );

    if ( m_frontColorIndex != n )
    {
        m_frontColorIndex = n;

        QColor currentColor = m_pEditor->object()->getColour( m_frontColorIndex ).colour;
        emit colorNumberChanged(m_frontColorIndex);
		emit colorChanged(currentColor);
    }
}

void ColorManager::pickColor(const QColor& newColor)
{
    QColor currentColor = m_pEditor->object()->getColour( m_frontColorIndex ).colour;
    if (currentColor != newColor)
    {
        m_pEditor->object()->setColour( m_frontColorIndex, newColor );
        emit colorChanged(newColor);

		qDebug("Pick Color(R=%d, G=%d, B=%d)", newColor.red(), newColor.green(), newColor.blue());
    }
}

int ColorManager::frontColorNumber()
{
    return m_frontColorIndex;
}