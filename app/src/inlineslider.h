/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef INLINESLIDER_H
#define INLINESLIDER_H

#include <QWidget>
#include <QLabel>
#include <QPainterPath>
#include <QEvent>

class LineEditNumberWidget;

class InlineSlider : public QWidget
{
    Q_OBJECT
public:

    enum CaretOriginType {
        LEADING,
        MIDDLE
    };

    enum ScaleType {
        LINEAR,
        LOG
    };

    explicit InlineSlider(QWidget* parent);

    void init(const QString& label, qreal min, qreal max);

    void setRange(qreal min, qreal max) { mMin = min; mMax = max; }
    void setMin(qreal min) { mMin = min; }
    void setMax(qreal max) { mMax = max; }

    void setValue(qreal value);
    void showDecimals(bool show);
    void setScaleType(const ScaleType& type) { mScaleType = type; }
    void setCaretOrigin(const CaretOriginType& origin) { mSliderOrigin = origin; }
    void setValuePostFix(QString postfix);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    bool event(QEvent *event) override;

signals:
    void valueChanged(qreal value);

private:
    void setupPixmap(const QSize& size);

    void onLineEditChanged();
    void onScreenChanged(qreal devicePixelRatio);

    /**
     * Calculates how much space there is for the left label vs the line edit text
     * and creates an elided version of the label.
     *
     * @return: A potentially elided label
     */
    QString calculatedDescriptionLabel(const QFontMetrics& metrics);

    /**
     * Calculates the carets pixel position based on the input slider value
     * @param sliderValue
     * @return the pixel position of the caret
     */
    qreal calculatedPixelPos(qreal sliderValue) const;
    QRectF calculatedContentsRect() const;

    void setSliderPixelPos(qreal pos);
    void setSliderValueFromPos(qreal pos);
    void setCornerRadius(qreal percentage);

    void drawSlider(const QRect& blitRect);
    void drawLeadingLabel(QPainter& painter, const QRectF& borderRect, const QColor& textColor);
    void drawCaret(QPainter& painter, const QRectF& borderRect, const QColor& caretColor);

    void updateLineEditStylesheet();

    QString mLabel;
    QPixmap mPixmap;

    qreal mMin = 0.0;
    qreal mMax = 0.0;

    int mBorderWidth = 1;

    qreal mCornerRadiusPercentage = 0.2;
    qreal mAbsoluteCornerRadiusX = 0;
    qreal mAbsoluteCornerRadiusY = 0;

    qreal mSliderValue = 0.0;
    qreal mSliderPos = 0.0;

    qreal mCaretWidth = 1.0;
    qreal mTextPadding = 6;

    qreal mCachedElidedLabelWidth = 0.0;
    QString mCachedElidedDescriptionLabel = "";

    ScaleType mScaleType = ScaleType::LINEAR;
    CaretOriginType mSliderOrigin = CaretOriginType::LEADING;

    LineEditNumberWidget* mValueLineEditWidget = nullptr;
};

#endif // INLINESLIDER_H
