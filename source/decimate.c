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

#include "decimate.h"
#include <stdlib.h>

int _iqa_decimate(float *img, int w, int h, int factor, const struct _kernel *k, float *result, int *rw, int *rh)
{
    int x,y;
    int sw = w/factor + (w&1);
    int sh = h/factor + (h&1);
    int err, src_offset, dst_offset;
    float *filtered=img, *dst=img;

    if (result)
        dst = result;

    /* Filter: If no result buffer given, then there is no need to filter into
     * a separate buffer since we are going to overwrite the original image
     * anyways.
     */
    if (k) {
        if (result) {
            filtered = (float*)malloc(w*h*sizeof(float));
            if (!filtered)
                return 1;
            err = _iqa_img_filter(img, w, h, k, filtered);
        }
        else
            err = _iqa_img_filter(img, w, h, k, 0);
        if (err) {
            if (result) free(filtered);
            return err;
        }
    }

    /* Downsample */
    for (y=0; y<sh; ++y) {
        src_offset = y*factor*w;
        dst_offset = y*sw;
        for (x=0; x<sw; ++x,++dst_offset,src_offset+=factor) {
            dst[dst_offset] = filtered[src_offset];
        }
    }
    
    if (k && result)
        free(filtered);
    if (rw) *rw = sw;
    if (rh) *rh = sh;

    return 0;
}
