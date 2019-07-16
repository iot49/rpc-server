#pragma once

#include <stdint.h>
#include "Arduino.h"

uint32_t getHeapSize();

/* log levels:
        0  NONE
        1  ERROR
        2  WARN
        3  INFO
        4  DEBUG
        5  VERBOSE
 */
void setLogLevel(String tag, int level);

String task_list();

int delay_ms(int ms);

void reset();
String reset_reason();