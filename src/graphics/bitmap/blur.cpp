/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <QtGui>
#include <math.h>
#include "blur.h"

Blur::Blur()
{
    // nothing
}


// Exponential blur, Jani Huhtanen, 2006
//
static inline void blurinner(unsigned char* bptr, int& zR, int& zG, int& zB, int& zA, int alpha, int aprec, int zprec);

static inline void blurrow( QImage& im, int line, int alpha, int aprec, int zprec);

static inline void blurcol( QImage& im, int col, int alpha, int aprec, int zprec);

/*
*  expblur(QImage &img, int radius)
*
*  In-place blur of image 'img' with kernel
*  of approximate radius 'radius'.
*
*  Blurs with two sided exponential impulse
*  response.
*
*  aprec = precision of alpha parameter
*  in fixed-point format 0.aprec
*
*  zprec = precision of state parameters
*  zR,zG,zB and zA in fp format 8.zprec
*/
void Blur::expblur( QImage& img, int radius, int aprec,int zprec )
{
    if(radius<1)
        return;

    /* Calculate the alpha such that 90% of
       the kernel is within the radius.
       (Kernel extends to infinity)
    */
    int alpha = (int)((1<<aprec)*(1.0f-expf(-2.3f/(radius+1.f))));

    for(int row=0; row<img.height(); row++)
    {
        blurrow(img,row,alpha,aprec,zprec);
    }

    for(int col=0; col<img.width(); col++)
    {
        blurcol(img,col,alpha,aprec,zprec);
    }
    return;
}

static inline void blurinner(unsigned char* bptr, int& zR, int& zG, int& zB, int& zA, int alpha, int aprec, int zprec)
{
    int R,G,B,A;
    R = *bptr;
    G = *(bptr+1);
    B = *(bptr+2);
    A = *(bptr+3);

    zR += (alpha * ((R<<zprec)-zR))>>aprec;
    zG += (alpha * ((G<<zprec)-zG))>>aprec;
    zB += (alpha * ((B<<zprec)-zB))>>aprec;
    zA += (alpha * ((A<<zprec)-zA))>>aprec;

    *bptr =     zR>>zprec;
    *(bptr+1) = zG>>zprec;
    *(bptr+2) = zB>>zprec;
    *(bptr+3) = zA>>zprec;
}

static inline void blurrow( QImage& im, int line, int alpha, int aprec, int zprec)
{
    int zR,zG,zB,zA;

    QRgb* ptr = (QRgb*)im.scanLine(line);

    zR = *((unsigned char*)ptr    )<<zprec;
    zG = *((unsigned char*)ptr + 1)<<zprec;
    zB = *((unsigned char*)ptr + 2)<<zprec;
    zA = *((unsigned char*)ptr + 3)<<zprec;

    for(int index=1; index<im.width(); index++)
    {
        blurinner((unsigned char*)&ptr[index],zR,zG,zB,zA,alpha,aprec,zprec);
    }
    for(int index=im.width()-2; index>=0; index--)
    {
        blurinner((unsigned char*)&ptr[index],zR,zG,zB,zA,alpha,aprec,zprec);
    }


}

static inline void blurcol( QImage& im, int col, int alpha, int aprec, int zprec)
{
    int zR,zG,zB,zA;

    QRgb* ptr = (QRgb*)im.bits();
    ptr+=col;

    zR = *((unsigned char*)ptr    )<<zprec;
    zG = *((unsigned char*)ptr + 1)<<zprec;
    zB = *((unsigned char*)ptr + 2)<<zprec;
    zA = *((unsigned char*)ptr + 3)<<zprec;

    for(int index=im.width(); index<(im.height()-1)*im.width(); index+=im.width())
    {
        blurinner((unsigned char*)&ptr[index],zR,zG,zB,zA,alpha,aprec,zprec);
    }

    for(int index=(im.height()-2)*im.width(); index>=0; index-=im.width())
    {
        blurinner((unsigned char*)&ptr[index],zR,zG,zB,zA,alpha,aprec,zprec);
    }

}



