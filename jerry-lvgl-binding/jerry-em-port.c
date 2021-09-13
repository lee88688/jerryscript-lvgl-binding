#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include "emscripten/emscripten.h"
#include "emscripten/websocket.h"
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

// ----- debugger ------
typedef struct
{
  jerry_debugger_transport_header_t header; /**< transport header */
  EMSCRIPTEN_WEBSOCKET_T websocket;
} jerry_em_debugger_transport_t;

typedef struct debugger_received_data {
  uint32_t length;
  uint8_t *data;
} debugger_received_data;

static debugger_received_data received_data = {
  .length = 0,
  .data = NULL
};

static const EmscriptenWebSocketCreateAttributes attr = {
  .url = "ws://127.0.0.1:9090",
  .protocols = NULL,
  .createOnMainThread = 0
};

static EMSCRIPTEN_WEBSOCKET_T websocket = -1;
static bool is_open = false;

EM_BOOL em_websocket_onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData) {
  printf("websocket opened!\n");
  is_open = true;
}

EM_BOOL em_websocket_message(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData) {
  received_data.data = realloc(received_data.data, received_data.length + websocketEvent->numBytes);
  memcpy(received_data.data + received_data.length, websocketEvent->data, websocketEvent->numBytes);
}

static bool jerry_em_debugger_send(jerry_debugger_transport_header_t *header, uint8_t *message, size_t len) {
  printf("send %d bytes.\n", len);
  EMSCRIPTEN_RESULT result = emscripten_websocket_send_binary(((jerry_em_debugger_transport_t *)header)->websocket, message, len);
  printf("send result: %d\n", result);
  return result > 0;
}

static bool jerry_em_debugger_receive(jerry_debugger_transport_header_t *header, jerry_debugger_transport_receive_context_t *context) {
  printf("receive bytes.\n");
  // EMSCRIPTEN_WEBSOCKET_T websocket = ((jerry_em_debugger_transport_t *) header)->websocket;

  uint8_t *buffer = context->buffer_p + context->received_length;
  size_t buffer_size = JERRY_DEBUGGER_TRANSPORT_MAX_BUFFER_SIZE - context->received_length;

  while (!received_data.length) {
    emscripten_sleep(10); // data can be received when sleep
  }
  if (buffer_size >= received_data.length) {
    buffer_size = received_data.length;
  }
  memcpy(buffer, received_data.data, buffer_size);
  size_t reserved_size = received_data.length - buffer_size;
  memcpy(received_data.data, received_data.data + buffer_size, reserved_size);
  received_data.data = realloc(received_data.data, reserved_size);
  received_data.length = reserved_size;
  return true;
}

static void jerry_em_debugger_close(jerry_debugger_transport_header_t *header) {
  printf("close!\n");
}

bool jerry_em_debugger_create() {
  EMSCRIPTEN_WEBSOCKET_T socket = emscripten_websocket_new(&attr);
  if (socket < 0) {
    return false;
  }
  emscripten_websocket_set_onopen_callback(socket, NULL, em_websocket_onopen);
  emscripten_websocket_set_onmessage_callback(socket, NULL, em_websocket_message);
  printf("wait for open, before\n");
  for (int i = 0; i < 1000; i++) {
    if (is_open) {
      break;
    }
    printf("i: %d\n");
    emscripten_sleep(10);
  }
  printf("wait for open, after\n");
  if (!is_open) {
    printf("waiting for open fails!\n");
    return false;
  }
  
  jerry_debugger_transport_header_t *header = jerry_heap_alloc(sizeof(jerry_em_debugger_transport_t));
  if (!header) return false;
  
  ((jerry_em_debugger_transport_t *) header)->websocket = socket;
  header->send = jerry_em_debugger_send;
  header->receive = jerry_em_debugger_receive;
  header->close = jerry_em_debugger_close;

  jerry_debugger_transport_add(header, 0, JERRY_DEBUGGER_TRANSPORT_MAX_BUFFER_SIZE, 0, JERRY_DEBUGGER_TRANSPORT_MAX_BUFFER_SIZE);
  return true;
}
