#ifndef CAMERA_DOF_H
#define CAMERA_DOF_H

#include <QtMath>
#include <vector>
#include <QRect>
#include <QDebug>

namespace std
{

    qreal hf_distance = 0.0;
    const double CoC = 0.03; // Circle of Confusion = 0.03 mm

    /** Get Hyperfocal distance
      *
     * \param B Focal length (50mm)
     * \param f f_stop Aperture (1,4 to 22 mm)
     * \return Hyperfocal Distance in millimeters
     */
    inline qreal getHyperfocalDistance(const qreal B, const qreal f)
    {
        return (B*B)/(f*CoC) + B;
    }

    /**  Get the nearest distance, where the object will be i "acceptable" focus
     *
     * \param H HyperFocal Distance
     * \param B Focal length (50mm)
     * \param d distance setting on camera in millimeters
     * \return dof_near in millimeters
     */
    inline qreal getDOF_near(qreal hf_distance, const qreal B, const qreal d)
    {
        return ((hf_distance - B) * d)/(hf_distance + d - 2*B);
    }

    /**  Get the farthest distance, where the object will be i "acceptable" focus
     *
     * \param H HyperFocal Distance
     * \param B Focal length (50mm)
     * \param d distance setting on camera in millimeters
     * \return dof_far in millimeters
     */
    inline qreal getDOF_far(qreal hf_distance, const qreal B, const qreal d)
    {
        return ((hf_distance - B) * d)/(hf_distance - d);
    }

    // Copyright (C) 2017-2022 Basile Fraboni
    // Copyright (C) 2014 Ivan Kutskir (for the original fast blur implmentation)
    // All Rights Reserved
    // You may use, distribute and modify this code under the
    // terms of the MIT license. For further details please refer
    // to : https://mit-license.org/
    //
    #pragma once

    //!
    //! \file fast_gaussian_blur_template.h
    //! \author Basile Fraboni
    //! \date 2017 - 2022
    //!
    //! \brief This contains a C++ implementation of a fast Gaussian blur algorithm in linear time.
    //!
    //! The image buffer is supposed to be of size w * h * c, h its height, with w its width,
    //! and c its number of channels.
    //! The default implementation only supports up to 4 channels images, but one can easily add support for any number of channels
    //! using either specific template cases or a generic function that takes the number of channels as an explicit parameter.
    //! This implementation is focused on learning and readability more than on performance.
    //! The fast blur algorithm is performed with several box blur passes over an image.
    //! The filter converges towards a true Gaussian blur after several passes (thanks TCL). In practice,
    //! three passes are sufficient for good quality results.
    //! For further details please refer to:
    //!     - http://blog.ivank.net/fastest-gaussian-blur.html
    //!     - https://www.peterkovesi.com/papers/FastGaussianSmoothing.pdf
    //!     - https://github.com/bfraboni/FastGaussianBlur
    //!
    //! **Note:** The fast gaussian blur algorithm is not accurate on image boundaries.
    //! It performs a diffusion of the signal with several independant passes, each pass depending
    //! of the preceding one. Some of the diffused signal is lost near borders and results in a slight
    //! loss of accuracy for next pass. This problem can be solved by increasing the image support of
    //! half the box kernel extent at each pass of the algorithm. The added padding would in this case
    //! capture the diffusion and make the next pass accurate.
    //! On contrary true Gaussian blur does not suffer this problem since the whole diffusion process
    //! is performed in one pass only.
    //! The extra padding is not performed in this implementation, however we provide and discuss several border
    //! policies resulting in dfferent approximations and accuracies.
    //!

    //!
    //! \brief Enumeration that decribes border policies for filters.
    //!
    //! For a detailed description of border policies please refer to:
    //! https://en.wikipedia.org/wiki/Kernel_(image_processing)#Edge_Handling
    //!
    //! \todo Add support for other border policies (wrap, mirror)
    enum BorderPolicy
    {
        kExtend,
        kKernelCrop,
        // kWrap,
        // kMirror,
    };

