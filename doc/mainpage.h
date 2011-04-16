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
 * @li It must be reliable (unit tested, Valgrind clean).
 * @li It must be well written, using straigh-forward algorithm implementations whenever possible.
 * @li It must be free (released under the BSD License).
 *
 * The following algorithms are implemented in the current release:
 * @li @ref mse "MSE"
 * @li @ref psnr "PSNR"
 * @li @ref ssim "SSIM"
 * @li @ref ms_ssim "MS-SSIM"
 * @li @ref ms_ssim_star "MS-SSIM*"
 *
 * Check out the @ref algorithms "algorithms" page for information about each one. I plan to add the following algorithms in the future:
 * @li Fast SSIM and MS-SSIM
 * @li CW-SSIM
 * @li VQM
 *
 * IQA exists because I was unable to find a quality, open-source C/C++ implementation of the SSIM algorithm that I could use commercially. I write cross-platform production code for a living, and I needed something I could rely on. Questions or suggestions are always welcome in the project forum (https://sourceforge.net/projects/iqa/support).
 * <br><br>
 * @section Download
 * Source: http://sourceforge.net/projects/iqa/files/
 *
 * Browse the source tree: http://iqa.svn.sourceforge.net/viewvc/iqa/trunk
 *
 * SVN Checkout: @code svn co https://iqa.svn.sourceforge.net/svnroot/iqa/trunk iqa @endcode
 * <br>
 *
 * @section get_started Getting Started
 * @li @ref algorithms "Algorithm Information"
 * @li @ref build "Building IQA"
 * @li @ref sample_code "Sample Code"
 *
 * <br>
 *
 * @section whos_using Who's Using IQA
 * @li IQApp - A Windows command line tool for video quality assessment. http://tdistler.com/projects/iqapp
 *
 * <br>
 *
 * @section what What's New in 1.1.1
 * @li Added 64-bit build configurations on Windows.
 * @li Fixed 64-bit rounding bug in tests (#3286061).
 * @li Fixed MS-SSIM buffer overflow bug for images that don't scale to an even multiple of 2 (#3288043). Added corresponding unit test.
 *
 * See CHANGELOG.txt for a detailed list of changes in each version.
 * <br>
 *
*/
