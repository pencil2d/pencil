#include "timelinelayercell.h"

#include "preferencemanager.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "pencilsettings.h"

#include "camerapropertiesdialog.h"

#include "layer.h"
#include "layercamera.h"

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
}

TimeLineLayerCell::~TimeLineLayerCell()
{
}

void TimeLineLayerCell::paint(QPainter& painter, const QPalette& palette) const
{
    const LayerVisibility& visibility = mEditor->layerVisibility();
    bool isSelected = mEditor->layers()->selectedLayerId() == mLayer->id();
    qDebug() << "isSelected: " << mEditor->layers()->selectedLayerId();
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
    int paddingLeft = 22;
    int itemSpacing = 2;

    const QPoint& iconPos = QPoint(x + paddingLeft, y - paddingTop);
    if (mLayer->type() == Layer::BITMAP) painter.drawPixmap(iconPos, QPixmap(":icons/themes/playful/timeline/cell-bitmap.svg"));
    if (mLayer->type() == Layer::VECTOR) painter.drawPixmap(iconPos, QPixmap(":icons/themes/playful/timeline/cell-vector.svg"));
    if (mLayer->type() == Layer::SOUND)  painter.drawPixmap(iconPos, QPixmap(":icons/themes/playful/timeline/cell-sound.svg"));
    if (mLayer->type() == Layer::CAMERA) painter.drawPixmap(iconPos, QPixmap(":icons/themes/playful/timeline/cell-camera.svg"));

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
    if (layerNumber == -1)
    {
        if (event->pos().x() < 15)
        {
            if (event->button() == Qt::LeftButton) {
                mEditor->increaseLayerVisibilityIndex();
            } else if (event->button() == Qt::RightButton) {
                mEditor->decreaseLayerVisibilityIndex();
            }
        }
    }
}

int TimeLineLayerCell::getLayerNumber(int posY) const
{
    int layerNumber = 0;
    int totalLayerCount = mEditor->layers()->count();
    if (posY - size().height() > 0) {
        layerNumber = (posY - size().height()) / size().height();
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


void TimeLineLayerCell::mouseMoveEvent(QMouseEvent *event)
{

}

void TimeLineLayerCell::mouseReleaseEvent(QMouseEvent *event)
{

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