    //!
    //! \brief This function performs a single separable horizontal box blur pass with border extend policy.
    //!
    //! To complete a box blur pass we need to do this operation two times, one horizontally
    //! and one vertically. Templated by buffer data type T, buffer number of channels C.
    //!
    //! \param[in] in           source buffer
    //! \param[in,out] out      target buffer
    //! \param[in] w            image width
    //! \param[in] h            image height
    //! \param[in] r            box dimension
    //!

    // original generic version
    template<typename T, int C>
    void horizontal_blur_extend(const T * in, T * out, const int w, const int h, const int r)
    {
        // change the local variable types depending on the template type for faster calculations
        using calc_type = std::conditional<std::is_integral<T>::value, int, float>;

        float iarr = 1.f / (r+r+1);
        #pragma omp parallel for
        for(int i=0; i<h; i++)
        {
            const int begin = i*w;
            const int end = begin+w;

            // current index, left index, right index
            int ti = begin, li = begin-r-1, ri = begin+r;

            // first value, last value, sliding accumulator
            calc_type fv[C], lv[C], acc[C];

            for(int ch = 0; ch < C; ++ch)
            {
                fv[ch] =  in[begin*C+ch];
                lv[ch] =  in[(end-1)*C+ch];
                acc[ch] = (r+1)*fv[ch];
            }

            // initial acucmulation
            for(int j=0; j<r; j++)
            for(int ch = 0; ch < C; ++ch)
            {
                // prefilling the accumulator with the last value seems slower than/equal to this ternary
                acc[ch] += j < w ? in[(begin+j)*C+ch] : lv[ch];
            }

            // perform filtering
            for(int j=0; j<w; j++, ri++, ti++, li++)
            for(int ch = 0; ch < C; ++ch)
            {
                acc[ch] += ri < end ?       in[ri*C+ch] : lv[ch];
                acc[ch] -= li >= begin ?    in[li*C+ch] : fv[ch];
                out[ti*C+ch] = acc[ch]*iarr + (std::is_integral<T>::value ? 0.5f : 0); // fixes darkening with integer types
            }
        }
    }

    // version for kernels that are correctly sized, that is when r <= w
    template<typename T, int C>
    void horizontal_blur_extend_small_kernel(const T * in, T * out, const int w, const int h, const int r)
    {
        // change the local variable types depending on the template type for faster calculations
        using calc_type = std::conditional<std::is_integral<T>::value, int, float>;

        float iarr = 1.f / (r+r+1);
        #pragma omp parallel for
        for(int i=0; i<h; i++)
        {
            const int begin = i*w;
//            const int end = begin+w;
            int ti = begin, li = begin, ri = begin+r;   // current index, left index, right index
            calc_type fv[C], lv[C], acc[C];             // first value, last value, sliding accumulator

            // init fv, lv, acc by extending outside the image buffer
            for(int ch = 0; ch < C; ++ch)
            {
                fv[ch] =  in[ti*C+ch];
                lv[ch] =  in[(ti+w-1)*C+ch];
                acc[ch] = (r+1)*fv[ch];
            }

            // initial acucmulation inside the image buffer
            for(int j=ti; j < ri; j++)
            for(int ch = 0; ch < C; ++ch)
            {
                // prefilling the accumulator with the last value seems slower than/equal to this ternary
                acc[ch] += in[j*C+ch];
            }

            for(int j=0; j<=r; j++, ri++, ti++) // remove li++ and add li=begin instead of li=begin-r-1
            for(int ch = 0; ch < C; ++ch)
            {
                acc[ch] += in[ri*C+ch] - fv[ch];
                out[ti*C+ch] = acc[ch]*iarr + (std::is_integral<T>::value ? 0.5f : 0); // fixes darkening with integer types
            }

            for(int j=r+1; j<w-r; j++, ri++, ti++, li++)
            for(int ch = 0; ch < C; ++ch)
            {
                acc[ch] += in[ri*C+ch] - in[li*C+ch];
                out[ti*C+ch] = acc[ch]*iarr + (std::is_integral<T>::value ? 0.5f : 0); // fixes darkening with integer types

            }

            for(int j=w-r; j<w; j++, ti++, li++) // remove ri++
            for(int ch = 0; ch < C; ++ch)
            {
                acc[ch] += lv[ch] - in[li*C+ch];
                out[ti*C+ch] = acc[ch]*iarr + (std::is_integral<T>::value ? 0.5f : 0); // fixes darkening with integer types
            }
        }
    }

