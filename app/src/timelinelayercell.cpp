#include "timelinelayercell.h"

#include "preferencemanager.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "pencilsettings.h"

#include "layer.h"
#include "layercamera.h"
#include "camerapropertiesdialog.h"

#include <QPalette>
#include <QInputDialog>
#include <QRegularExpression>
#include <QMouseEvent>

#include <QDebug>

TimeLineLayerCell::TimeLineLayerCell(TimeLine* parent,
                                     Editor* editor,
                                     Layer* layer,
                                     const QPoint& origin, int width, int height) : TimeLineBaseCell(parent, editor, origin, width, height)
{
    mLayer = layer;

    mOldBounds = mGlobalBounds;

    if (mLayer->type() == Layer::BITMAP) mIconPixmap = QPixmap(":icons/themes/playful/timeline/cell-bitmap.svg");
    if (mLayer->type() == Layer::VECTOR) mIconPixmap = QPixmap(":icons/themes/playful/timeline/cell-vector.svg");
    if (mLayer->type() == Layer::SOUND)  mIconPixmap = QPixmap(":icons/themes/playful/timeline/cell-sound.svg");
    if (mLayer->type() == Layer::CAMERA) mIconPixmap = QPixmap(":icons/themes/playful/timeline/cell-camera.svg");
}

TimeLineLayerCell::~TimeLineLayerCell()
{
}

void TimeLineLayerCell::paint(QPainter& painter, const QPalette& palette) const
{   
    const LayerVisibility& visibility = mEditor->layerVisibility();
    bool isSelected = mEditor->layers()->selectedLayerId() == mLayer->id();
    paintBackground(painter, palette, isSelected);
    paintLayerVisibility(painter, palette, visibility, isSelected);
    paintLabel(painter, palette, isSelected);
}

