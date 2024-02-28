#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <QtMath>
#include <QPoint>

namespace MathUtils
{
    /** Get the angle from the difference vector a->b to the x-axis.
     *
     * \param a Start point of vector
     * \param b End point of vector
     * \return Angle in radians from [-pi,+pi]
     */
    inline qreal getDifferenceAngle(const QPointF a, const QPointF b)
    {
        return qAtan2(b.y() - a.y(), b.x() - a.x());
    }

    /** Map one range onto another
     *  \param x The input value
     *  \param inputMin The input min value
     *  \param inputMax The input max value
     *  \param outputMin The output min value
     *  \param outputMax The output max value
     *  \return The value of x mapped to the corresponding range between outputMin and outputMax
     */
    inline qreal map(qreal x, qreal inputMin, qreal inputMax, qreal outputMin, qreal outputMax)
    {
        qreal slope = (outputMax - outputMin) / (inputMax - inputMin);
        return outputMin + slope * (x - inputMin);
    }

    /** Normalize x to a value between 0 and 1;
    *  \param x The input value
    *  \param min The input min value
    *  \param max The input max value
    *  \return The value of x normalized to a range between 0 and 1
    */
    inline qreal normalize(qreal x, qreal min, qreal max)
    {
        return qAbs((x - max) / (min - max));
    }
}

#endif // MATHUTILS_H
