#ifndef BSPLINE_H
#define BSPLINE_H

class BSpline
{
public:
    static void interpolate_quad (int   * x, int   * y, float t);
    static void interpolate_quad (float * x, float * y, float t);
};

#endif // BSPLINE_H
