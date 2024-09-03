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

    int getLayerHeight() const { return mLayerHeight; }

    int getLayerGutterYPosition(const QMouseEvent* event) const;

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

    TimeLine* mTimeLine = nullptr;
    Editor* mEditor = nullptr; // the editor for which this timeLine operates
    PreferenceManager* mPrefs = nullptr;

    QPixmap mPixmapCache;

    int mGutterPositionY = -1;
    int mFromLayer = 0;
    int mToLayer   = 0;

    bool mRedrawContent = false;
    int mLayerHeight = 20;
    int mStartY = 0;

    // is used to move layers, don't use this to get mousePos;
    int mMouseMoveY = 0;
    Qt::MouseButton mPrimaryButton = Qt::NoButton;

    bool mScrollingVertically = false;

    const static int mLayerDetachThreshold = 5;

    QMap<int, TimeLineLayerCell*> mLayerCells;
    QScrollArea* mScrollArea = nullptr;
};

#endif // TIMELINELAYERLIST_H
