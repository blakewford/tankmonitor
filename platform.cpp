#include <cstdio>
#include <cstdint>

#include <string>
#include <fstream>

void update_status(float percentage_full, const char* status)
{
    std::string json = "{\"level\":";
    json += std::to_string(percentage_full);
    json += ",\"status\":\"";
    json += status;
    json += "\"}";

    printf("%s\n", json.c_str());
}

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
            update_status(0.0, "Detected Raspberry Pi Model 3");
        }
    }

    if(!is_model3)
    {
        gpio_device = GPIO_DEVICE_PI_5;
    }

    return gpio_device;
}