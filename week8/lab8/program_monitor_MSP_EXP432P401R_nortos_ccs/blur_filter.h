/* Header file for the blur filter program.
 * Author: Naga Kandasamy
 * Date: August 8, 2019
 */

#ifndef BLUR_FILTER_H_
#define BLUR_FILTER_H_

/* Define the half-width of the box blur filter. */
#define BLUR_SIZE 1

/* Define the size of the image as well as the min and max pixel
 * intensity values in it.
 */
#define SIZE 75  // Size of the 2D N x N image in number of pixels along one dimension
#define MIN_VALUE 5
#define MAX_VALUE 10

/* Define the structure for our image or 2D array. */
typedef struct image_s {
    int size; /* Size of one side of the image. */
    float element[SIZE * SIZE]; /* Individual pixels in the image. */
} image_t;

/* Function definitions. */
void acquireImage (image_t *, int, int, unsigned int);
void blurFilter (const image_t *, image_t *);

#endif /* BLUR_FILTER_H_ */
