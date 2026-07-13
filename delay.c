#include "delay.h"
#include "io.h"
#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

static uint8_t cmos_read(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    io_wait();
    return inb(CMOS_DATA);
}

static int rtc_update_in_progress(void) {return cmos_read(0x0A) & 0x80;}

static uint8_t bcd_to_binary(uint8_t value) {return (uint8_t)((value & 0x0F) + ((value / 16) * 10));}

uint8_t get_rtc_seconds(void) {
    while(rtc_update_in_progress()) {}
    uint8_t seconds = cmos_read(0x00);
    uint8_t status_b = cmos_read(0x0B);
    if(!(status_b & 0x04)) {seconds = bcd_to_binary(seconds);}
    return seconds;
}

void delay_seconds(uint32_t seconds) {
    uint32_t elapsed = 0;
    uint8_t last_seconds = get_rtc_seconds();
    while(elapsed < seconds) {
        uint8_t current_seconds = get_rtc_seconds();
        if(current_seconds != last_seconds) {
            elapsed++;
            last_seconds = current_seconds;
        }
    }
}