#include "timelinelayerlist.h"

#include <QApplication>
#include <QDebug>

#include "editor.h"
#include "layermanager.h"
#include "preferencemanager.h"
#include "timeline.h"

#include "timelinelayercell.h"

TimeLineLayerList::TimeLineLayerList(TimeLine* parent, Editor* editor) : QWidget(parent)
{
    mTimeLine = parent;
    mEditor = editor;
    mPrefs = editor->preference();

    mFontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);

    setMinimumSize(500, 4 * mLayerHeight);
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setMouseTracking(true);

    connect(mPrefs, &PreferenceManager::optionChanged, this, &TimeLineLayerList::loadSetting);
}

TimeLineLayerList::~TimeLineLayerList()
{
}

void TimeLineLayerList::loadSetting(SETTING setting)
{
    switch (setting)
    {
    case SETTING::LABEL_FONT_SIZE:
        mFontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);
        break;
    default:
        break;
    }

    updateContent();
}

void TimeLineLayerList::loadLayerCells()
{
    if (!mLayerCells.isEmpty()) {
        for (const TimeLineLayerCell* cell : qAsConst(mLayerCells)) {
            delete cell;
        }
        mLayerCells.clear();
    }

    for (int i = 0; i < mEditor->layers()->count(); i++)
    {
        Layer* layeri = mEditor->layers()->getLayer(i);
        const int layerY = getLayerY(i);
        TimeLineLayerCell* cell = new TimeLineLayerCell(mTimeLine, mEditor, layeri, QPoint(0, layerY), width() - 1, mLayerHeight);
        mLayerCells.insert(layeri->id(), cell);
    }

    setMinimumHeight(mEditor->layers()->count() * mLayerHeight);
}

int TimeLineLayerList::getLayerNumber(int y) const
{
    int layerNumber =  y / mLayerHeight;

    int totalLayerCount = mEditor->layers()->count();

    // Layers numbers are displayed in descending order
    // The last row is layer 0
    if (layerNumber <= totalLayerCount)
        layerNumber = (totalLayerCount - 1) - layerNumber;
    else
        layerNumber = 0;

    if (y < 0)
    {
        layerNumber = -1;
    }

    if (layerNumber >= totalLayerCount)
    {
        layerNumber = totalLayerCount;
    }

    //If the mouse release event if fired with mouse off the frame of the application
    if (layerNumber < -1)
    {
        layerNumber = -1;
    }
    return layerNumber;
}

int TimeLineLayerList::getInbetweenLayerNumber(int y) const {
    int layerNumber = getLayerNumber(y);
    // Round the layer number towards the drag start
    if(layerNumber != mFromLayer) {
        if(mMouseMoveY > 0 && y < getLayerY(layerNumber) + mLayerHeight / 2) {
            layerNumber++;
        }
        else if(mMouseMoveY < 0 && y > getLayerY(layerNumber) + mLayerHeight / 2) {
            layerNumber--;
        }
    }
    return layerNumber;
}

int TimeLineLayerList::getLayerY(int layerNumber) const
{
    return (mEditor->layers()->count() - 1 - layerNumber) * mLayerHeight;
}

void TimeLineLayerList::updateContent()
{
    mRedrawContent = true;
    update();
}

void TimeLineLayerList::drawContent()
{
    QPainter painter(&mPixmapCache);

    // grey background of the view
    const QPalette palette = QApplication::palette();
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette.color(QPalette::Base));
    painter.drawRect(QRect(0, 0, width(), height()));

    if (mLayerCells.isEmpty()) { return; }

    for (const TimeLineLayerCell* cell : qAsConst(mLayerCells))
    {
        if (mEditor->layers()->selectedLayerId() != cell->layer()->id()) {
            cell->paint(painter, palette);
        }
    }

    int selectedLayerId = mEditor->layers()->selectedLayerId();

    const auto cell = getCell(selectedLayerId);

    cell->paint(painter, palette);

    if (cell->isDraggable()) {
        paintLayerGutter(painter, palette, cell);
    }
    mRedrawContent = false;
}

