/*

Pencil2D - Traditional Animation Software
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

#ifndef LEGACYBACKUPELEMENT_H
#define LEGACYBACKUPELEMENT_H

#include <QObject>
#include "vectorimage.h"
#include "bitmapimage.h"
#include "soundclip.h"

class Editor;

class LegacyBackupElement : public QObject
{
    Q_OBJECT
public:
    enum types { UNDEFINED, BITMAP_MODIF, VECTOR_MODIF, SOUND_MODIF };

    QString undoText;
    bool somethingSelected = false;
    qreal rotationAngle = 0.0;
    qreal scaleX = 1.0;
    qreal scaleY = 1.0;
    QPointF translation;
    QRectF mySelection;
    QPointF selectionAnchor;

    virtual int type() { return UNDEFINED; }
    virtual void restore(Editor*) { Q_ASSERT(false); }
};

class BackupLegacyBitmapElement : public LegacyBackupElement
{
    Q_OBJECT
public:
    explicit BackupLegacyBitmapElement(BitmapImage* bi) { bitmapImage = *bi; }

    int layerId = 0;

    int layer = 0;
    int frame = 0;
    BitmapImage bitmapImage;
    int type() override { return LegacyBackupElement::BITMAP_MODIF; }
    void restore(Editor*) override;
};

class BackupLegacyVectorElement : public LegacyBackupElement
{
    Q_OBJECT
public:
    explicit BackupLegacyVectorElement(VectorImage* vi) { vectorImage = *vi; }
    int layerId = 0;

    int layer = 0;
    int frame = 0;
    VectorImage vectorImage;

    int type() override { return LegacyBackupElement::VECTOR_MODIF; }
    void restore(Editor*) override;
};

class BackupLegacySoundElement : public LegacyBackupElement
{
    Q_OBJECT
public:
    explicit BackupLegacySoundElement(SoundClip* sound) { clip = *sound; }
    int layerId = 0;

    int layer = 0;
    int frame = 0;
    SoundClip clip;
    QString fileName, originalName;

    int type() override { return LegacyBackupElement::SOUND_MODIF; }
    void restore( Editor* ) override;
};

#endif // LEGACYBACKUPELEMENT_H
