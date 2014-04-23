
#include "object.h"
#include "editor.h"
#include "colormanager.h"


ColorManager::ColorManager( QObject* parent )
    : BaseManager( parent )
    , m_currentColorIndex( 0 )
    , m_currentFrontColor( 0, 0, 0 )
{
}

ColorManager::~ColorManager()
{
}

bool ColorManager::init()
{
    return true;
}

QColor ColorManager::frontColor()
{
    return editor()->object()->getColour( m_currentColorIndex ).colour;
}

void ColorManager::setColorNumber( int n )
{
    Q_ASSERT( n >= 0 );

    if ( m_currentColorIndex != n )
    {
        m_currentColorIndex = n;

        QColor currentColor = editor()->object()->getColour( m_currentColorIndex ).colour;
        emit colorNumberChanged(m_currentColorIndex);
		emit colorChanged(currentColor);
    }
}

void ColorManager::setColor(const QColor& newColor)
{
    QColor currentColor = editor()->object()->getColour( m_currentColorIndex ).colour;
    if (currentColor != newColor)
    {
        editor()->object()->setColour( m_currentColorIndex, newColor );
        emit colorChanged(newColor);

		qDebug("Pick Color(R=%d, G=%d, B=%d)", newColor.red(), newColor.green(), newColor.blue());
    }
}

int ColorManager::frontColorNumber()
{
    return m_currentColorIndex;
}
