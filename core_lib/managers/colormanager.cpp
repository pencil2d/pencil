
#include "object.h"
#include "editor.h"
#include "colormanager.h"


ColorManager::ColorManager( QObject* parent )
    : BaseManager( parent )
    , m_frontColorIndex( 0 )
{
}

ColorManager::~ColorManager()
{
}

bool ColorManager::initialize()
{
    return true;
}

QColor ColorManager::frontColor()
{
    return editor()->object()->getColour( m_frontColorIndex ).colour;
}

void ColorManager::pickColorNumber( int n )
{
    Q_ASSERT( n >= 0 );

    if ( m_frontColorIndex != n )
    {
        m_frontColorIndex = n;

        QColor currentColor = editor()->object()->getColour( m_frontColorIndex ).colour;
        emit colorNumberChanged(m_frontColorIndex);
		emit colorChanged(currentColor);
    }
}

void ColorManager::pickColor(const QColor& newColor)
{
    QColor currentColor = editor()->object()->getColour( m_frontColorIndex ).colour;
    if (currentColor != newColor)
    {
        editor()->object()->setColour( m_frontColorIndex, newColor );
        emit colorChanged(newColor);

		qDebug("Pick Color(R=%d, G=%d, B=%d)", newColor.red(), newColor.green(), newColor.blue());
    }
}

int ColorManager::frontColorNumber()
{
    return m_frontColorIndex;
}
