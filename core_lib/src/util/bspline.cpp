/***************************************************************************
 *   Copyright (C) 2009 by Anton R. <commanderkyle@gmail.com>              *
 *                                                                         *
 *   This file is a part of QAquarelle project.                            *
 *                                                                         *
 *   QAquarelle is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "bspline.h"

static const float B[3][3] = {
    {+1.0f, -2.0f, +1.0f},
    {-2.0f, +2.0f, +0.0f},
    {+1.0f, +1.0f, +0.0f}
};

/**
 * http://en.wikipedia.org/wiki/B-spline
 * =)
 */
void BSpline::interpolate_quad (int * x, int * y, float t) {
    const float t_vec[] = {t * t, t, 1.0f};
    float r_vec[3];

    r_vec[0] = t_vec[0] * B[0][0] + t_vec[1] * B[1][0] + t_vec[2] * B[2][0];
    r_vec[1] = t_vec[0] * B[0][1] + t_vec[1] * B[1][1] + t_vec[2] * B[2][1];
    r_vec[2] = t_vec[0] * B[0][2] + t_vec[1] * B[1][2] + t_vec[2] * B[2][2];
    r_vec[0] *= 0.5;
    r_vec[1] *= 0.5;
    r_vec[2] *= 0.5;

    x[3] = (int)(r_vec[0] * x[0] + r_vec[1] * x[1] + r_vec[2] * x[2]);
    y[3] = (int)(r_vec[0] * y[0] + r_vec[1] * y[1] + r_vec[2] * y[2]);
}

/**
 * http://en.wikipedia.org/wiki/B-spline
 * =)
 */
void BSpline::interpolate_quad (float * x, float * y, float t) {
    const float t_vec[] = {t * t, t, 1.0f};
    float r_vec[3];

    r_vec[0] = t_vec[0] * B[0][0] + t_vec[1] * B[1][0] + t_vec[2] * B[2][0];
    r_vec[1] = t_vec[0] * B[0][1] + t_vec[1] * B[1][1] + t_vec[2] * B[2][1];
    r_vec[2] = t_vec[0] * B[0][2] + t_vec[1] * B[1][2] + t_vec[2] * B[2][2];
    r_vec[0] *= 0.5;
    r_vec[1] *= 0.5;
    r_vec[2] *= 0.5;

    x[3] = r_vec[0] * x[0] + r_vec[1] * x[1] + r_vec[2] * x[2];
    y[3] = r_vec[0] * y[0] + r_vec[1] * y[1] + r_vec[2] * y[2];
}

