/* Header file.
 *
 *  Created on: Jun 22, 2019
 *  Author: Naga Kandasamy
 */

#ifndef UART_FUNCTIONS_H_
#define UART_FUNCTIONS_H_

#define DEBUG_FLAG 1 // Debug flag. Set to 0 to disable, 1 to enable
#define BUFFER_SIZE 32 // Maximum buffer size


/* Function prototypes. */
void writeString (char *buffer);
void writeInt (int value);
void writeFloat (float value);
void readString (char *buffer);
void readInt (int *value);
void readFloat(float *value);
void initUART (void);

#endif /* UART_FUNCTIONS_H_ */
