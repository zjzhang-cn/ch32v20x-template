
#ifndef __FAMILY_CH32V20x_H
#define __FAMILY_CH32V20x_H
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
void board_init(void);
uint32_t board_millis(void);
void board_led_write(bool state);
#endif /* __FAMILY_CH32V20x_H */


