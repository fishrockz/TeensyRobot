#include "SerialPrint.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <Arduino.h>

static const size_t MAX_PRINT_LINE = 500;
static char PRINT_BUFFER[MAX_PRINT_LINE];

struct ratelimit_entry {
    const char *fmt_string;
    uint32_t last_millis;
};

static const size_t N_RL_ENTRIES = 20;
struct ratelimit_entry RL_ENTRIES[N_RL_ENTRIES] = {0};

void debug_print(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(PRINT_BUFFER, MAX_PRINT_LINE, format, args);
    va_end(args);
    Serial.print(PRINT_BUFFER);
}

void debug_println(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(PRINT_BUFFER, MAX_PRINT_LINE, format, args);
    va_end(args);
    strncat(PRINT_BUFFER, "\n", MAX_PRINT_LINE);
    Serial.print(PRINT_BUFFER);
}

void telemetry_print(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(PRINT_BUFFER, MAX_PRINT_LINE, format, args);
    va_end(args);
    Serial.print(PRINT_BUFFER);
}

static bool check_for_rl(uint32_t gap, const char *format) {
    struct ratelimit_entry *to_fill = RL_ENTRIES;
    //search through cache to find uses of this format string
    for (size_t i = 0; i < N_RL_ENTRIES; i++) {
        if (RL_ENTRIES[i].fmt_string == format) {
            //found it. Check for time and return appropriately
            uint32_t current_millis = millis();
            if (current_millis - RL_ENTRIES[i].last_millis > gap) {
                RL_ENTRIES[i].last_millis = current_millis;
                return true;
            } else {
                return false;
            }
        //Otherwise, look for an entry we can fill in, either an empty one,
        //or the oldest one
        } else if (!RL_ENTRIES[i].fmt_string) {
            to_fill = &RL_ENTRIES[i];
        } else if (RL_ENTRIES[i].last_millis < to_fill->last_millis && to_fill->fmt_string) {
            to_fill = &RL_ENTRIES[i];
        }
    }
    // Didn't find it in cache, fill in the entry and return true
    to_fill->fmt_string = format;
    to_fill->last_millis = millis();
    return true;
}

void debug_print_rl(uint32_t gap, const char *format, ...) {
    if (check_for_rl(gap, format)) {
        va_list args;
        va_start(args, format);
        vsnprintf(PRINT_BUFFER, MAX_PRINT_LINE, format, args);
        va_end(args);
        Serial.print(PRINT_BUFFER);
    }
}

void debug_println_rl(uint32_t gap, const char *format, ...) {
    if (check_for_rl(gap, format)) {
        va_list args;
        va_start(args, format);
        vsnprintf(PRINT_BUFFER, MAX_PRINT_LINE, format, args);
        va_end(args);
        strncat(PRINT_BUFFER, "\n", MAX_PRINT_LINE);
        Serial.print(PRINT_BUFFER);
    }
}
