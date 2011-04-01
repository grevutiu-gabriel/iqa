/*
 * Copyright (c) 2011, Tom Distler (http://tdistler.com)
 * All rights reserved.
 *
 * The BSD License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * - Neither the name of the tdistler.com nor the names of its contributors may
 *   be used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "iqa.h"
#include "os.h"
#include "convolve.h"
#include "decimate.h"
#include "math_utils.h"
#include "ssim.h"
#include <stdlib.h>
#include <math.h>

/*
 * Circular-symmetric Gaussian weighting.
 * h(x,y) = hg(x,y)/SUM(SUM(hg)) , for normalization to 1.0
 * hg(x,y) = e^( -0.5*( (x^2+y^2)/sigma^2 ) ) , where sigma was 1.5
 */
#define GAUSSIAN_LEN 11
static const float g_gaussian_window[GAUSSIAN_LEN][GAUSSIAN_LEN] = {
    {0.000001f, 0.000008f, 0.000037f, 0.000112f, 0.000219f, 0.000274f, 0.000219f, 0.000112f, 0.000037f, 0.000008f, 0.000001f},
    {0.000008f, 0.000058f, 0.000274f, 0.000831f, 0.001619f, 0.002021f, 0.001619f, 0.000831f, 0.000274f, 0.000058f, 0.000008f},
    {0.000037f, 0.000274f, 0.001296f, 0.003937f, 0.007668f, 0.009577f, 0.007668f, 0.003937f, 0.001296f, 0.000274f, 0.000037f},
    {0.000112f, 0.000831f, 0.003937f, 0.011960f, 0.023294f, 0.029091f, 0.023294f, 0.011960f, 0.003937f, 0.000831f, 0.000112f},
    {0.000219f, 0.001619f, 0.007668f, 0.023294f, 0.045371f, 0.056662f, 0.045371f, 0.023294f, 0.007668f, 0.001619f, 0.000219f},
    {0.000274f, 0.002021f, 0.009577f, 0.029091f, 0.056662f, 0.070762f, 0.056662f, 0.029091f, 0.009577f, 0.002021f, 0.000274f},
    {0.000219f, 0.001619f, 0.007668f, 0.023294f, 0.045371f, 0.056662f, 0.045371f, 0.023294f, 0.007668f, 0.001619f, 0.000219f},
    {0.000112f, 0.000831f, 0.003937f, 0.011960f, 0.023294f, 0.029091f, 0.023294f, 0.011960f, 0.003937f, 0.000831f, 0.000112f},
    {0.000037f, 0.000274f, 0.001296f, 0.003937f, 0.007668f, 0.009577f, 0.007668f, 0.003937f, 0.001296f, 0.000274f, 0.000037f},
    {0.000008f, 0.000058f, 0.000274f, 0.000831f, 0.001619f, 0.002021f, 0.001619f, 0.000831f, 0.000274f, 0.000058f, 0.000008f},
    {0.000001f, 0.000008f, 0.000037f, 0.000112f, 0.000219f, 0.000274f, 0.000219f, 0.000112f, 0.000037f, 0.000008f, 0.000001f},
};

/*
 * Equal weight square window.
 * Each pixel is equally weighted (1/64) so that SUM(x) = 1.0
 */
#define SQUARE_LEN 8
static const float g_square_window[SQUARE_LEN][SQUARE_LEN] = {
    {0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f},
    {0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f},
    {0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f},
    {0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f},
    {0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f},
    {0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f},
    {0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f},
    {0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f, 0.015625f},
};


/* 
 * SSIM(x,y)=(2*ux*uy + C1)*(2sxy + C2) / (ux^2 + uy^2 + C1)*(sx^2 + sy^2 + C2)
 * where,
 *  ux = SUM(w*x)
 *  sx = (SUM(w*(x-ux)^2)^0.5
 *  sxy = SUM(w*(x-ux)*(y-uy))
 *
 * Returns mean SSIM. MSSIM(X,Y) = 1/M * SUM(SSIM(x,y))
 */
float iqa_ssim(const unsigned char *ref, const unsigned char *cmp, int w, int h, int stride,
    int gaussian, const struct iqa_ssim_args *args)
{
    int scale;
    int x,y,src_offset,offset;
    float *ref_f,*cmp_f;
    struct _kernel low_pass;
    struct _kernel window;
    float result;

    /* Initialize algorithm parameters */
    scale = _max( 1, _round( (float)_min(w,h) / 256.0f ) );
    if (args && args->f)
        scale = args->f;
    window.kernel = (float*)g_square_window;
    window.w = window.h = SQUARE_LEN;
    window.normalized = 1;
    window.bnd_opt = KBND_SYMMETRIC;
    if (gaussian) {
        window.kernel = (float*)g_gaussian_window;
        window.w = window.h = GAUSSIAN_LEN;
    }

    /* Convert image values to floats. Forcing stride = width. */
    ref_f = (float*)malloc(w*h*sizeof(float));
    cmp_f = (float*)malloc(w*h*sizeof(float));
    if (!ref_f || !cmp_f) {
        if (ref_f) free(ref_f);
        if (cmp_f) free(cmp_f);
        return NAN;
    }
    for (y=0; y<h; ++y) {
        src_offset = y*stride;
        offset = y*w;
        for (x=0; x<w; ++x, ++offset, ++src_offset) {
            ref_f[offset] = (float)ref[src_offset];
            cmp_f[offset] = (float)cmp[src_offset];
        }
    }

    /* Scale the images down if required */
    if (scale > 1) {
        /* Generate simple low-pass filter */
        low_pass.kernel = (float*)malloc(scale*scale*sizeof(float));
        if (!low_pass.kernel) {
            free(ref_f);
            free(cmp_f);
            return NAN;
        }
        low_pass.w = low_pass.h = scale;
        low_pass.normalized = 0;
        low_pass.bnd_opt = KBND_SYMMETRIC;
        for (offset=0; offset<scale*scale; ++offset)
            low_pass.kernel[offset] = 1.0f;

        /* Resample */
        if (_iqa_decimate(ref_f, w, h, scale, &low_pass, 0, 0, 0) ||
            _iqa_decimate(cmp_f, w, h, scale, &low_pass, 0, &w, &h)) { /* Update w/h */
            free(ref_f);
            free(cmp_f);
            free(low_pass.kernel);
            return NAN;
        }
        free(low_pass.kernel);
    }

    result = _iqa_ssim(ref_f, cmp_f, w, h, &window, args);
    
    free(ref_f);
    free(cmp_f);

    return result;
}