    // version for kernels that are too large, that is when r > w
    template<typename T, int C>
    void horizontal_blur_extend_large_kernel(const T * in, T * out, const int w, const int h, const int r)
    {
        // change the local variable types depending on the template type for faster calculations
        using calc_type = std::conditional<std::is_integral<T>::value, int, float>;

        float iarr = 1.f / (r+r+1);
        #pragma omp parallel for
        for(int i=0; i<h; i++)
        {
            const int begin = i*w;
            const int end = begin+w;
            calc_type fv[C], lv[C], acc[C]; // first value, last value, sliding accumulator

            for(int ch = 0; ch < C; ++ch)
            {
                fv[ch] =  in[begin*C+ch];
                lv[ch] =  in[(end-1)*C+ch];
                acc[ch] = (r+1)*fv[ch];
            }

            // initial acucmulation
            for(int j=0; j<r; j++)
            for(int ch = 0; ch < C; ++ch)
            {
                // prefilling the accumulator with the last value seems slower than/equal to this ternary
                acc[ch] += j < w ? in[(begin+j)*C+ch] : lv[ch];
            }

            for(int ti = begin; ti < end; ti++)
            for(int ch = 0; ch < C; ++ch)
            {
                acc[ch] += lv[ch] - fv[ch];
                out[ti*C+ch] = acc[ch]*iarr + (std::is_integral<T>::value ? 0.5f : 0); // fixes darkening with integer types
            }
        }
    }

    //!
    //! \brief This function performs a single separable horizontal box blur pass with kernel crop border policy.
    //!
    //! To complete a box blur pass we need to do this operation two times, one horizontally
    //! and one vertically. Templated by buffer data type T, buffer number of channels C.
    //!
    //! \param[in] in           source buffer
    //! \param[in,out] out      target buffer
    //! \param[in] w            image width
    //! \param[in] h            image height
    //! \param[in] r            box dimension
    //!
    template<typename T, int C>
    void horizontal_blur_kernel_crop(const T * in, T * out, const int w, const int h, const int r)
    {
        // change the local variable types depending on the template type for faster calculations
        using calc_type = std::conditional<std::is_integral<T>::value, int, float>;

        #pragma omp parallel for
        for(int i=0; i<h; i++)
        {
            int ti = i*w, li = ti-r-1, ri = ti+r;   // current index, left index, right index
            calc_type acc[C];                       // sliding accumulator

            for(int ch = 0; ch < C; ++ch)
            {
                acc[ch] = 0;
            }

            // initial acucmulation
            for(int j=0; j<r; j++)
            for(int ch = 0; ch < C; ++ch)
            {
                acc[ch] += ti+j < ti+w ? in[(ti+j)*C+ch] : 0;
            }

            // perform filtering
            for(int j=0; j<w; j++, ri++, ti++, li++)
            for(int ch = 0; ch < C; ++ch)
            {
                acc[ch] += ri < (i+1)*w ?   in[ri*C+ch] : 0;
                acc[ch] -= li >= i*w ?      in[li*C+ch] : 0;
                int start = std::max(i*w-1, li);
                int end = std::min((i+1)*w-1, ri);
                // renormalize kernel
                out[ti*C+ch] = acc[ch]/float(end-start) + (std::is_integral<T>::value ? 0.5f : 0); // fixes darkening with integer types;
            }
        }
    }

