#ifndef TIMELINECELLS_H
#define TIMELINECELLS_H

#include <QWidget>
#include <QString>


class TimeLine;
class QPaintEvent;
class QMouseEvent;
class QResizeEvent;
class Editor;

enum class TIMELINE_CELL_TYPE
{
    Layers,
    Tracks
};

class TimeLineCells : public QWidget
{
    Q_OBJECT

public:
    TimeLineCells( TimeLine* parent, Editor* editor, TIMELINE_CELL_TYPE );
    int getLayerNumber(int y);
    int getLayerY(int layerNumber);
    int getFrameNumber(int x);
    int getFrameX(int frameNumber);
    int getMouseMoveY() { return mouseMoveY; }
    int getOffsetY() { return m_offsetY; }
    int getLayerHeight() { return layerHeight; }
    int getFrameLength() {return frameLength;}

Q_SIGNALS:
    void mouseMovedY(int);

public slots:
    void updateContent();
    void updateFrame(int frameNumber);
    void lengthChange(QString); //try to remove content and call TimeLine::forceUpdateLength instead
    void frameSizeChange(int);
    void fontSizeChange(int);
    void scrubChange(int);
    void labelChange(int);
    void hScrollChange(int);
    void vScrollChange(int);
    void setMouseMoveY(int x) { mouseMoveY = x;}

protected:
    void drawContent();
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

private:
    TimeLine* timeLine;
    Editor* mEditor; // the editor for which this timeLine operates

    TIMELINE_CELL_TYPE m_eType;

    QPixmap* m_pCache;
    bool drawFrameNumber;
    bool shortScrub;
    int frameLength;
    int frameSize;
    int fontSize;
    bool scrubbing;
    int layerHeight;
    const static int m_offsetX = 0;
    const static int m_offsetY = 20;
    int startY, endY, startLayerNumber;
    int mouseMoveY;
    int frameOffset, layerOffset;
};

#endif // TIMELINECELLS_H
