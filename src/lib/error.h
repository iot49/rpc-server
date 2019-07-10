#pragma once

#include "esp_err.h"
#include "esp_log.h"

// these check for errors and either report in the log or
// throw a C++ exception (const char*)

void err_check_log(const char* tag, esp_err_t e);
void err_check_throw(esp_err_t e);