    //! template<typename T, int C>
    //! void horizontal_blur_mirror(const T * in, T * out, const int w, const int h, const int r);
    //! template<typename T, int C>
    //! void horizontal_blur_wrap(const T * in, T * out, const int w, const int h, const int r);

    //!
    //! \brief This function performs a single separable horizontal box blur pass.
    //!
    //! To complete a box blur pass we need to do this operation two times, one horizontally
    //! and one vertically. Templated by buffer data type T, buffer number of channels C, and border policy P.
    //!
    //! \param[in] in           source buffer
    //! \param[in,out] out      target buffer
    //! \param[in] w            image width
    //! \param[in] h            image height
    //! \param[in] r            box dimension
    //!
    template<typename T, int C, BorderPolicy P = kExtend>
    void horizontal_blur(const T * in, T * out, const int w, const int h, const int r)
    {
        if constexpr(P == kExtend)
        {
            // horizontal_blur_extend<T,C>(in, out, w, h, r);   // generic version
            if( r > w )
                horizontal_blur_extend_large_kernel<T,C>(in, out, w, h, r); // large kernels version
            else
                horizontal_blur_extend_small_kernel<T,C>(in, out, w, h, r); // small kernels version
        }
        else
        {
            horizontal_blur_kernel_crop<T,C>(in, out, w, h, r);
        }
    }

    //!
    //! \brief Utility template dispatcher function for horizontal_blur. Templated by buffer data type T.
    //!
    //! \param[in] in           source buffer
    //! \param[in,out] out      target buffer
    //! \param[in] w            image width
    //! \param[in] h            image height
    //! \param[in] c            image channels
    //! \param[in] r            box dimension
    //!
    template<typename T>
    void horizontal_blur(const T * in, T * out, const int w, const int h, const int c, const int r)
    {
        switch(c)
        {
            case 1: horizontal_blur<T,1>(in, out, w, h, r); break;
            case 2: horizontal_blur<T,2>(in, out, w, h, r); break;
            case 3: horizontal_blur<T,3>(in, out, w, h, r); break;
            case 4: horizontal_blur<T,4>(in, out, w, h, r); break;
            default: printf("horizontal_blur over %d channels is not supported yet. Add a specific case if possible or fall back to the generic version.\n", c); break;
            // default: horizontal_blur<T>(in, out, w, h, c, r); break;
        }
    }

    //!
    //! \brief This function performs a 2D tranposition of an image.
    //!
    //! The transposition is done per
    //! block to reduce the number of cache misses and improve cache coherency for large image buffers.
    //! Templated by buffer data type T and buffer number of channels C.
    //!
    //! \param[in] in           source buffer
    //! \param[in,out] out      target buffer
    //! \param[in] w            image width
    //! \param[in] h            image height
    //!
    template<typename T, int C>
    void flip_block(const T * in, T * out, const int w, const int h)
    {
        constexpr int block = 256/C;
        #pragma omp parallel for collapse(2)
        for(int x= 0; x < w; x+= block)
        for(int y= 0; y < h; y+= block)
        {
            const T * p = in + y*w*C + x*C;
            T * q = out + y*C + x*h*C;

            const int blockx= std::min(w, x+block) - x;
            const int blocky= std::min(h, y+block) - y;
            for(int xx= 0; xx < blockx; xx++)
            {
                for(int yy= 0; yy < blocky; yy++)
                {
                    for(int k= 0; k < C; k++)
                        q[k]= p[k];
                    p+= w*C;
                    q+= C;
                }
                p+= -blocky*w*C + C;
                q+= -blocky*C + h*C;
            }
        }
    }
    //!
    //! \brief Utility template dispatcher function for flip_block. Templated by buffer data type T.
    //!
    //! \param[in] in           source buffer
    //! \param[in,out] out      target buffer
    //! \param[in] w            image width
    //! \param[in] h            image height
    //! \param[in] c            image channels
    //!
    template<typename T>
    void flip_block(const T * in, T * out, const int w, const int h, const int c)
    {
        switch(c)
        {
            case 1: flip_block<T,1>(in, out, w, h); break;
            case 2: flip_block<T,2>(in, out, w, h); break;
            case 3: flip_block<T,3>(in, out, w, h); break;
            case 4: flip_block<T,4>(in, out, w, h); break;
            default: printf("flip_block over %d channels is not supported yet. Add a specific case if possible or fall back to the generic version.\n", c); break;
            // default: flip_block<T>(in, out, w, h, c); break;
        }
    }

