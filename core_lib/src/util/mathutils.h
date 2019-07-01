#ifndef MATHUTILS_H
#define MATHUTILS_H

#define M_PI 3.14159265358979323846264338327950288

namespace MathUtils
{
    inline double radToDeg(const double& radians)
    {
        return radians * 180.0 / M_PI;
    }
}

#endif // MATHUTILS_H
