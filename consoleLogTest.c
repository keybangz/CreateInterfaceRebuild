#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logger.h>
#include <loggerconf.h>

int main(void) {
  logger_initConsoleLogger(stderr);
  logger_setLevel(LogLevel_DEBUG);
  LOG_INFO("Console logging test...\n");

  return 0;
}