void TimeLineLayerCell::paintLayerVisibility(QPainter& painter, const QPalette& palette, const LayerVisibility& layerVisibility, bool isSelected) const
{
    int x = topLeft().x();
    int y = topLeft().y();
    if (!mLayer->visible())
    {
        painter.setBrush(palette.color(QPalette::Base));
    }
    else
    {
        if ((layerVisibility == LayerVisibility::ALL) || isSelected)
        {
            painter.setBrush(palette.color(QPalette::Text));
        }
        else if (layerVisibility == LayerVisibility::CURRENTONLY)
        {
            painter.setBrush(palette.color(QPalette::Base));
        }
        else if (layerVisibility == LayerVisibility::RELATED)
        {
            QColor color = palette.color(QPalette::Text);
            color.setAlpha(128);
            painter.setBrush(color);
        }
    }
    if (isSelected)
    {
        painter.setPen(palette.color(QPalette::HighlightedText));
    }
    else
    {
        painter.setPen(palette.color(QPalette::Text));
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(x + 6, y + 4, 9, 9);
    painter.setRenderHint(QPainter::Antialiasing, false);
}

void TimeLineLayerCell::paintLayerGutter(QPainter& painter, const QPalette& palette) const
{
    painter.setPen(palette.color(QPalette::Mid));
    painter.drawRect(0, mGlobalBounds.bottom(), mGlobalBounds.width(), 2);
}

void TimeLineLayerCell::paintBackground(QPainter& painter, const QPalette& palette, bool isSelected) const
{
    int x = topLeft().x();
    int y = topLeft().y();
    if (isSelected)
    {
        painter.setBrush(palette.color(QPalette::Highlight));
    }
    else
    {
        painter.setBrush(palette.color(QPalette::Base));
    }
    painter.setPen(Qt::NoPen);
    painter.drawRect(x, y, size().width(), size().height()); // empty rectangle by default
}

void TimeLineLayerCell::paintLabel(QPainter& painter, const QPalette& palette, bool isSelected) const
{
    int x = topLeft().x();
    int y = topLeft().y();

    int paddingTop = 1;
    int paddingLeft = mLabelIconSize.width();
    int itemSpacing = 2;

    const QPoint& iconPos = QPoint(x + paddingLeft, y - paddingTop);
    if (mLayer->type() == Layer::BITMAP) painter.drawPixmap(iconPos, mIconPixmap);
    if (mLayer->type() == Layer::VECTOR) painter.drawPixmap(iconPos, mIconPixmap);
    if (mLayer->type() == Layer::SOUND)  painter.drawPixmap(iconPos, mIconPixmap);
    if (mLayer->type() == Layer::CAMERA) painter.drawPixmap(iconPos, mIconPixmap);

    if (isSelected)
    {
        painter.setPen(palette.color(QPalette::HighlightedText));
    }
    else
    {
        painter.setPen(palette.color(QPalette::Text));
    }
    int textCenterY = (y + paddingTop) + (2 * size().height()) / 3;
    painter.drawText(QPoint(iconPos.x() + mLabelIconSize.width() + itemSpacing, textCenterY), mLayer->name());
}

void TimeLineLayerCell::mousePressEvent(QMouseEvent *event)
{   
    if (!mGlobalBounds.contains(event->pos())) { return; }
    
    int layerNumber = getLayerNumber(event->pos().y());
    if (layerNumber < 0) { return; }
    
    if (event->pos().x() < 15)
    {
        mEditor->switchVisibilityOfLayer(layerNumber);
    }
    else if (mEditor->currentLayerIndex() != layerNumber)
    {
        mEditor->layers()->setCurrentLayer(layerNumber);
        mEditor->layers()->currentLayer()->deselectAll();
    }

    if (event->pos().x() > 15) {
        if (event->buttons() & Qt::LeftButton) {
            mIsDraggable = true;
            mOldBounds = mGlobalBounds;
            emit drag(DragEvent::STARTED, this, 0, mGlobalBounds.top());
        }
    }
}

void TimeLineLayerCell::mouseMoveEvent(QMouseEvent *event)
{   
    if (event->buttons() & Qt::LeftButton && mIsDraggable) {
        if (hasDetached(event->pos().y() - mOldBounds.center().y())) {
            mDidDetach = true;
            move(0, event->pos().y() - mGlobalBounds.center().y());
            emit drag(DragEvent::DRAGGING, this, 0, mGlobalBounds.top());
        } else {
            mDidDetach = false;
            mGlobalBounds = mOldBounds;
            emit drag(DragEvent::DRAGGING, this, 0, mOldBounds.top());
        }
    }
}

void TimeLineLayerCell::mouseReleaseEvent(QMouseEvent *event)
{
    handleDraggingEnded(event);
}

void TimeLineLayerCell::handleDraggingEnded(QMouseEvent* event)
{
    if (mDidDetach) {
        emit drag(DragEvent::ENDED, this, 0, mOldBounds.top());
        mDidDetach = false;
    }
    mIsDraggable = false;
    mGlobalBounds = mOldBounds;
}

int TimeLineLayerCell::getLayerNumber(int posY) const
{
    int layerNumber = 0;
    int totalLayerCount = mEditor->layers()->count();
    if (posY - size().height() > 0) {
        layerNumber = posY / size().height();
    }

    // Layers numbers are displayed in descending order
    // The last row is layer 0
    if (layerNumber <= totalLayerCount)
        layerNumber = (totalLayerCount - 1) - layerNumber;
    else
        layerNumber = 0;

    if (posY < topLeft().y())
    {
        layerNumber = -1;
    }

    if (layerNumber >= totalLayerCount)
    {
        layerNumber = totalLayerCount;
    }

    if (layerNumber < -1)
    {
        layerNumber = -1;
    }
    return layerNumber;
}

void TimeLineLayerCell::editLayerProperties() const
{
    if (mLayer->type() == Layer::CAMERA) {
        editLayerProperties(static_cast<LayerCamera*>(mLayer));
    } else {
        editLayerName(mLayer);
    }
}

void TimeLineLayerCell::editLayerProperties(LayerCamera* cameraLayer) const
{
    QRegularExpression regex("([\\x{FFEF}-\\x{FFFF}])+");

    CameraPropertiesDialog dialog(cameraLayer->name(), cameraLayer->getViewRect().width(),
                                  cameraLayer->getViewRect().height());
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }
    QString name = dialog.getName().replace(regex, "");

    if (!name.isEmpty())
    {
        mEditor->layers()->renameLayer(cameraLayer, name);
    }
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_FIELD_W, dialog.getWidth());
    settings.setValue(SETTING_FIELD_H, dialog.getHeight());
    cameraLayer->setViewRect(QRect(-dialog.getWidth() / 2, -dialog.getHeight() / 2, dialog.getWidth(), dialog.getHeight()));
    mEditor->view()->forceUpdateViewTransform();
}

void TimeLineLayerCell::editLayerName(Layer* layer) const
{
    QRegularExpression regex("([\\x{FFEF}-\\x{FFFF}])+");

    bool ok;
    QString name = QInputDialog::getText(nullptr, QObject::tr("Layer Properties"),
                                         QObject::tr("Layer name:"), QLineEdit::Normal,
                                         layer->name(), &ok);
    name.replace(regex, "");
    if (!ok || name.isEmpty())
    {
        return;
    }

    mEditor->layers()->renameLayer(layer, name);
}

