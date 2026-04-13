#include <string>
#include <fstream>

#include "constants.h"
#include "report.h"

int get_active_gpio()
{
    const int32_t GPIO_DEVICE_PI_3 = 0;
    const int32_t GPIO_DEVICE_PI_5 = 4;

    int gpio_device = GPIO_DEVICE_PI_3;

    std::string line;
    bool is_model3 = false;
    std::ifstream cpuinfo("/proc/cpuinfo");
    while(std::getline(cpuinfo, line))
    {
        if(line.find("Raspberry Pi 3") != std::string::npos)
        {
            is_model3 = true;
            std::string detail = "Detected Raspberry Pi Model 3";
            update_status(0.0, UNKNOWN, detail.c_str());
        }
    }

    if(!is_model3)
    {
        gpio_device = GPIO_DEVICE_PI_5;
    }

    return gpio_device;
}