    //!
    //! \brief This function converts the standard deviation of
    //! Gaussian blur into a box radius for each box blur pass.
    //! Returns the approximate sigma value achieved with the N box blur passes.
    //!
    //! For further details please refer to :
    //! - https://www.peterkovesi.com/papers/FastGaussianSmoothing.pdf
    //!
    //! \param[out] boxes   box radiis for kernel sizes of 2*boxes[i]+1
    //! \param[in] sigma    Gaussian standard deviation
    //! \param[in] n        number of box blur pass
    //!
    float sigma_to_box_radius(int boxes[], const float sigma, const int n)
    {
        // ideal filter width
        float wi = std::sqrt((12*sigma*sigma/n)+1);
        int wl = wi; // no need std::floor
        if(wl%2==0) wl--;
        int wu = wl+2;

        float mi = (12*sigma*sigma - n*wl*wl - 4*n*wl - 3*n)/(-4*wl - 4);
        int m = mi+0.5f; // avoid std::round by adding 0.5f and cast to integer type

        for(int i=0; i<n; i++)
            boxes[i] = ((i < m ? wl : wu) - 1) / 2;

        return std::sqrt((m*wl*wl+(n-m)*wu*wu-n)/12.f);
    }

    //!
    //! \brief This function performs a fast Gaussian blur. Templated by buffer data type T and number of passes N.
    //!
    //! Applying several times box blur tends towards a true Gaussian blur (thanks TCL). Three passes are sufficient
    //! for good results. Templated by buffer data type T and number of passes N. The input buffer is also used
    //! as temporary and modified during the process hence it can not be constant.
    //!
    //! Usually the process should alternate between horizontal and vertical passes
    //! as much times as we want box blur passes. However thanks to box blur properties
    //! the separable passes can be performed in any order without changing the result.
    //! Hence for performance purposes the algorithm is:
    //! - apply N times horizontal blur (horizontal passes)
    //! - flip the image buffer (transposition)
    //! - apply N times horizontal blur (vertical passes)
    //! - flip the image buffer (transposition)
    //!
    //! We provide two version of the function:
    //! - generic N passes (in which more std::swap are used)
    //! - specialized 3 passes only
    //!
    //! \param[in,out] in       source buffer reference ptr
    //! \param[in,out] out      target buffer reference ptr
    //! \param[in] w            image width
    //! \param[in] h            image height
    //! \param[in] c            image channels
    //! \param[in] sigma        Gaussian standard deviation
    //!
    template<typename T, unsigned int N>
    void fast_gaussian_blur(T *& in, T *& out, const int w, const int h, const int c, const float sigma)
    {
        // compute box kernel sizes
        int boxes[N];
        sigma_to_box_radius(boxes, sigma, N);

        // perform N horizontal blur passes
        for(int i = 0; i < N; ++i)
        {
            horizontal_blur(in, out, w, h, c, boxes[i]);
            std::swap(in, out);
        }

        // flip buffer
        flip_block(in, out, w, h, c);
        std::swap(in, out);

        // perform N horizontal blur passes on flipped image
        for(int i = 0; i < N; ++i)
        {
            horizontal_blur(in, out, h, w, c, boxes[i]);
            std::swap(in, out);
        }

        // flip buffer
        flip_block(in, out, h, w, c);
    }