// Stack Blur Algorithm by Mario Klingemann <mario@quasimondo.com>
void Blur::fastbluralpha(QImage& img, int radius)
{
    if (radius < 1)
    {
        return;
    }

    QRgb* pix = (QRgb*)img.bits();
    int w   = img.width();
    int h   = img.height();
    int wm  = w-1;
    int hm  = h-1;
    int wh  = w*h;
    int div = radius+radius+1;

    int* r = new int[wh];
    int* g = new int[wh];
    int* b = new int[wh];
    int* a = new int[wh];
    int rsum, gsum, bsum, asum, x, y, i, yp, yi, yw;
    QRgb p;
    int* vmin = new int[qMax(w,h)];

    int divsum = (div+1)>>1;
    divsum *= divsum;
    int* dv = new int[256*divsum];
    for (i=0; i < 256*divsum; ++i)
    {
        dv[i] = (i/divsum);
    }

    yw = yi = 0;

    int** stack = new int*[div];
    for(int i = 0; i < div; ++i)
    {
        stack[i] = new int[4];
    }


    int stackpointer;
    int stackstart;
    int* sir;
    int rbs;
    int r1 = radius+1;
    int routsum, goutsum, boutsum, aoutsum;
    int rinsum, ginsum, binsum, ainsum;

    for (y = 0; y < h; ++y)
    {
        rinsum = ginsum = binsum = ainsum
                                   = routsum = goutsum = boutsum = aoutsum
                                               = rsum = gsum = bsum = asum = 0;
        for(i =- radius; i <= radius; ++i)
        {
            p = pix[yi+qMin(wm,qMax(i,0))];
            sir = stack[i+radius];
            sir[0] = qRed(p);
            sir[1] = qGreen(p);
            sir[2] = qBlue(p);
            sir[3] = qAlpha(p);

            rbs = r1-abs(i);
            rsum += sir[0]*rbs;
            gsum += sir[1]*rbs;
            bsum += sir[2]*rbs;
            asum += sir[3]*rbs;

            if (i > 0)
            {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
                ainsum += sir[3];
            }
            else
            {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
                aoutsum += sir[3];
            }
        }
        stackpointer = radius;

        for (x=0; x < w; ++x)
        {

            r[yi] = dv[rsum];
            g[yi] = dv[gsum];
            b[yi] = dv[bsum];
            a[yi] = dv[asum];

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            asum -= aoutsum;

            stackstart = stackpointer-radius+div;
            sir = stack[stackstart%div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            aoutsum -= sir[3];

            if (y == 0)
            {
                vmin[x] = qMin(x+radius+1,wm);
            }
            p = pix[yw+vmin[x]];

            sir[0] = qRed(p);
            sir[1] = qGreen(p);
            sir[2] = qBlue(p);
            sir[3] = qAlpha(p);

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            ainsum += sir[3];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            asum += ainsum;

            stackpointer = (stackpointer+1)%div;
            sir = stack[(stackpointer)%div];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            aoutsum += sir[3];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            ainsum -= sir[3];

            ++yi;
        }
        yw += w;
    }
    for (x=0; x < w; ++x)
    {
        rinsum = ginsum = binsum = ainsum
                                   = routsum = goutsum = boutsum = aoutsum
                                               = rsum = gsum = bsum = asum = 0;

        yp =- radius * w;

        for(i=-radius; i <= radius; ++i)
        {
            yi=qMax(0,yp)+x;

            sir = stack[i+radius];

            sir[0] = r[yi];
            sir[1] = g[yi];
            sir[2] = b[yi];
            sir[3] = a[yi];

            rbs = r1-abs(i);

            rsum += r[yi]*rbs;
            gsum += g[yi]*rbs;
            bsum += b[yi]*rbs;
            asum += a[yi]*rbs;

            if (i > 0)
            {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
                ainsum += sir[3];
            }
            else
            {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
                aoutsum += sir[3];
            }

            if (i < hm)
            {
                yp += w;
            }
        }

        yi = x;
        stackpointer = radius;

        for (y=0; y < h; ++y)
        {
            pix[yi] = qRgba(dv[rsum], dv[gsum], dv[bsum], dv[asum]);

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            asum -= aoutsum;

            stackstart = stackpointer-radius+div;
            sir = stack[stackstart%div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            aoutsum -= sir[3];

            if (x==0)
            {
                vmin[y] = qMin(y+r1,hm)*w;
            }
            p = x+vmin[y];

            sir[0] = r[p];
            sir[1] = g[p];
            sir[2] = b[p];
            sir[3] = a[p];

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            ainsum += sir[3];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            asum += ainsum;

            stackpointer = (stackpointer+1)%div;
            sir = stack[stackpointer];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            aoutsum += sir[3];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            ainsum -= sir[3];

            yi += w;
        }
    }
    delete [] r;
    delete [] g;
    delete [] b;
    delete [] a;
    delete [] vmin;
    delete [] dv;
}
