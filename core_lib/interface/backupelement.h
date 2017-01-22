#ifndef BACKUPELEMENT_H
#define BACKUPELEMENT_H

#include <QObject>
#include "vectorimage.h"
#include "bitmapimage.h"


class Editor;

class BackupElement : public QObject
{
    Q_OBJECT
public:
    enum types { UNDEFINED, BITMAP_MODIF, VECTOR_MODIF };

    QString undoText;
    bool somethingSelected;
    QRectF mySelection, myTransformedSelection, myTempTransformedSelection;

    virtual int type() { return UNDEFINED; }
    virtual void restore(Editor*) { qDebug() << "Wrong"; }
};

class BackupBitmapElement : public BackupElement
{
    Q_OBJECT
public:
    BackupBitmapElement(BitmapImage* bi) { bitmapImage = bi->copy(); }

    int layer, frame;
    BitmapImage bitmapImage;
    //BackupBitmapElement() { type = BackupElement::BITMAP_MODIF; }
    int type() { return BackupElement::BITMAP_MODIF; }
    void restore(Editor*);
};

class BackupVectorElement : public BackupElement
{
    Q_OBJECT
public:
    BackupVectorElement(VectorImage* vi) { vectorImage = *vi; }
    int layer, frame;
    VectorImage vectorImage;

    int type() { return BackupElement::VECTOR_MODIF; }
    void restore(Editor*);
};

#endif // BACKUPELEMENT_H
