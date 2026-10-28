#ifndef ANZAR_IO_H
#define ANZAR_IO_H

#include <stdbool.h>
#include <stdint.h>

void anzar_io_write(const char *s, uint16_t len);
void anzar_io_print(const char *s);
void anzar_io_led(bool on);
void anzar_io_buzz(uint32_t ms);

/* Reports the error, then blinks the LED fast forever. */
void anzar_io_fatal(const char *why);

#endif
