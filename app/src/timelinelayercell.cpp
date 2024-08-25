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

TimeLineLayerCell::TimeLineLayerCell(TimeLine* parent, Editor* editor, Layer* layer, const QPalette& palette, const QRect& rect)
{
    mEditor = editor;
    mTimeLine = parent;
    mLocalRect = rect;
    mLayer = layer;
    mPalette = palette;
    mPrefs = mEditor->preference();
}

void TimeLineLayerCell::paint(QPainter& painter, bool isSelected, const LayerVisibility &LayerVisibility) const
{
    paintBackground(painter, isSelected);
    paintLayerVisibility(painter, LayerVisibility, isSelected);
    paintLabel(painter, isSelected);
}

void TimeLineLayerCell::paintLayerVisibility(QPainter& painter, const LayerVisibility& layerVisibility, bool isSelected) const
{
    int x = posX();
    int y = posY();
    if (!mLayer->visible())
    {
        painter.setBrush(mPalette.color(QPalette::Base));
    }
    else
    {
        if ((layerVisibility == LayerVisibility::ALL) || isSelected)
        {
            painter.setBrush(mPalette.color(QPalette::Text));
        }
        else if (layerVisibility == LayerVisibility::CURRENTONLY)
        {
            painter.setBrush(mPalette.color(QPalette::Base));
        }
        else if (layerVisibility == LayerVisibility::RELATED)
        {
            QColor color = mPalette.color(QPalette::Text);
            color.setAlpha(128);
            painter.setBrush(color);
        }
    }
    if (isSelected)
    {
        painter.setPen(mPalette.color(QPalette::HighlightedText));
    }
    else
    {
        painter.setPen(mPalette.color(QPalette::Text));
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(x + 6, y + 4, 9, 9);
    painter.setRenderHint(QPainter::Antialiasing, false);
}

void TimeLineLayerCell::paintBackground(QPainter& painter, bool isSelected) const
{
    if (isSelected)
    {
        painter.setBrush(mPalette.color(QPalette::Highlight));
    }
    else
    {
        painter.setBrush(mPalette.color(QPalette::Base));
    }
    painter.setPen(Qt::NoPen);
    painter.drawRect(posX(), posY(), mLocalRect.width(), mLocalRect.height()); // empty rectangle by default
}

void TimeLineLayerCell::paintLabel(QPainter& painter, bool isSelected) const
{
    int y = posY();

    int paddingTop = 1;
    int paddingLeft = 22;
    int itemSpacing = 2;

    const QPoint& iconPos = QPoint(posX() + paddingLeft, y - paddingTop);
    if (mLayer->type() == Layer::BITMAP) painter.drawPixmap(iconPos, QPixmap(":icons/themes/playful/timeline/cell-bitmap.svg"));
    if (mLayer->type() == Layer::VECTOR) painter.drawPixmap(iconPos, QPixmap(":icons/themes/playful/timeline/cell-vector.svg"));
    if (mLayer->type() == Layer::SOUND)  painter.drawPixmap(iconPos, QPixmap(":icons/themes/playful/timeline/cell-sound.svg"));
    if (mLayer->type() == Layer::CAMERA) painter.drawPixmap(iconPos, QPixmap(":icons/themes/playful/timeline/cell-camera.svg"));

    if (isSelected)
    {
        painter.setPen(mPalette.color(QPalette::HighlightedText));
    }
    else
    {
        painter.setPen(mPalette.color(QPalette::Text));
    }
    int textCenterY = (y + paddingTop) + (2 * mLocalRect.height()) / 3;
    painter.drawText(QPoint(iconPos.x() + labelIconSize.width() + itemSpacing, textCenterY), mLayer->name());
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

