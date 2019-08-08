/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef COLORSPINBOXGROUP_H
#define COLORSPINBOXGROUP_H

#include <memory>
#include <QPaintEvent>
#include <QList>
#include <QMenu>
#include <QSignalMapper>
#include "basedockwidget.h"

namespace Ui {
class ColorInspector;
}

class ColorInspector : public BaseDockWidget
{
    Q_OBJECT

    friend class ColorSliders;
    
public:
    explicit ColorInspector(QWidget *parent = nullptr);
    ~ColorInspector() override;
    QColor color();

    void initUI() override;
    void updateUI() override;

    void updateLastColorButton(QColor *color = nullptr);
    bool eventFilter(QObject *target, QEvent *event);
    void setLastColorShortcut(QKeySequence keySequence);

protected:
    void paintEvent(QPaintEvent *) override;

signals:
    void colorChanged(const QColor& c);
    void modeChange(const bool& isRgb);

public slots:
    void setColor(QColor newColor);
    void saveColor();

private slots:
    void onModeChanged();
    void onColorChanged();
    void onSliderChanged(QColor color);

    void lastColorButtonClicked();
    void onMoreColorButtonClicked();
    void useOldColor(int index);

private:

    Ui::ColorInspector* ui = nullptr;
    bool isRgbColors = true;
    QColor mCurrentColor;

    // for color history
    QList<QColor> mOldColors;
    QSize mLastColorSize;
    bool isColorUsed = false;
    std::shared_ptr<QMenu> mColorMenu;
    std::shared_ptr<QSignalMapper> mSignalMap;
    QList<std::shared_ptr<QAction>> mCtxActions;
};

#endif // COLORSPINBOXGROUP_H
