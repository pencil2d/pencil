#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include <QColor>
#include <QTransform>


class ObjectData
{
public:
    ObjectData();

	void setCurrentFrame( int n ) { mCurrentFrame = n; }
	int  getCurrentFrame() const { return mCurrentFrame; }

	void   setCurrentColor( QColor c ) { mCurrentColor = c; }
	QColor getCurrentColor() const { return mCurrentColor; }

	void setCurrentLayer( int n ) { mCurrentLayer = n; }
	int  getCurrentLayer() const { return mCurrentLayer; }

	void setCurrentView( QTransform t ) { mCurrentView = t; }
	QTransform getCurrentView() const { return mCurrentView; }

	void setFrameRate( int n ) { mFps = n; }
	int  getFrameRate() const { return mFps; }
	
	void setLooping( bool b ) { mIsLoop = b; }
	bool isLooping() const { return mIsLoop; }

	void setRangedPlayback( bool b ) { mIsRangedPlayback = b; }
	bool isRangedPlayback() const { return mIsRangedPlayback; }

	void setMarkInFrameNumber( int n ) { mMarkInFrame = n; }
	int  getMarkInFrameNumber() const { return mMarkInFrame; }

	void setMarkOutFrameNumber( int n ) { mMarkOutFrame = n; }
	int  getMarkOutFrameNumber() const { return mMarkOutFrame; }

private:
	int mCurrentFrame = 0;
	QColor mCurrentColor{ 0, 0, 0, 255 };
	int mCurrentLayer = 2; // Layers are counted bottom up
                           // 0 - Camera Layer
                           // 1 - Vector Layer
                           // 2 - Bitmap Layer
    // view manager
    QTransform mCurrentView;
    
    // playback manager
    int  mFps    = 12;
    bool mIsLoop = false;
    bool mIsRangedPlayback = false;
    int  mMarkInFrame  = 1;
    int  mMarkOutFrame = 10;

};

#endif // EDITORSTATE_H
