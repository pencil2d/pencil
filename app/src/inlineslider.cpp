#include "inlineslider.h"

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOption>
#include <QStylePainter>

#include <QDebug>
#include <QLabel>
#include <QLayout>

#include <QStackedLayout>

#include "lineeditwidget.h"

InlineSlider::InlineSlider(QWidget* parent) : QWidget(parent)
{
    setCornerRadius(mCornerRadiusPercentage);
    setContentsMargins(0,0,0,0);
    setMinimumSize(50,20);

    setLayout(new QStackedLayout);

    mValueLineEditWidget = new LineEditNumberWidget(this, mSliderValue);

    layout()->addWidget(mValueLineEditWidget);
    connect(mValueLineEditWidget, &LineEditNumberWidget::editingFinished, this, &InlineSlider::onLineEditChanged);

    updateLineEditStylesheet();
}

void InlineSlider::init(QString label, qreal min, qreal max, SliderStartPosType type)
{
    mMin = min;
    mMax = max;
    mLabel = label;
    mSliderOrigin = type;
}

InlineSlider::~InlineSlider()
{
}

void InlineSlider::onScreenChanged(qreal devicePixelRatio)
{
    // We need to act on screen change updates to make sure the pixmap is drawn with correct DPI.
    if (devicePixelRatio != mPixmap.devicePixelRatio()) {
        setupPixmap(size());
    }
}

void InlineSlider::onLineEditChanged()
{
    qreal value = mValueLineEditWidget->value();
    setValue(value);
    emit valueChanged(value);
    updateLineEditStylesheet();
}

void InlineSlider::updateLineEditStylesheet()
{
    QString stylesheet = QString("LineEditNumberWidget[readOnly=true] {"
                                "background-color: transparent;"
                                "border: none;"
                                "padding-right: %5;"
                            "}"
                            "LineEditNumberWidget[readOnly=false]{"
                                "border-radius: %1px %2px;"
                                "border: %3px solid %4;"
                                "padding-right: %5;"
                            "}")
                         .arg(mAbsoluteCornerRadiusX)
                         .arg(mAbsoluteCornerRadiusY)
                         .arg(mBorderWidth)
                         .arg(palette().highlight().color().name())
                         .arg(mTextPadding - 2); // The line edit widget has additional implicit padding that we need to account for...

    mValueLineEditWidget->setAlignment(Qt::AlignRight);
    mValueLineEditWidget->setStyleSheet(stylesheet);

}

bool InlineSlider::event(QEvent *event)
{

    #if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        if (event->type() == QEvent::DevicePixelRatioChange) {
    #else
        // Prior to the above version there was no way to easily check when the screen changed...
        // As such we rely on this. The alternative would require more invasive changes which I think is out of scope for this PR.
        if (event->type() == QEvent::ScreenChangeInternal) {
    #endif
            onScreenChanged(this->devicePixelRatio());
            return true;
        }
    return QWidget::event(event);
}

void InlineSlider::paintEvent(QPaintEvent*)
{
    drawSlider();

    QPainter painter(this);
    painter.drawPixmap(0, 0, mPixmap);
    painter.end();
}

void InlineSlider::drawSlider()
{
    QStyleOption option;
    option.initFrom(this);

    QPainter painter;
    mPixmap.fill(Qt::transparent);

    painter.begin(&mPixmap);

    const QRectF& borderRect = this->borderRect();

    painter.setRenderHint(QPainter::Antialiasing);

    QBrush brush(option.palette.highlight());

    painter.save();

    painter.setPen(Qt::NoPen);

    // // First fill the area, so we can blend the filled region with no spill
    painter.setBrush(option.palette.window());
    painter.drawRoundedRect(borderRect,
                            mAbsoluteCornerRadiusX,
                            mAbsoluteCornerRadiusY,
                            Qt::SizeMode::AbsoluteSize);

    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);

    // Fill out the remaining part with some background color
    painter.fillRect(borderRect,
                     option.palette.base());

    // // Draw the filled part of the slider
    switch (mSliderOrigin) {
        case SliderStartPosType::LEFT:
        {

            painter.fillRect(QRect(borderRect.left(),
                                    borderRect.top(),
                                    mSliderPos - mCaretWidth,
                                    borderRect.bottom()),
                                    brush);
            break;
        }
        case SliderStartPosType::MIDDLE:
        {
            // Now fill the with the brush
            painter.fillRect(QRect(mSliderPos,
                                    borderRect.top(),
                                    borderRect.center().x() - mSliderPos - mCaretWidth,
                                    borderRect.height()),
                                    brush);

            painter.save();
            painter.setPen(option.palette.text().color());

            // Draw center line
            painter.drawLine(QPoint(borderRect.center().x() - mCaretWidth, borderRect.top()),
                             QPoint(borderRect.center().x() - mCaretWidth, borderRect.bottom()));
            painter.restore();
            break;
        }
    }

    drawCaret(painter, borderRect, option.palette.dark().color());
    drawLabels(painter, borderRect, option.palette.text().color());
    painter.restore();

    if (mValueLineEditWidget->isReadOnly()) {
        painter.setPen(option.palette.dark().color());
        painter.drawRoundedRect(borderRect, mAbsoluteCornerRadiusX, mAbsoluteCornerRadiusY);
    }

    painter.end();
}