void TimeLineLayerList::paintLayerGutter(QPainter& painter, const QPalette& palette, const TimeLineLayerCell* cell) const
{
    painter.setPen(palette.color(QPalette::Mid));
    if (cell->didDetach())
    {
        painter.drawRect(0, getLayerY(getInbetweenLayerNumber(mEndY))+cell->mGlobalBounds.height(), width(), 2);
    }
    else
    {
        painter.drawRect(0, cell->mGlobalBounds.bottom(), width(), 2);
    }
}

void TimeLineLayerList::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    if (mPixmapCache.isNull() || mRedrawContent)
    {
        drawContent();
    }
    if (!mPixmapCache.isNull())
    {
        painter.drawPixmap(QPoint(0, 0), mPixmapCache);
    }
}

void TimeLineLayerList::resizeEvent(QResizeEvent* event)
{
    if (event->size() != mPixmapCache.size()) {
        mPixmapCache = QPixmap(event->size() * devicePixelRatioF());
        mPixmapCache.fill(Qt::transparent);
        mPixmapCache.setDevicePixelRatio(this->devicePixelRatioF());
    }
    setMinimumHeight(mEditor->layers()->count() * mLayerHeight);;

    for (TimeLineLayerCell* cell : qAsConst(mLayerCells)) {
        cell->setSize(QSize(event->size().width(), mLayerHeight));
    }

    updateContent();
    event->accept();

}

void TimeLineLayerList::mousePressEvent(QMouseEvent* event)
{
    int layerNumber = getLayerNumber(event->pos().y());
    mCurrentLayerNumber = layerNumber;

    mFromLayer = mToLayer = layerNumber;

    mStartY = event->pos().y();
    mStartLayerNumber = layerNumber;
    mEndY = event->pos().y();

    primaryButton = event->button();

    if (layerNumber != -1) {
        getCell(mEditor->layers()->getLayer(layerNumber)->id())->mousePressEvent(event);
    }
}

void TimeLineLayerList::mouseMoveEvent(QMouseEvent* event)
{
    mLayerPosMoveY = getLayerNumber(event->pos().y());

    if (event->buttons() & Qt::LeftButton ) {
        mEndY = event->pos().y();
        emit mouseMovedY(mEndY - mStartY);
    }

    for (TimeLineLayerCell* cell : qAsConst(mLayerCells)) {
        cell->mouseMoveEvent(event);
    }
}

void TimeLineLayerList::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != primaryButton) return;

    int layerNumber = getLayerNumber(event->pos().y());

    if (!mScrollingVertically && layerNumber != mStartLayerNumber && mStartLayerNumber != -1 && layerNumber != -1)
    {
        mToLayer = getInbetweenLayerNumber(event->pos().y());
        if (mToLayer != mFromLayer && mToLayer > -1 && mToLayer < mEditor->layers()->count())
        {
            // Bubble the from layer up or down to the to layer
            if (mToLayer < mFromLayer) // bubble up
            {
                for (int i = mFromLayer - 1; i >= mToLayer; i--)
                    mEditor->swapLayers(i, i + 1);
            }
            else // bubble down
            {
                for (int i = mFromLayer + 1; i <= mToLayer; i++)
                    mEditor->swapLayers(i, i - 1);
            }
        }
    }

    for (TimeLineLayerCell* cell : qAsConst(mLayerCells)) {
        cell->mouseReleaseEvent(event);
    }

    if (event->button() == Qt::LeftButton)
    {
        emit mouseMovedY(0);
    }

    primaryButton = Qt::NoButton;
    mEndY = mStartY;
    mTimeLine->scrubbing = false;
}

void TimeLineLayerList::mouseDoubleClickEvent(QMouseEvent* event)
{
    int layerNumber = getLayerNumber(event->pos().y());

    // -- layer --
    Layer* layer = mEditor->layers()->getLayer(layerNumber);
    if (layer && event->buttons() & Qt::LeftButton)
    {
        if (event->pos().x() >= 15)
        {
            getCell(layer->id())->editLayerProperties();
        }
    }
    QWidget::mouseDoubleClickEvent(event);
}

void TimeLineLayerList::vScrollChange(int x)
{
    mScrollingVertically = true;
    updateContent();
}

void TimeLineLayerList::onScrollingVerticallyStopped()
{
    mScrollingVertically = false;
}

void TimeLineLayerList::setMouseMoveY(int x)
{
    mMouseMoveY = x;
    updateContent();
}
