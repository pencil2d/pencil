#include "timelinelayercelleditorwidget.h"

#include "editor.h"
#include "timeline.h"
#include "pencilsettings.h"

#include "layermanager.h"
#include "layercamera.h"
#include "preferencemanager.h"
#include "viewmanager.h"
#include "camerapropertiesdialog.h"

#include <QApplication>
#include <QInputDialog>
#include <QLineEdit>
#include <QRegularExpression>
#include <QDebug>

TimeLineLayerCellEditorWidget::TimeLineLayerCellEditorWidget(QWidget* parent,
                                                             Editor* editor,
                                                             Layer* layer)
    : QWidget(parent),
      mEditor(editor),
      mLayer(layer)
{

    if (mLayer->type() == Layer::BITMAP) mIconPixmap = QPixmap(":icons/themes/playful/timeline/cell-bitmap.svg");
    if (mLayer->type() == Layer::VECTOR) mIconPixmap = QPixmap(":icons/themes/playful/timeline/cell-vector.svg");
    if (mLayer->type() == Layer::SOUND)  mIconPixmap = QPixmap(":icons/themes/playful/timeline/cell-sound.svg");
    if (mLayer->type() == Layer::CAMERA) mIconPixmap = QPixmap(":icons/themes/playful/timeline/cell-camera.svg");
}

void TimeLineLayerCellEditorWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPalette palette = QApplication::palette();

    const LayerVisibility& visibility = mEditor->layerVisibility();
    bool isSelected = mEditor->layers()->selectedLayerId() == mLayer->id();
    paintBackground(painter, palette, isSelected);
    paintLayerVisibility(painter, palette, visibility, isSelected);
    paintLabel(painter, palette, isSelected);
}

void TimeLineLayerCellEditorWidget::paintLayerVisibility(QPainter& painter, const QPalette& palette, const LayerVisibility& layerVisibility, bool isSelected) const
{
    int x = rect().topLeft().x();
    int y = rect().topLeft().y();
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

void TimeLineLayerCellEditorWidget::paintLayerGutter(QPainter& painter, const QPalette& palette) const
{
    painter.setPen(palette.color(QPalette::Mid));
    painter.drawRect(0, rect().bottom(), rect().width(), 2);
}

void TimeLineLayerCellEditorWidget::paintBackground(QPainter& painter, const QPalette& palette, bool isSelected) const
{
    int x = rect().topLeft().x();
    int y = rect().topLeft().y();
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

void TimeLineLayerCellEditorWidget::paintLabel(QPainter& painter, const QPalette& palette, bool isSelected) const
{
    int x = rect().topLeft().x();
    int y = rect().topLeft().y();

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

void TimeLineLayerCellEditorWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << event->pos();

    if (isInsideLayerVisibilityArea(event))
    {
        mLayer->switchVisibility();
        emit layerVisibilityChanged();
    }
    else if (mEditor->layers()->currentLayer() != mLayer)
    {
        mEditor->layers()->setCurrentLayer(mLayer);
        mEditor->layers()->currentLayer()->deselectAll();
    }

    handleDragStarted(event);
}

void TimeLineLayerCellEditorWidget::mouseMoveEvent(QMouseEvent *event)
{
    handleDragging(event);
}

void TimeLineLayerCellEditorWidget::mouseReleaseEvent(QMouseEvent *event)
{
    handleDragEnded(event);
}

void TimeLineLayerCellEditorWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (!isInsideLayerVisibilityArea(event))
        {
            editLayerProperties();
        }
    }
}

bool TimeLineLayerCellEditorWidget::isInsideLayerVisibilityArea(QMouseEvent* event) const
{
    return event->pos().x() < 15;
}

void TimeLineLayerCellEditorWidget::handleDragStarted(QMouseEvent* event)
{
    if (isInsideLayerVisibilityArea(event)) {
        return;
    }

    if (event->buttons() & Qt::LeftButton) {
        mIsDraggable = true;
        mDragFromY = y();
        emit drag(DragEvent::STARTED, this, 0, y());
    }
}

void TimeLineLayerCellEditorWidget::handleDragging(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton && mIsDraggable) {
        int delta = event->pos().y() - y();
        int mappedY = mapToParent(event->pos()).y();
        int centerY = mDragFromY + (size().height() * 0.5);
        if (hasDetached(mappedY - centerY)) {
            mDidDetach = true;
            int newCenter = mapToParent(pos()).y() - size().height() * 0.5;
            emit drag(DragEvent::DRAGGING, this, 0, newCenter + delta);
        } else {
            mDidDetach = false;
            emit drag(DragEvent::DRAGGING, this, 0, mDragFromY);
        }
    }
}

void TimeLineLayerCellEditorWidget::handleDragEnded(QMouseEvent*)
{
    if (mDidDetach) {
        emit drag(DragEvent::ENDED, this, 0, y());
        mDidDetach = false;
    }
    mIsDraggable = false;
    mDragFromY = y();
}

int TimeLineLayerCellEditorWidget::getLayerNumber(int posY) const
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

    if (posY < rect().topLeft().y())
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

void TimeLineLayerCellEditorWidget::editLayerProperties() const
{
    if (mLayer->type() == Layer::CAMERA) {
        editLayerProperties(static_cast<LayerCamera*>(mLayer));
    } else {
        editLayerName(mLayer);
    }
}

void TimeLineLayerCellEditorWidget::editLayerProperties(LayerCamera* cameraLayer) const
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

void TimeLineLayerCellEditorWidget::editLayerName(Layer* layer) const
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
