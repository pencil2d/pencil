#include "keyframetextedit.h"

KeyFrameTextEdit::KeyFrameTextEdit(QWidget* parent) : QPlainTextEdit(parent) {

}

void KeyFrameTextEdit::focusOutEvent(QFocusEvent *event) {

    if (event->lostFocus()) {
        clearFocus();
    }
    QPlainTextEdit::focusOutEvent(event);
}

bool KeyFrameTextEdit::event(QEvent* event)
{
    if (event->type() == QEvent::Leave) {
        emit outsideWidget();
    }

    return QPlainTextEdit::event(event);
}
