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

#ifndef KEYCAPTURELINEEDIT_H
#define KEYCAPTURELINEEDIT_H

#include <QLineEdit>
#include <QKeySequence>

class QKeyEvent;


class KeyCaptureLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit KeyCaptureLineEdit(QWidget* parent = 0);

signals:
    void keyCaptured(QKeySequence seq);

public slots:

protected:
    void keyPressEvent(QKeyEvent *);
};

#endif // KEYCAPTURELINEEDIT_H
