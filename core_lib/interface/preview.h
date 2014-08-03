#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWidget>
#include "bitmapimage.h"

class Preview : public QWidget
{
    Q_OBJECT
public:
    Preview(QWidget *parent = 0);
	void setImage( BitmapImage* img ) { mImage = img; }

	void updateImage() { update();  }
protected:
	void paintEvent( QPaintEvent* ) override;

private:
	BitmapImage* mImage = nullptr;
};

#endif // PREVIEW_H
