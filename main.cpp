#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include <unistd.h>

#include <sstream>
#include <string>
#include <fstream>

#ifdef __aarch64__
#include <lgpio.h>
#else
#include "simulate_lgpio.hpp"
#endif

#include "mqtt.h"

#define INVALID_DISTANCE -1
#define CONVERSION_CONSTANT 17150

#define MQTT_BROKER_HOSTNAME "homeassistant"
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

int main()
{
    const int TRIG_PIN = 5;
    const int ECHO_PIN = 22;

    const int MINIMUM_TRIGGER_WIDTH = 10; //us
    const int MINIMUM_PULSE_WIDTH = 150;  //ms

    const int MINIMUM_DISTANCE = 2;
    const int MAXIMUM_DISTANCE = 400; //cm

    bool is_model3 = false;

    std::string line;
    std::ifstream cpuinfo("/proc/cpuinfo");
    while(std::getline(cpuinfo, line))
    {
        if(line.find("Raspberry Pi 3") != std::string::npos)
        {
            is_model3 = true;
            printf("Detected Raspberry Pi Model 3\n");
        }
    }

    const int GPIO_DEVICE_PI_3 = 0;
    const int GPIO_DEVICE_PI_5 = 4;
    int gpio_device = GPIO_DEVICE_PI_3;
    if(!is_model3)
    {
        gpio_device = GPIO_DEVICE_PI_5;
    }

    while(true)
    {
        auto handle = lgGpiochipOpen(gpio_device);
        if(handle < 0)
        {
            printf("Could not get GPIO chip handle!");
            sleep(1);
            continue;
        }

        if(lgGpioClaimOutput(handle, 0, TRIG_PIN, 0) == LG_OKAY
        && lgGpioClaimInput(handle, 0, ECHO_PIN)     == LG_OKAY
        && lgGpioSetDebounce(handle, ECHO_PIN, MINIMUM_PULSE_WIDTH)  == LG_OKAY)
        {
            lgGpioWrite(handle, TRIG_PIN, false);
            lgGpioWrite(handle, TRIG_PIN, true);
            usleep(MINIMUM_TRIGGER_WIDTH);
            lgGpioWrite(handle, TRIG_PIN, false);

            // Wait for pulse to start
            while(!lgGpioRead(handle, ECHO_PIN))
                ;

            double mark = lguTime();
            while(lgGpioRead(handle, ECHO_PIN))
                ;

            float distance = INVALID_DISTANCE;
            const float raw = (lguTime()-mark) * CONVERSION_CONSTANT;
            if(raw > MINIMUM_DISTANCE && raw < MAXIMUM_DISTANCE)
            {
                distance = raw;
            }

            int sock = mqtt::connect(MQTT_BROKER_HOSTNAME, MQTT_USERNAME, MQTT_PASSWORD);
            if(sock == -1) continue;

            printf("%.0f\n", distance);

            mqtt::disconnect(sock);
            close(sock);
        }

        lgGpiochipClose(handle);
    }
}
