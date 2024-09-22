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
                  "border: 0;"
                  "}");
    setText(text);
    setReadOnly(true);
}

void LineEditWidget::mousePressEvent(QMouseEvent* event)
{
    QLineEdit::mousePressEvent(event);

    // The event is ignored explicitly
    // so that we can allow it to propergate up the chain
    event->ignore();
}

void LineEditWidget::mouseMoveEvent(QMouseEvent* event)
{
    QLineEdit::mouseMoveEvent(event);

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
}

void LineEditWidget::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    setReadOnly(true);
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
    QLineEdit::keyPressEvent(event);

    if (event->key() == Qt::Key_Return) {
        setReadOnly(true);
    } else if (event->key() == Qt::Key_Escape) {
        undo();
        setReadOnly(true);
    }
    reloadStylesheet();
}

void LineEditWidget::deselect()
{
    QLineEdit::deselect();
    setReadOnly(true);

    reloadStylesheet();
}
