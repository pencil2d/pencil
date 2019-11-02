#ifndef LAYERMULTIPLANECAMERA_H
#define LAYERMULTIPLANECAMERA_H

#include "layer.h"

struct Campoint {
    int frame;
    QTransform transform;
    QPainterPath path;
};

class LayerMultiPlaneCamera : public Layer
{
    Q_OBJECT

public:
    LayerMultiPlaneCamera(Object* object);

    void setViewRect(QRect viewrect) { mViewRect = viewrect; }
    QRect getViewRect() { return mViewRect; }

private:
    QList<Campoint> mCampointList;
    QRect mViewRect;
};

#endif // LAYERMULTIPLANECAMERA_H
/*
Hver frame har sin egen Campoint.
DVS:
Frame 1 har altid et Campoint. Er der ikke nogen kamerabevægelser, bruges
Campoint fra frame 1 i hele scenen.
Laver man en pan fra frame 50 til 80 i en 300 frames lang scene vil følgende ske:
Frame 1 har et Campoint.
Idet frame 50 tildeles en transform, udregnes transforms for frame 1 til 50. Hvis
der ikke er forskel på fielden i frame 1 og 50, har alle 50 samme transform.
Idet frame 80 får en transform sker det samme. 50 til 80 udregnes. Når frame
80 ændres, vil 50 til 80 igen blive beregnet.
*/
