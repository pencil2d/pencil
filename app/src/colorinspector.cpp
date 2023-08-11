/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "colorinspector.h"
#include "ui_colorinspector.h"

#include <QSettings>
#include <QDebug>

#include "colorslider.h"
#include "pencildef.h"
#include "editor.h"
#include "colormanager.h"
#include "util.h"


ColorInspector::ColorInspector(QWidget *parent) :
    BaseDockWidget(parent)
{
    QWidget* innerWidget = new QWidget;
    setWindowTitle(tr("Color Inspector", "Window title of color inspector"));

    ui = new Ui::ColorInspector;
    ui->setupUi(innerWidget);
    setWidget(innerWidget);
}

ColorInspector::~ColorInspector()
{
    delete ui;
}

void ColorInspector::initUI()
{
    mCurrentColor = editor()->color()->frontColor();

    QSettings settings(PENCIL2D, PENCIL2D);
    isRgbColors = settings.value("isRgb").toBool();

    if (isRgbColors) {
        ui->colorSpecTabWidget->setCurrentWidget(ui->rgbTab);
    } else {
        ui->colorSpecTabWidget->setCurrentWidget(ui->hsvTab);
    }
    onColorSpecChanged();

    ui->redSlider->init(ColorSlider::ColorSpecType::RGB, ColorSlider::ColorType::RED, mCurrentColor, 0.0, 255.0);
    ui->greenSlider->init(ColorSlider::ColorSpecType::RGB, ColorSlider::ColorType::GREEN, mCurrentColor, 0.0, 255.0);
    ui->blueSlider->init(ColorSlider::ColorSpecType::RGB, ColorSlider::ColorType::BLUE, mCurrentColor, 0.0, 255.0);
    ui->rgbAlphaSlider->init(ColorSlider::ColorSpecType::RGB, ColorSlider::ColorType::ALPHA, mCurrentColor, 0.0, 255.0);

    ui->hueSlider->init(ColorSlider::ColorSpecType::HSV, ColorSlider::ColorType::HUE, mCurrentColor, 0.0, 359.0);
    ui->saturationSlider->init(ColorSlider::ColorSpecType::HSV, ColorSlider::ColorType::SAT, mCurrentColor, 0.0, 255.0);
    ui->valueSlider->init(ColorSlider::ColorSpecType::HSV, ColorSlider::ColorType::VAL, mCurrentColor, 0.0, 255.0);
    ui->hsvAlphaSlider->init(ColorSlider::ColorSpecType::HSV, ColorSlider::ColorType::ALPHA, mCurrentColor, 0.0, 255.0);

    QPalette p1 = ui->colorWrapper->palette();
    p1.setBrush(QPalette::Window, QBrush(QImage(":/background/checkerboard.png")));
    ui->colorWrapper->setPalette(p1);

    QPalette p2 = ui->color->palette();
    p2.setColor(QPalette::Window, mCurrentColor);
    ui->color->setPalette(p2);

    connect(ui->colorSpecTabWidget, &QTabWidget::currentChanged, this, &ColorInspector::onColorSpecChanged);

    auto onColorChangedSlider = static_cast<void(ColorInspector::*)(const QColor&)>(&ColorInspector::onColorChanged);
    connect(ui->redSlider, &ColorSlider::valueChanged, this, onColorChangedSlider);
    connect(ui->greenSlider, &ColorSlider::valueChanged, this, onColorChangedSlider);
    connect(ui->blueSlider, &ColorSlider::valueChanged, this, onColorChangedSlider);
    connect(ui->rgbAlphaSlider, &ColorSlider::valueChanged, this, onColorChangedSlider);
    connect(ui->hueSlider, &ColorSlider::valueChanged, this, onColorChangedSlider);
    connect(ui->saturationSlider, &ColorSlider::valueChanged, this, onColorChangedSlider);
    connect(ui->valueSlider, &ColorSlider::valueChanged, this, onColorChangedSlider);
    connect(ui->hsvAlphaSlider, &ColorSlider::valueChanged, this, onColorChangedSlider);

    auto spinBoxChanged = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    auto onColorChangedSpinBox = static_cast<void(ColorInspector::*)()>(&ColorInspector::onColorChanged);
    connect(ui->redSpinBox, spinBoxChanged, this, onColorChangedSpinBox);
    clearFocusOnFinished(ui->redSpinBox);
    connect(ui->greenSpinBox, spinBoxChanged, this, onColorChangedSpinBox);
    clearFocusOnFinished(ui->greenSpinBox);
    connect(ui->blueSpinBox, spinBoxChanged, this, onColorChangedSpinBox);
    clearFocusOnFinished(ui->blueSpinBox);
    connect(ui->rgbAlphaSpinBox, spinBoxChanged, this, onColorChangedSpinBox);
    clearFocusOnFinished(ui->rgbAlphaSpinBox);
    connect(ui->hueSpinBox, spinBoxChanged, this, onColorChangedSpinBox);
    clearFocusOnFinished(ui->hueSpinBox);
    connect(ui->saturationSpinBox, spinBoxChanged, this, onColorChangedSpinBox);
    clearFocusOnFinished(ui->saturationSpinBox);
    connect(ui->valueSpinBox, spinBoxChanged, this, onColorChangedSpinBox);
    clearFocusOnFinished(ui->valueSpinBox);
    connect(ui->hsvAlphaSpinBox, spinBoxChanged, this, onColorChangedSpinBox);
    clearFocusOnFinished(ui->hsvAlphaSpinBox);

    connect(editor(), &Editor::objectLoaded, this, &ColorInspector::updateUI);
}

