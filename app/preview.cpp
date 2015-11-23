
#include "preview.h"
#include <QPainter>


PreviewCanvas::PreviewCanvas( QWidget* parent ) : QWidget( parent )
{
	setFixedSize( 200, 200 );
}

void PreviewCanvas::paintEvent( QPaintEvent* )
{
	QPainter painter( this );
	if ( mBitmapImage )
	{
		painter.drawImage( rect( ), *( mBitmapImage->image() ) );
	}
	painter.end( );
}

PreviewWidget::PreviewWidget( QWidget* parent ) : QDockWidget( parent )
{
	mCanvas = new PreviewCanvas( this );
	setWidget( mCanvas );
}

void PreviewWidget::updateImage()
{
	mCanvas->update();
}