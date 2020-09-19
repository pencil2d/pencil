/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef BACKUPELEMENT_H
#define BACKUPELEMENT_H

#include <QObject>
#include "vectorimage.h"
#include "bitmapimage.h"
#include "soundclip.h"

class Editor;

class BackupElement : public QObject
{
    Q_OBJECT
public:
    enum types { UNDEFINED, BITMAP_MODIF, VECTOR_MODIF, SOUND_MODIF };

    QString undoText;
    bool somethingSelected = false;
    qreal rotationAngle = 0.0;
    QRectF mySelection, myTransformedSelection, myTempTransformedSelection;

    virtual int type() { return UNDEFINED; }
    virtual void restore(Editor*) { Q_ASSERT(false); }
};

class BackupBitmapElement : public BackupElement
{
    Q_OBJECT
public:
    BackupBitmapElement(BitmapImage* bi) { bitmapImage = bi->copy(); }

    int layer = 0;
    int frame = 0;
    BitmapImage bitmapImage;
    int type() { return BackupElement::BITMAP_MODIF; }
    void restore(Editor*);
};

class BackupVectorElement : public BackupElement
{
    Q_OBJECT
public:
    BackupVectorElement(VectorImage* vi) { vectorImage = *vi; }
    int layer = 0;
    int frame = 0;
    VectorImage vectorImage;

    int type() { return BackupElement::VECTOR_MODIF; }
    void restore(Editor*);
};

class BackupSoundElement : public BackupElement
{
    Q_OBJECT
public:
    BackupSoundElement(SoundClip* sound) { clip = *sound; }
    int layer = 0;
    int frame = 0;
    SoundClip clip;
    QString fileName;

    int type() { return BackupElement::SOUND_MODIF; }
    void restore( Editor* );
};

#endif // BACKUPELEMENT_H
