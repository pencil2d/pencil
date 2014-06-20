
#include "object.h"
#include "editor.h"
#include "colormanager.h"


ColorManager::ColorManager( QObject* parent )
    : BaseManager( parent )
    , mCurrentColorIndex( 0 )
    , mCurrentFrontColor( 0, 0, 0 )
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
    return editor()->object()->getColour( mCurrentColorIndex ).colour;
}

void ColorManager::setColorNumber( int n )
{
    Q_ASSERT( n >= 0 );

    if ( mCurrentColorIndex != n )
    {
        mCurrentColorIndex = n;

        QColor currentColor = editor()->object()->getColour( mCurrentColorIndex ).colour;
        emit colorNumberChanged(mCurrentColorIndex);
		emit colorChanged(currentColor);
    }
}

void ColorManager::setColor(const QColor& newColor)
{
    QColor currentColor = editor()->object()->getColour( mCurrentColorIndex ).colour;
    if (currentColor != newColor)
    {
        editor()->object()->setColour( mCurrentColorIndex, newColor );
        emit colorChanged(newColor);

		qDebug("Pick Color(R=%d, G=%d, B=%d)", newColor.red(), newColor.green(), newColor.blue());
    }
}

int ColorManager::frontColorNumber()
{
    return mCurrentColorIndex;
}
