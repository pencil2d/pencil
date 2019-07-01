#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <QtMath>
#include <QPoint>

namespace MathUtils
{
    /** Convert angles from radians to degrees.
     *
     * \param radians Angle in radians.
     * \return Angle in degrees.
     */
    inline qreal radToDeg(const qreal radians)
    {
        return radians * 180.0 / M_PI;
    }

    /** Convert angles from degrees to radians.
     *
     * \param degrees Angle in degrees.
     * \return Angle in radians.
     */
    inline qreal degToRad(const qreal degrees)
    {
        return degrees * M_PI / 180.0;
    }

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
}

#endif // MATHUTILS_H
