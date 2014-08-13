#ifndef PREVIEW_H
#define PREVIEW_H

#include <QDockWidget>
#include "bitmapimage.h"

class PreviewCanvas : public QWidget
{
	Q_OBJECT
public:
	PreviewCanvas( QWidget* );
	void setImage( BitmapImage* img ) { mBitmapImage = img; }

protected:
	void paintEvent( QPaintEvent* );
private:
	BitmapImage* mBitmapImage = nullptr;
};



class Preview : public QDockWidget
{
    Q_OBJECT
public:
    Preview(QWidget *parent = 0);
	void setImage( BitmapImage* img ) { mCanvas->setImage( img ); }
	void updateImage();

private:
	PreviewCanvas* mCanvas = nullptr;
	
};

#endif // PREVIEW_H
