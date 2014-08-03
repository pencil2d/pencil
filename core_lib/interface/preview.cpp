
#include "preview.h"

#include <QPainter>

Preview::Preview( QWidget* parent ) : QWidget( parent )
{
	setFixedSize( 400, 400 );
}

void Preview::paintEvent( QPaintEvent* event )
{
	QPainter painter( this );
	if ( mImage )
	{
		painter.drawImage( rect(), *(mImage->mImage) );
	}
}