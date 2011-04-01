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

/**
 * @mainpage
 * @section Overview
 *
 * The Image Quality Assessment (IQA) library implements popular algorithms for generating image/video quality metrics. There are several principles that guide IQA's development:
 * @li It must be easy to use (cover the common use cases... a.k.a. the 90% rule).
 * @li It must be accurate (results are checked against provided Matlab files in Octave).
 * @li It must be fast.
 * @li It must be portable.
 * @li It must be reliable (unit testing, Valgrind clean).
 * @li It must be well written, using straigh-forward algorithm implementations whenever possible.
 * @li It must be free (released under the BSD License).
 *
 * The following algorithms are currently implemented:
 * @li MSE
 * @li PSNR
 * @li SSIM
 *
 * I plan to add the following algorithms in the future:
 * @li Faster (i.e. less precise) version of SSIM
 * @li MS-SSIM
 * @li CW-SSIM
 * @li VQM
 *
 * IQA exists because I was unable to find a quality, open-source C/C++ implementation of the SSIM algorithm that I could use commercially. I write cross-platform production code for a living, and I needed something I could rely on. Questions or suggestions are always welcome in the project forum (https://sourceforge.net/projects/iqa/support).
 * <br><br>
 * @section Download
 * Source: http://sourceforge.net/projects/iqa/files/
 *
 * Browse the source tree: http://iqa.svn.sourceforge.net/viewvc/iqa/
 *
 * SVN Checkout: @code svn co https://iqa.svn.sourceforge.net/svnroot/iqa iqa @endcode
 * <br>
 * @section Examples
 *
 * Here's a simple example showing how to calculate PSNR:
 *
 * @code
 * #include "iqa.h"
 *
 * // Load reference and modified images.
 *
 * float psnr = iqa_psnr(img_ref, img_mod, width, height, stride);
 * @endcode
 *
 * Some algorithms support additional configuration parameters. Here's an example that calculates the SSIM index using different weighting factors (a=0.5, b=1.34, c=0.5):
  *
 * @code
 * #include "iqa.h"
 *
 * // Load reference and modified images.
 *
 * struct iqa_ssim_args args;
 * args.alpha = 0.5f;
 * args.beta  = 1.34f;
 * args.gamma = 0.5f;
 * args.L  = 255;   // default
 * args.K1 = 0.01;  // default
 * args.K2 = 0.03;  // default
 * args.f  = 0;     // default
 *
 * float ssim = iqa_ssim(img_ref, img_mod, width, height, stride, 1, &args);
 * @endcode
*/
