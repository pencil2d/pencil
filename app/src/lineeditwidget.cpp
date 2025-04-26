/*

Pencil2D - Traditional Animation Software
Copyright (C) 2025 Oliver S. Larsen

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

LineEditWidget::LineEditWidget(QWidget* parent, QString text)
    : QLineEdit(parent)
{
    setObjectName("LineEditWidget");

    setStyleSheet("LineEditWidget[readOnly=true] {"
                  "background-color: transparent;"
                  "selection-color: palette(bright-text);"
                  "selection-background-color: palette(dark);"
                  "border: none;"
                  "}");
    setText(text);
    setReadOnly(true);
}

void LineEditWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (isReadOnly()) {
        // No context menu is shown while
        // we're in read only mode.
        return;
    }

    QLineEdit::contextMenuEvent(event);
}

void LineEditWidget::mousePressEvent(QMouseEvent* event)
{
    QLineEdit::mousePressEvent(event);

    const QPoint& parentPos = mapToParent(event->pos());
    if (this->geometry().contains(parentPos) && !isReadOnly()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void LineEditWidget::mouseMoveEvent(QMouseEvent* event)
{
    QLineEdit::mouseMoveEvent(event);

    if (isReadOnly()) {
        // While we're not editing, we don't
        // allow selecting the text.
        setSelection(0,0);
    }
    // The event is ignored explicitly
    // so that we can allow it to propergate up the chain
    event->ignore();
}

void LineEditWidget::mouseReleaseEvent(QMouseEvent* event)
{
    QLineEdit::mouseReleaseEvent(event);

    // The event is ignored explicitly
    // so that we can allow it to propergate up the chain
    event->ignore();
}

void LineEditWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QLineEdit::mouseDoubleClickEvent(event);

    setReadOnly(false);
    setFocus();
    selectAll();

    reloadStylesheet();

    // There's no ignore event here because in this case we want to catch it.
    // The event is ignored explicitly
    // so that we can allow it to propergate up the chain
    event->accept();
}

void LineEditWidget::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);

    if (!geometry().contains(mapToParent(mapFromGlobal(QCursor::pos())))) {
        // If we're clicking outside the widget, set the widget back to read only.
        setReadOnly(true);
    }

    reloadStylesheet();
}

void LineEditWidget::reloadStylesheet()
{
    // Apparently it's good enough to just call setStyleSheet with its current styling
    // to make it update.
    setStyleSheet(styleSheet());
}

void LineEditWidget::keyPressEvent(QKeyEvent* event)
{
    // We need to get the readonly value here because
    // QLineEdit othewise negates our intended behavour.
    bool toggle = isReadOnly();

    QLineEdit::keyPressEvent(event);

    bool eventAccepted = false;
    if (event->key() == Qt::Key_Return) {
        setReadOnly(!toggle);
        reloadStylesheet();
        eventAccepted = true;
    } else if (event->key() == Qt::Key_Escape) {
        undo();
        setReadOnly(true);
        reloadStylesheet();
        eventAccepted = true;
    }

    event->setAccepted(eventAccepted);
}

void LineEditWidget::deselect()
{
    QLineEdit::deselect();
    setReadOnly(true);

    reloadStylesheet();
}