void InlineSlider::drawLabels(QPainter& painter, const QRectF& borderRect, const QColor& textColor)
{
    painter.save();
    painter.setPen(textColor);
    const QRectF& textRect = borderRect.adjusted(mTextPadding, 0, -mTextPadding, 0);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, mLabel);
    painter.restore();
}

void InlineSlider::drawCaret(QPainter& painter, const QRectF& borderRect, const QColor& caretColor)
{
    QPen caretPen = caretColor;;
    caretPen.setWidth(mCaretWidth);
    // // And draw the caret
    painter.save();
    painter.setPen(caretPen);
    painter.drawLine(QPoint(mSliderPos, borderRect.top()),
                     QPoint(mSliderPos, borderRect.bottom()));
    painter.restore();
}

void InlineSlider::setCornerRadius(qreal percentage)
{
    const qreal minRad = qMin(width(), height());
    const qreal maxRad = qMax(width(), height());

    qreal absolutePercentage = maxRad * percentage;

    if (minRad * percentage < absolutePercentage) {
        mAbsoluteCornerRadiusX = minRad * percentage;
        mAbsoluteCornerRadiusY = minRad * percentage;
    } else {
        mAbsoluteCornerRadiusX = absolutePercentage;
        mAbsoluteCornerRadiusY = absolutePercentage;
    }

    mCornerRadiusPercentage = percentage;
}

void InlineSlider::resizeEvent(QResizeEvent* event)
{
    setupPixmap(event->size());
    setCornerRadius(mCornerRadiusPercentage);

    const auto newValue =  valueFromMappedRange(mSliderValue, 0, event->size().width(), mMin, mMax);
    setSliderPixelPos(newValue);
    update();
}

void InlineSlider::setupPixmap(const QSize& size)
{
    mPixmap = QPixmap(size * devicePixelRatio());
    mPixmap.setDevicePixelRatio(devicePixelRatio());
    mPixmap.fill(Qt::transparent);
}

void InlineSlider::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        setSliderPixelPos(event->localPos().x());
        setSliderValueFromPos(mSliderPos);
        update();
    }
}

void InlineSlider::setValue(qreal newValue)
{
    if (mSliderValue == newValue) { return; }

    const QRect& borderRect = this->borderRect().toAlignedRect();
    setSliderPixelPos(valueFromMappedRange(newValue, borderRect.left(), borderRect.width(), mMin, mMax));
    mSliderValue = qBound(mMin, newValue, mMax);
    mValueLineEditWidget->setValue(mSliderValue);
    update();
}

void InlineSlider::setSliderValueFromPos(qreal pos)
{
    const QRect& borderRect = this->borderRect().toAlignedRect();

    if (mSliderOrigin == SliderStartPosType::MIDDLE) {
        if (qAbs(pos - borderRect.center().x()) <= 0.5) {
            mSliderValue = 0;
        }
    }

    const qreal oldMin = borderRect.left();
    const qreal oldMax = qMax(static_cast<qreal>(borderRect.right() + mCaretWidth), static_cast<qreal>(pos));
    const qreal newMin = mMin;
    const qreal newMax = mMax;

    qreal newValue = valueFromMappedRange(pos, newMin, newMax, oldMin, oldMax);

    mSliderValue = qBound(mMin, newValue, mMax);
    mValueLineEditWidget->setValue(mSliderValue);
    emit valueChanged(mSliderValue);
}

qreal InlineSlider::valueFromMappedRange(qreal value, qreal newMin, qreal newMax, qreal oldMin, qreal oldMax) const
{
    return ((newMax-newMin) * (value - oldMin)) / (oldMax - oldMin) + newMin;
}

void InlineSlider::setSliderPixelPos(qreal pos)
{
    qreal sliderPos = pos;
    const QRect& borderRect = this->borderRect().adjusted(-mCaretWidth, 0, mCaretWidth, 0).toAlignedRect();
    if (sliderPos <= borderRect.left()) {
        sliderPos = borderRect.left();
    } else if (sliderPos >= borderRect.right()) {
        sliderPos = borderRect.right();
    }
    mSliderPos = sliderPos;
}

QRectF InlineSlider::borderRect() const
{
    const QRect& rect = contentsRect();

    qreal left = rect.left() + mBorderWidth;
    qreal top = rect.top() + mBorderWidth;
    qreal width = rect.right() - mBorderWidth;
    qreal height = rect.bottom() - mBorderWidth;

    if (devicePixelRatio() > 1) {
        return QRectF(left, top, width, height);
    } else {
        // For non high DPI scaling,
        // we have to move the coordinate 0.5 pixel to account for anti-aliasing
        // Otherwise certain lines will look blurry
        return QRectF(left + 0.5, top + 0.5, width, height);
    }
}
