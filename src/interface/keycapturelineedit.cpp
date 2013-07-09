
#include <QKeyEvent>
#include <QDebug>
#include "keycapturelineedit.h"



KeyCaptureLineEdit::KeyCaptureLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void KeyCaptureLineEdit::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control ||
        event->key() == Qt::Key_Shift   ||
        event->key() == Qt::Key_Alt     ||
        event->key() == Qt::Key_Meta )
    {
        // only modifier key is not allowed.
        qDebug() << "only!";
        event->accept();
        return;
    }

    int keyInt = event->key();

    if (event->modifiers() & Qt::CTRL)
    {
        keyInt += Qt::CTRL;
    }
    if (event->modifiers() & Qt::SHIFT)
    {
        keyInt += Qt::SHIFT;
    }
    if (event->modifiers() & Qt::ALT)
    {
        keyInt += Qt::ALT;
    }
    if (event->modifiers() & Qt::META)
    {
        keyInt += Qt::META;
    }

    QKeySequence keySeq(keyInt);
    QString strKeySeq = keySeq.toString(QKeySequence::NativeText);

    emit keyCaptured(keySeq);

    this->setText(strKeySeq);
}
