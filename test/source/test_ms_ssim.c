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

#include "test_ms_ssim.h"
#include "iqa.h"
#include "convolve.h"
#include "bmp.h"
#include "hptime.h"
#include "math_utils.h"
#include <stdio.h>
#include <string.h>

static const struct iqa_ms_ssim_args args_wang = {
    1,  /* Wang */
    1,  /* Gaussian (default) */
    5,  /* Scales (default) */
    0,  /* alphas (default) */
    0,  /* betas (default) */
    0,  /* gammas (default) */
};

static const struct iqa_ms_ssim_args args_linear = {
    0,  /* R/H (default) */
    0,  /* Linear 8x8 window */
    5,  /* Scales (default) */
    0,  /* alphas (default) */
    0,  /* betas (default) */
    0,  /* gammas (default) */
};

static const float g_alphas[] = { 0.0f, 0.0f, 0.0f, 0.25f };
static const float g_betas[]  = { 0.25f, 0.25f, 0.25f, 0.25f };
static const float g_gammas[] = { 0.25f, 0.25f, 0.25f, 0.25f };

static const struct iqa_ms_ssim_args args_scale4 = {
    0,  /* R/H (default) */
    1,  /* Gaussian (default) */
    4,  /* Scales */
    g_alphas,
    g_betas,
    g_gammas,
};


/* Defines the answer format */
struct answer {
    float value;        /**< Expected result */
    int   precision;    /**< Digits of precision */
};

static const struct answer ans_key_einstein_def[] = {
    {1.00000f, 5},  /* Identical */
    {0.85324f, 5},  /* Blur */
    {0.96787f, 5},  /* Contrast */
    {0.12226f, 5},  /* Flip Vertical */
    {0.92878f, 5},  /* Impulse */
    {0.76612f, 5},  /* JPEG */
    {0.99937f, 5},  /* Mean Shift */
};

static const struct answer ans_key_einstein_wang[] = {
    {1.00000f, 5},  /* Identical */
    {0.91875f, 5},  /* Blur */
    {0.97275f, 5},  /* Contrast */
    {0.25467f, 5},  /* Flip Vertical */
    {0.95357f, 5},  /* Impulse */
    {0.89226f, 4},  /* JPEG (rounding error on last digit) */
    {0.99938f, 5},  /* Mean Shift */
};

static const struct answer ans_key_einstein_linear[] = {
    {1.00000f, 5},  /* Identical */
    {0.88482f, 5},  /* Blur */
    {0.96774f, 5},  /* Contrast */
    {0.13595f, 5},  /* Flip Vertical */
    {0.93071f, 5},  /* Impulse */
    {0.79850f, 3},  /* JPEG (rounding error on last digits) */
    {0.99926f, 5},  /* Mean Shift */
};

static const struct answer ans_key_einstein_scale4[] = {
    {1.00000f, 5},  /* Identical */
    {0.72383f, 5},  /* Blur */
    {0.96678f, 5},  /* Contrast */
    {0.09137f, 5},  /* Flip Vertical */
    {0.88489f, 5},  /* Impulse */
    {0.61626f, 4},  /* JPEG (rounding error on last digit) */
    {0.99863f, 5},  /* Mean Shift */
};

static const struct answer ans_key_courtright[] = {
    {1.00000f, 5},    /* Identical */
    {0.59030f, 5},    /* Noise */
};


#define BMP_ORIGINAL    "einstein.bmp"
#define BMP_BLUR        "blur.bmp"
#define BMP_CONTRAST    "contrast.bmp"
#define BMP_FLIPVERT    "flipvertical.bmp"
#define BMP_IMPULSE     "impulse.bmp"
#define BMP_JPG         "jpg.bmp"
#define BMP_MEANSHIFT   "meanshift.bmp"
#define BMP_CR_ORIGINAL "Courtright.bmp"
#define BMP_CR_NOISE    "Courtright_Noise.bmp"

static int _test_einstein_bmp(const struct answer *answers, const struct iqa_ms_ssim_args *args, const char* str);
static int _test_courtright_bmp(const struct answer *answers, const struct iqa_ms_ssim_args *args, const char* str);

/*----------------------------------------------------------------------------
 * TEST ENTRY POINT
 *---------------------------------------------------------------------------*/