/* _iqa_ssim */
float _iqa_ssim(float *ref, float *cmp, int w, int h, const struct _kernel *k, const struct iqa_ssim_args *args)
{
    float alpha=1.0f, beta=1.0f, gamma=1.0f;
    int L=255;
    float K1=0.01f, K2=0.03f;
    float C1,C2,C3;
    int x,y,offset;
    float *ref_mu,*cmp_mu,*ref_sigma_sqd,*cmp_sigma_sqd,*sigma_both;
    double ssim_sum, numerator, denominator;
    double luminance_comp, contrast_comp, structure_comp, sigma_ref, sigma_cmp;

    /* Initialize algorithm parameters */
    if (args) {
        alpha = args->alpha;
        beta  = args->beta;
        gamma = args->gamma;
        L     = args->L;
        K1    = args->K1;
        K2    = args->K2;
    }
    C1 = (K1*L)*(K1*L);
    C2 = (K2*L)*(K2*L);
    C3 = C2 / 2.0f;

    ref_mu = (float*)malloc(w*h*sizeof(float));
    cmp_mu = (float*)malloc(w*h*sizeof(float));
    ref_sigma_sqd = (float*)malloc(w*h*sizeof(float));
    cmp_sigma_sqd = (float*)malloc(w*h*sizeof(float));
    sigma_both = (float*)malloc(w*h*sizeof(float));
    if (!ref_mu || !cmp_mu || !ref_sigma_sqd || !cmp_sigma_sqd || !sigma_both) {
        if (ref_mu) free(ref_mu);
        if (cmp_mu) free(cmp_mu);
        if (ref_sigma_sqd) free(ref_sigma_sqd);
        if (cmp_sigma_sqd) free(cmp_sigma_sqd);
        if (sigma_both) free(sigma_both);
        return NAN;
    }

    /* Calculate mean */
    _iqa_convolve(ref, w, h, k, ref_mu, 0, 0);
    _iqa_convolve(cmp, w, h, k, cmp_mu, 0, 0);

    for (y=0; y<h; ++y) {
        offset = y*w;
        for (x=0; x<w; ++x, ++offset) {
            ref_sigma_sqd[offset] = ref[offset] * ref[offset];
            cmp_sigma_sqd[offset] = cmp[offset] * cmp[offset];
            sigma_both[offset] = ref[offset] * cmp[offset];
        }
    }

    /* Calculate sigma */
    _iqa_convolve(ref_sigma_sqd, w, h, k, 0, 0, 0);
    _iqa_convolve(cmp_sigma_sqd, w, h, k, 0, 0, 0);
    _iqa_convolve(sigma_both, w, h, k, 0, &w, &h); /* Update the width and height */

    /* The convolution results are smaller by the kernel width and height */
    for (y=0; y<h; ++y) {
        offset = y*w;
        for (x=0; x<w; ++x, ++offset) {
            ref_sigma_sqd[offset] -= ref_mu[offset] * ref_mu[offset];
            cmp_sigma_sqd[offset] -= cmp_mu[offset] * cmp_mu[offset];
            sigma_both[offset] -= ref_mu[offset] * cmp_mu[offset];
        }
    }

    ssim_sum = 0.0;
    for (y=0; y<h; ++y) {
        offset = y*w;
        for (x=0; x<w; ++x, ++offset) {

            if (!args) {
                /* The default case */
                numerator   = (2.0 * ref_mu[offset] * cmp_mu[offset] + C1) * (2.0 * sigma_both[offset] + C2);
                denominator = (ref_mu[offset]*ref_mu[offset] + cmp_mu[offset]*cmp_mu[offset] + C1) * 
                    (ref_sigma_sqd[offset] + cmp_sigma_sqd[offset] + C2);
                ssim_sum += numerator / denominator;
            }
            else {
                /* User tweaked alpha, beta, or gamma */
                sigma_ref = sqrt(fabs(ref_sigma_sqd[offset]));
                sigma_cmp = sqrt(fabs(cmp_sigma_sqd[offset]));
                luminance_comp = fabs((2.0 * ref_mu[offset] * cmp_mu[offset] + C1) / 
                    (ref_mu[offset]*ref_mu[offset] + cmp_mu[offset]*cmp_mu[offset] + C1));
                contrast_comp  = fabs((2.0 * sigma_ref * sigma_cmp + C2) / 
                    (ref_sigma_sqd[offset] + cmp_sigma_sqd[offset] + C2));
                structure_comp = fabs((sigma_both[offset] + C3) / (sigma_ref * sigma_cmp + C3));
                ssim_sum += pow(luminance_comp,(double)alpha) * pow(contrast_comp,(double)beta) * pow(structure_comp,(double)gamma);
            }
        }
    }

    free(ref_mu);
    free(cmp_mu);
    free(ref_sigma_sqd);
    free(cmp_sigma_sqd);
    free(sigma_both);

    return (float)(ssim_sum / (double)(w*h));
}
