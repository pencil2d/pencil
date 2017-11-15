/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "keycapturelineedit.h"

#include <QKeyEvent>
#include <QDebug>


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

    this->setText(strKeySeq);
    emit keyCaptured(keySeq);
}
