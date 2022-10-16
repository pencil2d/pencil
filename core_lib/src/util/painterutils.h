#ifndef PAINTERUTILS_H
#define PAINTERUTILS_H

#include <QtMath>

/** Calculate layer opacity based on current layer offset */
inline qreal calculateRelativeOpacityForLayer(int currentLayerIndex, int layerIndexNext, float threshold)
{
    int layerOffset = currentLayerIndex - layerIndexNext;
    int absoluteOffset = qAbs(layerOffset);
    qreal newOpacity = 1.0;
    if (absoluteOffset != 0)
    {
        newOpacity = qPow(static_cast<qreal>(threshold), absoluteOffset);
    }
    return newOpacity;
};

#endif // PAINTERUTILS_H
