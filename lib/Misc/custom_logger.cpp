#include <custom_logger.h>
#include <stdio.h>

int _log_vprintf(const char *fmt, va_list args)
{
    printf("l");
    return vprintf(fmt, args);
}
