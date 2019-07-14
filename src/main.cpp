#include "sdkconfig.h" 
#include "esp_log.h"
#include "lib/init_rpc.h"


static const char *TAG = "main";


extern "C" void app_main()
{
    ESP_LOGI(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> app_main ...");

    init_rpc();
}
