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

#include "lineeditwidget.h"

#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>

LineEditNumberWidget::LineEditNumberWidget(QWidget* parent, qreal value)
    : QLineEdit(parent)
{
    setObjectName("LineEditNumberWidget");

    setStyleSheet("LineEditNumberWidget[readOnly=true] {"
                  "background-color: transparent;"
                  "border: 0;"
                  "}");
    setValue(value);
    setReadOnly(true);
}

void LineEditNumberWidget::mousePressEvent(QMouseEvent* event)
{
    QLineEdit::mousePressEvent(event);

    // The event is ignored explicitly
    // so that we can allow it to propergate up the chain
    event->ignore();
}

void LineEditNumberWidget::mouseMoveEvent(QMouseEvent* event)
{
    QLineEdit::mouseMoveEvent(event);

    // The event is ignored explicitly
    // so that we can allow it to propergate up the chain
    event->ignore();
}

void LineEditNumberWidget::mouseReleaseEvent(QMouseEvent* event)
{
    QLineEdit::mouseReleaseEvent(event);

    // The event is ignored explicitly
    // so that we can allow it to propergate up the chain
    event->ignore();
}

void LineEditNumberWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QLineEdit::mouseDoubleClickEvent(event);

    setReadOnly(false);
    setFocus();
    selectAll();

    reloadStylesheet();

    // There's no ignore event here because in this case we want to catch it.
}

void LineEditNumberWidget::focusInEvent(QFocusEvent* event)
{
    QLineEdit::focusInEvent(event);

    if (event->reason() == Qt::TabFocusReason) {
        setReadOnly(false);
        reloadStylesheet();
    }
}

void LineEditNumberWidget::focusOutEvent(QFocusEvent *event)
{
    if (!isReadOnly()) {
        setReadOnly(true);
        reloadStylesheet();
        setProperty(mValueKey, text());
    }

    QLineEdit::focusOutEvent(event);
}

void LineEditNumberWidget::reloadStylesheet()
{
    // Apparently it's good enough to just call setStyleSheet with its current styling
    // to make it update.
    setStyleSheet(styleSheet());
}

void LineEditNumberWidget::setValue(qreal value)
{
    setProperty(mValueKey, value);
    setText(QString::number(value, 'f', 2));
}

qreal LineEditNumberWidget::value() const
{
    return property(mValueKey).toReal();
}

void LineEditNumberWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return) {
        setReadOnly(true);
        setProperty(mValueKey, text());
    } else if (event->key() == Qt::Key_Escape) {
        undo();
        deselect();
    }
    reloadStylesheet();

    QLineEdit::keyPressEvent(event);
}

void LineEditNumberWidget::deselect()
{
    QLineEdit::deselect();
    setReadOnly(true);

    reloadStylesheet();
}
