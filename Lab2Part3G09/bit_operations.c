/* Functions that implement bit-level operations
 *
 */

#include <stdint.h>
#include "bit_operations.h"

uint8_t countBits (uint8_t v)
{
    uint8_t count = 0;

    uint8_t copy, index, mask;

    for (index=0; index<8; index++) {
        mask = 1;
        copy = (mask << index)^v;
        if (copy < v) {
            count++;
        }
    }

    return count;
}

int isPowerOfTwo (uint8_t v)
{
    int status = 1;
    uint8_t index;
    for (index=0; index<8; index++) {
        if ( ((v % 2) & (v != 1)) | (v==0) ) {
            status = 0;
            break;
        }
        v = v/2;
    }

    return status;
}

uint8_t rearrangeBits (uint8_t v)
{
    uint8_t rv = 0;
    rv = (-1 << 8-countBits(v) ); /* bitshift 11111111 (-1) to the left for each 0 in v ( 8 - countBits(v) ) */

    return rv;
}
