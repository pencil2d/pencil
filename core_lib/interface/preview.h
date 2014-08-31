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
	void paintEvent( QPaintEvent* ) override;

private:
	BitmapImage* mBitmapImage = nullptr;
};



class PreviewWidget : public QDockWidget
{
    Q_OBJECT
public:
    PreviewWidget(QWidget *parent = 0);
	void setImage( BitmapImage* img ) { mCanvas->setImage( img ); }
	void updateImage();

private:
	PreviewCanvas* mCanvas = nullptr;
	
};

#endif // PREVIEW_H
