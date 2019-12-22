#include "settings.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "lib/cpp_main.h"

static const char *TAG = "main";


// just a stub calling the actual C++ main
extern "C" void app_main()
{
    ESP_LOGI(TAG, ">>>>>>>>>>>>>>>> app_main --> C++ main");

    cpp_main();
}
