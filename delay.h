#ifndef DELAY_H
#define DELAY_H
#include <stdint.h>

void delay_seconds(uint32_t seconds);
uint8_t get_rtc_seconds(void);

#endif