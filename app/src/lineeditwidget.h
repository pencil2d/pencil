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
#ifndef LINEEDITWIDGET_H
#define LINEEDITWIDGET_H

#include <QObject>
#include <QLineEdit>

class Layer;
class QLabel;
class QLineEdit;
class QStackedLayout;

class LineEditNumberWidget : public QLineEdit
{
    Q_OBJECT
public:
    LineEditNumberWidget(QWidget* parent, qreal value);

    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void setValue(qreal value);
    qreal value() const;

    void deselect();

private:
    // The stylesheet has to be updated on every event
    // where the read-only property is changed
    void reloadStylesheet();

    static constexpr const char* mValueKey = "value";

};

#endif // LINEEDITWIDGET_H
