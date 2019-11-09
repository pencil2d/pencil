#include "keyframetextedit.h"

KeyFrameTextEdit::KeyFrameTextEdit(QWidget* parent) : QPlainTextEdit(parent) {

}

void KeyFrameTextEdit::focusOutEvent(QFocusEvent *event) {

    if (event->lostFocus()) {
        emit(lostFocus());
        clearFocus();
    }
    QPlainTextEdit::focusOutEvent(event);
}