int test_ms_ssim()
{
    int failure = 0;

    printf("\nMS-SSIM:\n");
    failure += _test_einstein_bmp(ans_key_einstein_def, 0, "Rouse/Hemami");
    failure += _test_einstein_bmp(ans_key_einstein_wang, &args_wang, "Wang");
    failure += _test_einstein_bmp(ans_key_einstein_linear, &args_linear, "Linear 8x8 Window");
    failure += _test_einstein_bmp(ans_key_einstein_scale4, &args_scale4, "Custom scale = 4");
    failure += _test_courtright_bmp(ans_key_courtright, 0, "Rouse/Hemami");

    return failure;
}

/*----------------------------------------------------------------------------
 * _test_einstein_bmp
 *---------------------------------------------------------------------------*/
int _test_einstein_bmp(const struct answer *answers, const struct iqa_ms_ssim_args *args, const char* str)
{
    struct bmp orig, cmp;
    int passed, failures=0;
    float result;
    unsigned long long start, end;

    printf("\tEinstein (%s):\n", str);

    if (load_bmp(BMP_ORIGINAL, &orig)) {
        printf("FAILED to load \'%s\'\n", BMP_ORIGINAL);
        return 1;
    }

    printf("\t  Identical: ");
    start = hpt_get_time();
    result = iqa_ms_ssim(orig.img, orig.img, orig.w, orig.h, orig.stride, args);
    end = hpt_get_time();
    passed = _cmp_float(result, answers[0].value, answers[0].precision) ? 0 : 1;
    printf("\t\t%.5f  (%.3lf ms)\t%s\n", 
        result, 
        hpt_elapsed_time(start,end,hpt_get_frequency()) * 1000.0,
        passed?"PASS":"FAILED");
    failures += passed?0:1;

    printf("\t  Blur: ");
    if (load_bmp(BMP_BLUR, &cmp)) {
        printf("FAILED to load \'%s\'\n", BMP_BLUR);
        failures++;
    }
    else {
        start = hpt_get_time();
        result = iqa_ms_ssim(orig.img, cmp.img, orig.w, orig.h, orig.stride, args);
        end = hpt_get_time();
        passed = _cmp_float(result, answers[1].value, answers[1].precision) ? 0 : 1;
        printf("\t\t%.5f  (%.3lf ms)\t%s\n", 
            result, 
            hpt_elapsed_time(start,end,hpt_get_frequency()) * 1000.0,
            passed?"PASS":"FAILED");
        failures += passed?0:1;
        free_bmp(&cmp);
    }

    printf("\t  Contrast: ");
    if (load_bmp(BMP_CONTRAST, &cmp)) {
        printf("FAILED to load \'%s\'\n", BMP_CONTRAST);
        failures++;
    }
    else {
        start = hpt_get_time();
        result = iqa_ms_ssim(orig.img, cmp.img, orig.w, orig.h, orig.stride, args);
        end = hpt_get_time();
        passed = _cmp_float(result, answers[2].value, answers[2].precision) ? 0 : 1;
        printf("\t\t%.5f  (%.3lf ms)\t%s\n", 
            result, 
            hpt_elapsed_time(start,end,hpt_get_frequency()) * 1000.0,
            passed?"PASS":"FAILED");
        failures += passed?0:1;
        free_bmp(&cmp);
    }

    printf("\t  Flip Vertical: ");
    if (load_bmp(BMP_FLIPVERT, &cmp)) {
        printf("FAILED to load \'%s\'\n", BMP_FLIPVERT);
        failures++;
    }
    else {
        start = hpt_get_time();
        result = iqa_ms_ssim(orig.img, cmp.img, orig.w, orig.h, orig.stride, args);
        end = hpt_get_time();
        passed = _cmp_float(result, answers[3].value, answers[3].precision) ? 0 : 1;
        printf("\t%.5f  (%.3lf ms)\t%s\n", 
            result, 
            hpt_elapsed_time(start,end,hpt_get_frequency()) * 1000.0,
            passed?"PASS":"FAILED");
        failures += passed?0:1;
        free_bmp(&cmp);
    }

    printf("\t  Impulse: ");
    if (load_bmp(BMP_IMPULSE, &cmp)) {
        printf("FAILED to load \'%s\'\n", BMP_IMPULSE);
        failures++;
    }
    else {
        start = hpt_get_time();
        result = iqa_ms_ssim(orig.img, cmp.img, orig.w, orig.h, orig.stride, args);
        end = hpt_get_time();
        passed = _cmp_float(result, answers[4].value, answers[4].precision) ? 0 : 1;
        printf("\t\t%.5f  (%.3lf ms)\t%s\n", 
            result, 
            hpt_elapsed_time(start,end,hpt_get_frequency()) * 1000.0,
            passed?"PASS":"FAILED");
        failures += passed?0:1;
        free_bmp(&cmp);
    }

    printf("\t  Jpeg: ");
    if (load_bmp(BMP_JPG, &cmp)) {
        printf("FAILED to load \'%s\'\n", BMP_JPG);
        failures++;
    }
    else {
        start = hpt_get_time();
        result = iqa_ms_ssim(orig.img, cmp.img, orig.w, orig.h, orig.stride, args);
        end = hpt_get_time();
        passed = _cmp_float(result, answers[5].value, answers[5].precision) ? 0 : 1;
        printf("\t\t%.5f  (%.3lf ms)\t%s\n", 
            result, 
            hpt_elapsed_time(start,end,hpt_get_frequency()) * 1000.0,
            passed?"PASS":"FAILED");
        failures += passed?0:1;
        free_bmp(&cmp);
    }

    printf("\t  Meanshift: ");
    if (load_bmp(BMP_MEANSHIFT, &cmp)) {
        printf("FAILED to load \'%s\'\n", BMP_MEANSHIFT);
        failures++;
    }
    else {
        start = hpt_get_time();
        result = iqa_ms_ssim(orig.img, cmp.img, orig.w, orig.h, orig.stride, args);
        end = hpt_get_time();
        passed = _cmp_float(result, answers[6].value, answers[6].precision) ? 0 : 1;
        printf("\t\t%.5f  (%.3lf ms)\t%s\n", 
            result, 
            hpt_elapsed_time(start,end,hpt_get_frequency()) * 1000.0,
            passed?"PASS":"FAILED");
        failures += passed?0:1;
        free_bmp(&cmp);
    }

    free_bmp(&orig);
    return failures;
}

