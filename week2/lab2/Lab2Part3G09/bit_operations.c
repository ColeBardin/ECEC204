/* Functions that implement bit-level operations
 *
 */

#include <stdint.h>
#include "bit_operations.h"

uint8_t countBits (uint8_t v)
{
    uint8_t count = 0;

    uint8_t copy;
    uint8_t index;
    uint8_t mask;

    for (index=0; index<8; index++) {
        mask = 1;
        copy = (mask << index)^v;
        if (copy < v) {
            count++;
        }
    }
    /* FIXME: Write your code here. */

    return count;
}

int isPowerOfTwo (uint8_t v)
{
    int status = 0;

    /* FIXME: Write your code here. */

    return status;
}

uint8_t rearrangeBits (uint8_t v)
{
    uint8_t rv = 0;

    /* FIXME: Write your code here. */

    return rv;
}
