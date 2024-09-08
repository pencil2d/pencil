#ifndef TIMELINELAYERLIST_H
#define TIMELINELAYERLIST_H

#include <QMap>
#include <QWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include "timelinelayercell.h"

enum class LayerVisibility;
enum class SETTING;

class Layer;
class TimeLine;
class Editor;
class PreferenceManager;
class TimeLineLayerHeaderWidget;

class QScrollArea;
class QPaintEvent;
class QMouseEvent;
class QResizeEvent;
class QListWidget;

class TimeLineLayerList : public QWidget
{
    Q_OBJECT

public:
    TimeLineLayerList( TimeLine* parent, Editor* editor);
    ~TimeLineLayerList() override;

    int getLayerHeight() const { return mLayerHeight; }

    int getLayerGutterYPosition(int posY) const;

    void loadLayerCells();
    void onCellDragged(const DragEvent& event, const TimeLineLayerCell* cell, int x, int y);

signals:
    void cellDraggedY(const DragEvent& event, int y);
    void offsetChanged(int);

public slots:
    void updateContent();
    void vScrollChange(int);
    void onScrollingVerticallyStopped();

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

    TimeLineLayerCell* getCell(int id) const;

    int getLayerNumber(int y) const;
    int getLayerCellY(int layerNumber) const;

    void drawContent();
    void paintLayerGutter(QPainter& painter, const QPalette&) const;

    TimeLine* mTimeLine = nullptr;
    Editor* mEditor = nullptr; // the editor for which this timeLine operates
    PreferenceManager* mPrefs = nullptr;

    QPixmap mPixmapCache;

    int mGutterPositionY = -1;
    int mFromLayer = 0;

    bool mRedrawContent = false;
    int mLayerHeight = 20;

    Qt::MouseButton mPrimaryButton = Qt::NoButton;

    bool mScrollingVertically = false;

    const static int mLayerDetachThreshold = 5;

    QMap<int, TimeLineLayerCell*> mLayerCells;
};

#endif // TIMELINELAYERLIST_H
