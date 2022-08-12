/* Code for the box blur filter.
 *
 * Author: Naga Kandasamy
 * Date created: August 8, 2019
 */

#include <stdlib.h>
#include <math.h>
#include "blur_filter.h"

/* Function populates image structure with random FP values. This simulates
 * the acquisition of an image from a sensor in real time. */
void
acquireImage (image_t *img, int max, int min, unsigned int size)
{
   unsigned int i, j;
   img->size = size; // Size of the 2D N x N image along one dimension
   for (i = 0; i < size; i++) {
      for (j = 0; j < size; j++) {
         img->element[i * size + j] = min + (max - min) * rand ()/(float) RAND_MAX;
      }
   }
   return;
}

/* Function applies the box filter operation on the input image.
 * Stores the processed image in out. */
void
blurFilter (const image_t *in, image_t *out)
{
    int pix, i, j;
    int size;
    int row, col;
    int curr_row, curr_col;
    float blur_value;
    int num_neighbors;

    size = in->size;
    out->size = size;

    for (pix = 0; pix < size * size; pix++) { /* Iterate over pixels in image. */
        row = pix/size;             /* Obtain the row number of the pixel. */
        col = pix % size;           /* Obtain the column number of the pixel. */

        /* Apply the blur filter to the current pixel. */
        blur_value = 0.0;
        num_neighbors = 0;
        for (i = -BLUR_SIZE; i < (BLUR_SIZE + 1); i++) {
            for (j = -BLUR_SIZE; j < (BLUR_SIZE + 1); j++) {
                /* Accumulate the values of the neighbors while checking for
                 * boundary conditions. */
                curr_row = row + i;
                curr_col = col + j;
                if ((curr_row > -1) && (curr_row < size) &&\
                        (curr_col > -1) && (curr_col < size)) {
                    blur_value += in->element[curr_row * size + curr_col];
                    num_neighbors += 1;
                }
            }
        }

        /* Write the averaged blurred value out. */
        out->element[pix] = blur_value/num_neighbors;
    }

    return;
}




