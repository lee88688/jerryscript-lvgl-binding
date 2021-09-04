#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include "emscripten/emscripten.h"
#include "jerryscript-port.h"
#include "jerryscript-ext/debugger.h"

void jerry_port_fatal(jerry_fatal_code_t code) {
  abort();
}

void jerry_port_log(jerry_log_level_t level, const char *format, ...)
{
  va_list args;
  FILE *out = NULL;
  if (level <= JERRY_LOG_LEVEL_WARNING) {
    out = stderr;
  } else {
    out = stdout;
  }

  va_start(args, format);
  vfprintf(out, format, args);
  va_end(args);
}

void jerry_port_print_char(char c)
{
  putchar(c);
}

// date
double jerry_port_get_local_time_zone_adjustment(double unix_ms, bool is_utc)
{
  struct tm tm;
  time_t now = (time_t) (unix_ms / 1000);
  localtime_r (&now, &tm);
  if (!is_utc)
  {
    now -= tm.tm_gmtoff;
    localtime_r (&now, &tm);
  }
  return ((double) tm.tm_gmtoff) * 1000;
}

double jerry_port_get_current_time()
{
  struct timeval tv;

  if (gettimeofday (&tv, NULL) != 0)
  {
    return 0;
  }

  return ((double) tv.tv_sec) * 1000.0 + ((double) tv.tv_usec) / 1000.0;
}

// external context
static jerry_context_t *current_context_p = NULL;

void jerry_port_default_set_current_context (jerry_context_t *context_p) /**< points to the created context */
{
  current_context_p = context_p;
}

jerry_context_t *jerry_port_get_current_context ()
{
  return current_context_p;
}

// sleep
void jerry_port_sleep (uint32_t sleep_time) {
  emscripten_sleep(sleep_time);
}
