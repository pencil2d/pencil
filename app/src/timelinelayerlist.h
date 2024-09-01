#ifndef TIMELINELAYERLIST_H
#define TIMELINELAYERLIST_H

#include <QMap>
#include <QWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>

class Layer;
enum class LayerVisibility;
class TimeLine;
class QPaintEvent;
class QMouseEvent;
class QResizeEvent;
class Editor;
class PreferenceManager;
enum class SETTING;
class TimeLineLayerCell;
class TimeLineLayerHeaderWidget;
class QScrollArea;

class TimeLineLayerList : public QWidget
{
    Q_OBJECT

public:
    TimeLineLayerList( TimeLine* parent, Editor* editor);
    ~TimeLineLayerList() override;

    static int getOffsetX() { return mOffsetX; }
    int getLayerHeight() const { return mLayerHeight; }

    void loadLayerCells();

signals:
    void mouseMovedY(int);
    void offsetChanged(int);

public slots:
    void updateContent();
    void vScrollChange(int);
    void onScrollingVerticallyStopped();
    void setMouseMoveY(int x);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private slots:
    void loadSetting(SETTING setting);

private:

    TimeLineLayerCell* getCell(int id) const { return mLayerCells.find(id).value(); }

    int getLayerNumber(int y) const;
    int getInbetweenLayerNumber(int y) const;
    int getLayerY(int layerNumber) const;

    void drawContent();
    void paintLayerGutter(QPainter& painter, const QPalette&, const TimeLineLayerCell* cell) const;

    TimeLine* mTimeLine;
    Editor* mEditor; // the editor for which this timeLine operates
    PreferenceManager* mPrefs;

    QPixmap mPixmapCache;
    bool mRedrawContent = false;
    int mFontSize = 10;
    int mLayerHeight = 20;
    int mStartY = 0;
    int mEndY   = 0;

    int mCurrentLayerNumber = 0;

    int mFromLayer = 0;
    int mToLayer   = 1;
    int mStartLayerNumber = -1;

    // is used to move layers, don't use this to get mousePos;
    int mMouseMoveY = 0;
    int mScrollOffsetY = 0;
    Qt::MouseButton primaryButton = Qt::NoButton;

    bool mScrollingVertically = false;

    int mLayerPosMoveY = 0;

    const static int mOffsetX = 0;
    const static int mLayerDetachThreshold = 5;

    QMap<int, TimeLineLayerCell*> mLayerCells;
    // TimeLineLayerHeaderWidget* mHeaderCell = nullptr;

    QScrollArea* mScrollArea = nullptr;

};

#endif // TIMELINELAYERLIST_H