    //!
    //! \brief Specialized 3 passes of separable fast box blur with less std::swap. Templated by buffer data type T.
    //!
    //! Applying several times box blur tends towards a true Gaussian blur (thanks TCL). Three passes are sufficient
    //! for good results. Templated by buffer data type T and number of passes N. The input buffer is also used
    //! as temporary and modified during the process hence it can not be constant.
    //!
    //! Usually the process should alternate between horizontal and vertical passes
    //! as much times as we want box blur passes. However thanks to box blur properties
    //! the separable passes can be performed in any order without changing the result.
    //! Hence for performance purposes the algorithm is:
    //! - apply N times horizontal blur (horizontal passes)
    //! - flip the image buffer (transposition)
    //! - apply N times horizontal blur (vertical passes)
    //! - flip the image buffer (transposition)
    //!
    //! We provide two version of the function:
    //! - generic N passes (in which more std::swap are used)
    //! - specialized 3 passes only
    //!
    //! \param[in,out] in       source buffer reference ptr
    //! \param[in,out] out      target buffer reference ptr
    //! \param[in] w            image width
    //! \param[in] h            image height
    //! \param[in] c            image channels
    //! \param[in] sigma        Gaussian standard deviation
    //!
    template<typename T>
    void fast_gaussian_blur(T *& in, T *& out, const int w, const int h, const int c, const float sigma)
    {
        // compute box kernel sizes
        int boxes[3];
        sigma_to_box_radius(boxes, sigma, 3);

        // perform 3 horizontal blur passes
        horizontal_blur(in, out, w, h, c, boxes[0]);
        horizontal_blur(out, in, w, h, c, boxes[1]);
        horizontal_blur(in, out, w, h, c, boxes[2]);

        // flip buffer
        flip_block(out, in, w, h, c);

        // perform 3 horizontal blur passes on flipped image
        horizontal_blur(in, out, h, w, c, boxes[0]);
        horizontal_blur(out, in, h, w, c, boxes[1]);
        horizontal_blur(in, out, h, w, c, boxes[2]);

        // flip buffer
        flip_block(out, in, h, w, c);

        // swap pointers to get result in the ouput buffer
        std::swap(in, out);
    }

    //!
    //! \brief Utility template dispatcher function for fast_gaussian_blur. Templated by buffer data type T.
    //!
    //! This is the main exposed function and the one that should be used in programs.
    //!
    //! \todo Make border policies an argument of this function.
    //!
    //! \param[in,out] in       source buffer reference ptr
    //! \param[in,out] out      target buffer reference ptr
    //! \param[in] w            image width
    //! \param[in] h            image height
    //! \param[in] c            image channels
    //! \param[in] sigma        Gaussian standard deviation
    //! \param[in] n            number of passes, should be > 0
    //!
    template<typename T>
    void fast_gaussian_blur(T *& in, T *& out, const int w, const int h, const int c, const float sigma, const unsigned int n)
    {
        switch(n)
        {
            case 1: fast_gaussian_blur<T,1>(in, out, w, h, c, sigma); break;
            case 2: fast_gaussian_blur<T,2>(in, out, w, h, c, sigma); break;
            case 3: fast_gaussian_blur<T>(in, out, w, h, c, sigma); break;      // specialized 3 passes version
            case 4: fast_gaussian_blur<T,4>(in, out, w, h, c, sigma); break;
            case 5: fast_gaussian_blur<T,5>(in, out, w, h, c, sigma); break;
            case 6: fast_gaussian_blur<T,6>(in, out, w, h, c, sigma); break;
            case 7: fast_gaussian_blur<T,7>(in, out, w, h, c, sigma); break;
            case 8: fast_gaussian_blur<T,8>(in, out, w, h, c, sigma); break;
            case 9: fast_gaussian_blur<T,9>(in, out, w, h, c, sigma); break;
            case 10: fast_gaussian_blur<T,10>(in, out, w, h, c, sigma); break;
            default: printf("fast_gaussian_blur with %d passes is not supported yet. Add a specific case if possible or fall back to the generic version.\n", n); break;
            // default: fast_gaussian_blur<T,10>(in, out, w, h, c, sigma, n); break;
        }
    }
}

#endif // CAMERA_DOF_H