/*----------------------------------------------------------------------------
 * _test_courtright_bmp
 *---------------------------------------------------------------------------*/
int _test_courtright_bmp(const struct answer *answers, const struct iqa_ms_ssim_args *args, const char* str)
{
    struct bmp orig, cmp;
    int passed, failures=0;
    float result;
    unsigned long long start, end;

    printf("\tCourtright (%s):\n", str);

    if (load_bmp(BMP_CR_ORIGINAL, &orig)) {
        printf("FAILED to load \'%s\'\n", BMP_CR_ORIGINAL);
        return 1;
    }

    printf("\t  Identical: ");
    start = hpt_get_time();
    result = iqa_ms_ssim(orig.img, orig.img, orig.w, orig.h, orig.stride, args);
    end = hpt_get_time();
    passed = _cmp_float(result, answers[0].value, answers[0].precision) ? 0 : 1;
    printf("\t\t%.5f  (%.3lf ms)\t%s\n", 
        result, 
        hpt_elapsed_time(start,end,hpt_get_frequency()) * 1000.0,
        passed?"PASS":"FAILED");
    failures += passed?0:1;

    printf("\t  Noise: ");
    if (load_bmp(BMP_CR_NOISE, &cmp)) {
        printf("FAILED to load \'%s\'\n", BMP_CR_NOISE);
        failures++;
    }
    else {
        start = hpt_get_time();
        result = iqa_ms_ssim(orig.img, cmp.img, orig.w, orig.h, orig.stride, args);
        end = hpt_get_time();
        passed = _cmp_float(result, answers[1].value, answers[1].precision) ? 0 : 1;
        printf("\t\t%.5f  (%.3lf ms)\t%s\n", 
            result, 
            hpt_elapsed_time(start,end,hpt_get_frequency()) * 1000.0,
            passed?"PASS":"FAILED");
        failures += passed?0:1;
        free_bmp(&cmp);
    }

    free_bmp(&orig);
    return failures;
}