void ColorInspector::updateUI()
{
    QColor newColor = editor()->color()->frontColor();
    setColor(newColor);
}

void ColorInspector::setColor(QColor newColor)
{
    // compare under the same color spec
    newColor = (isRgbColors) ? newColor.toRgb() : newColor.toHsv();

    if (newColor == mCurrentColor)
    {
        return;
    }

    mCurrentColor = newColor;

    updateControls();
}

QColor ColorInspector::color()
{
    return mCurrentColor;
}

void ColorInspector::updateControls()
{
    // this is a UI update function, never emit any signals
    // grab the color from color manager, and then update itself, that's it.

    QSignalBlocker b1(ui->redSlider);
    QSignalBlocker b2(ui->greenSlider);
    QSignalBlocker b3(ui->blueSlider);
    QSignalBlocker b4(ui->rgbAlphaSlider);

    ui->redSlider->setRgb(mCurrentColor);
    ui->greenSlider->setRgb(mCurrentColor);
    ui->blueSlider->setRgb(mCurrentColor);
    ui->rgbAlphaSlider->setRgb(mCurrentColor);

    QSignalBlocker b5(ui->redSpinBox);
    QSignalBlocker b6(ui->greenSpinBox);
    QSignalBlocker b7(ui->blueSpinBox);
    QSignalBlocker b8(ui->rgbAlphaSpinBox);

    ui->redSpinBox->setValue(mCurrentColor.red());
    ui->greenSpinBox->setValue(mCurrentColor.green());
    ui->blueSpinBox->setValue(mCurrentColor.blue());
    ui->rgbAlphaSpinBox->setValue(mCurrentColor.alpha());

    QSignalBlocker b9(ui->hueSlider);
    QSignalBlocker b10(ui->saturationSlider);
    QSignalBlocker b11(ui->valueSlider);
    QSignalBlocker b12(ui->hsvAlphaSlider);

    ui->hueSlider->setHsv(mCurrentColor);
    ui->saturationSlider->setHsv(mCurrentColor);
    ui->valueSlider->setHsv(mCurrentColor);
    ui->hsvAlphaSlider->setHsv(mCurrentColor);

    QSignalBlocker b13(ui->hueSpinBox);
    QSignalBlocker b14(ui->saturationSpinBox);
    QSignalBlocker b15(ui->valueSpinBox);
    QSignalBlocker b16(ui->hsvAlphaSpinBox);

    ui->hueSpinBox->setValue(mCurrentColor.hue());
    ui->saturationSpinBox->setValue(qRound(mCurrentColor.saturation() / 2.55));
    ui->valueSpinBox->setValue(qRound(mCurrentColor.value() / 2.55));
    ui->hsvAlphaSpinBox->setValue(qRound(mCurrentColor.alpha() / 2.55));

    QPalette p = ui->color->palette();
    p.setColor(QPalette::Window, mCurrentColor);
    ui->color->setPalette(p);

    update();
}

void ColorInspector::onColorSpecChanged()
{
    // assume hsv if not checked
    isRgbColors = ui->colorSpecTabWidget->currentWidget() == ui->rgbTab;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("isRgb", isRgbColors);

    if (isRgbColors)
    {
        mCurrentColor = mCurrentColor.toRgb();
    }
    else
    {
        mCurrentColor = mCurrentColor.toHsv();
    }

    updateControls();

    emit colorSpecChanged(isRgbColors);
}

void ColorInspector::onColorChanged()
{
    if (isRgbColors) {
        mCurrentColor.setRgb(
            ui->redSpinBox->value(),
            ui->greenSpinBox->value(),
            ui->blueSpinBox->value(),
            ui->rgbAlphaSpinBox->value());
    } else {
        mCurrentColor.setHsv(
            ui->hueSpinBox->value(),
            static_cast<int>(ui->saturationSpinBox->value() * 2.55),
            static_cast<int>(ui->valueSpinBox->value() * 2.55),
            static_cast<int>(ui->hsvAlphaSpinBox->value() * 2.55));
    }

    updateControls();

    emit colorChanged(mCurrentColor);
}

void ColorInspector::onColorChanged(const QColor& color)
{
    mCurrentColor = color;

    updateControls();

    emit colorChanged(color);
}
