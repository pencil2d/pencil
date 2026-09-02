/*
Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2012-2020 Matthew Chiawen Chang
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#ifndef LAYERVISIBILITYBUTTON_H
#define LAYERVISIBILITYBUTTON_H

#include <QObject>
#include <QWidget>

class Layer;
class QIcon;
class QToolButton;
class Editor;
class QPixmap;

enum class LayerVisibilityContext
{
    LOCAL,
    GLOBAL
};

class LayerVisibilityButton : public QWidget
{
    Q_OBJECT
public:
    LayerVisibilityButton(QWidget* parent,
                          const LayerVisibilityContext& visibilityContext,
                          Layer* layer,
                          Editor* editor);

    void paintEvent(QPaintEvent* event) override;

    void paintLocalVisibilityState(QPainter& painter, const QPalette& palette);
    void paintGlobalVisibilityState(QPainter& painter, const QPalette& palette);

    void mousePressEvent(QMouseEvent* event) override;

    QSize sizeHint() const override;

signals:
    void visibilityChanged();

private:

    Layer* mLayer = nullptr;
    Editor* mEditor = nullptr;
    QSize mVisibilityCircleSize = QSize(10,10);

    bool mIconValid = false;

    const LayerVisibilityContext mVisibilityContext;

};

#endif // LAYERVISIBILITYBUTTON_H
