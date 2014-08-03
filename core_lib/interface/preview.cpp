
#include "preview.h"
#include <QPainter>

PreviewCanvas::PreviewCanvas( QWidget* parent ) : QWidget( parent )
{
	setFixedSize( 200, 200 );
}

void PreviewCanvas::paintEvent( QPaintEvent* )
{
	QPainter painter( this );
	if ( mImage )
	{
		painter.drawImage( rect( ), *( mImage->mImage ) );
	}
	painter.end( );
}

Preview::Preview( QWidget* parent ) : QDockWidget( parent )
{
	mCanvas = new PreviewCanvas( this );
	setWidget( mCanvas );
}

void Preview::updateImage()
{
	mCanvas->update();
}