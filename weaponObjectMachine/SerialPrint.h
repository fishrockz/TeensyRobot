#ifndef SERIAL_PRINT_H
#define SERIAL_PRINT_H

#include <stdint.h>
#include <Stream.h>

void debug_print(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void debug_println(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void debug_print_rl(uint32_t gap, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
void debug_println_rl(uint32_t gap, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
void telemetry_print(const char *format, ...) __attribute__ ((format (printf, 1, 2)));

#endif
