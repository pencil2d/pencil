
#include "colormanager.h"

#include "object.h"
#include "editor.h"


ColorManager::ColorManager( QObject* parent ) : BaseManager( parent )
{
}

ColorManager::~ColorManager()
{
}

bool ColorManager::init()
{
    return true;
}

Status ColorManager::load( Object* o )
{
    return Status::OK;
}

Status ColorManager::save( Object* o )
{
	o->data()->setCurrentColor( mCurrentFrontColor );
	return Status::OK;
}

QColor ColorManager::frontColor()
{
    return editor()->object()->getColour( mCurrentColorIndex ).colour;
}

void ColorManager::setColorNumber( int n )
{
    Q_ASSERT( n >= 0 );

    mCurrentColorIndex = n;

    QColor currentColor = editor()->object()->getColour( mCurrentColorIndex ).colour;
    emit colorNumberChanged(mCurrentColorIndex);
    emit colorChanged(currentColor);
}

void ColorManager::setColor(const QColor& newColor)
{
    QColor currentColor = editor()->object()->getColour( mCurrentColorIndex ).colour;
    if (currentColor != newColor)
    {
        editor()->object()->setColour( mCurrentColorIndex, newColor );
        emit colorChanged(newColor);

		qDebug("Pick Color(R=%d, G=%d, B=%d, A=%d)", newColor.red(), newColor.green(), newColor.blue(), newColor.alpha());
    }
}

int ColorManager::frontColorNumber()
{
    return mCurrentColorIndex;
}
