/*
 * def.h
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */

#ifndef SRC_COMMON_DEF_H_
#define SRC_COMMON_DEF_H_


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define _DEF_LED1           0
#define _DEF_LED2           1
#define _DEF_LED3           2
#define _DEF_LED4           3

#define _DEF_UART1          0
#define _DEF_UART2          1
#define _DEF_UART3          2
#define _DEF_UART4          3

#define _DEF_BUTTON1        0
#define _DEF_BUTTON2        1
#define _DEF_BUTTON3        2
#define _DEF_BUTTON4        3


#define _DEF_INPUT             0
#define _DEF_INPUT_PULL_UP     1
#define _DEF_INPUT_PULL_DOWN   2
#define _DEF_OUTPUT            3
#define _DEF_OUTPUT_PULL_UP    4
#define _DEF_OUTPUT_PULL_DOWN  5


#define _DEF_LOW      0
#define _DEF_HIGH     1


#define _DEF_SPI1          0
#define _DEF_SPI2          1
#define _DEF_SPI3          2



#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif


#ifndef map
#define map(value, in_min, in_max, out_min, out_max) ((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#endif
typedef struct
{
  uint8_t version[32];
  uint8_t name[32];
} firm_version_t;

typedef struct
{
  uint8_t version[32];
  uint8_t name[32];
} boot_version_t;
#define MAGIC_NUMBER      0x5555AAAA

#endif /* SRC_COMMON_DEF_H_ */
