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
 * @page algorithms Algorithms
 * All of the algorithms described here are called full-reference algorithms. This means they required the original undistorted image to compare the distorted image against.
 * 
 * <br>
 * <hr>
 * @section mse MSE
 * Mean Squared Error is the average squared difference between a reference image and a distorted image. It is computed pixel-by-pixel by adding up the squared differences of all the pixels and dividing by the total pixel count.
 *
 * For images A = {a1 .. aM} and B = {b1 .. bM}, where M is the number of pixels:
 * @image html mse_eq.jpg
 * 
 * The squaring of the differences dampens small differences between the 2 pixels but penalizes large ones. 
 *
 * More info: http://en.wikipedia.org/wiki/Mean_squared_error
 *
 * <br>
 * <hr>
 * @section psnr PSNR
 * Peak Signal-to-Noise Ratio
 *
 * @image html psnr_eq.jpg
 *
 * More info: http://en.wikipedia.org/wiki/PSNR
 *
 * <br>
 * <hr>
 * @section ssim SSIM
 * Structural SIMilarity
 *
 * Original paper: https://ece.uwaterloo.ca/~z70wang/publications/ssim.html
 *
 * <br>
 * <hr>
 * @section ms_ssim MS-SSIM
 * Multi-Scale Structural SIMilarity
 *
 * Original paper: https://ece.uwaterloo.ca/~z70wang/publications/msssim.pdf
 *
 * <br>
 * <hr>
 * @section ms_ssim_star MS-SSIM*
 *
 * Original paper: http://foulard.ece.cornell.edu/publications/dmr_hvei2008_paper.pdf
 *
 */
