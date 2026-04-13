#include <cstdio>
#include <cstdint>

#include <unistd.h>

#ifdef __aarch64__
#include <lgpio.h>
#else
#include "simulate_lgpio.hpp"
#endif

#include <string>

#include "constants.h"
#include "report.h"

float calculate_liquid_ratio(int gpio_device)
{
    const int TRIG_PIN = 5;
    const int ECHO_PIN = 22;
    const int MINIMUM_PULSE_WIDTH = 150;  //ms

    const int32_t MINIMUM_DISTANCE = 2;
    const int32_t MAXIMUM_DISTANCE = 400; //cm
    const int32_t MINIMUM_TRIGGER_WIDTH = 10; //us

    float percentage_full = 0.0f;
    auto handle = lgGpiochipOpen(gpio_device);
    if(handle < 0)
    {
        std::string detail = "Could not get GPIO chip handle!";
        update_status(0.0, UNKNOWN, detail.c_str());
        sleep(1); // Expected cycle time for the main loop
        return percentage_full;
    }

    float distance = INVALID;
    if(lgGpioClaimOutput(handle, 0, TRIG_PIN, 0) == LG_OKAY
    && lgGpioClaimInput(handle, 0, ECHO_PIN)     == LG_OKAY
    && lgGpioSetDebounce(handle, ECHO_PIN, MINIMUM_PULSE_WIDTH)  == LG_OKAY)
    {
        lgGpioWrite(handle, TRIG_PIN, false);
        lgGpioWrite(handle, TRIG_PIN, true);
        usleep(MINIMUM_TRIGGER_WIDTH);
        lgGpioWrite(handle, TRIG_PIN, false);

        // Wait for pulse to start
        int32_t spins = 0;
        const int32_t MAXIMUM_SPINS = 512;
        while(!lgGpioRead(handle, ECHO_PIN))
        {
            spins++;
            if(spins > MAXIMUM_SPINS)
            {
                // Effectively timed out
                lgGpiochipClose(handle);
                return percentage_full;
            }
        }

        double mark = lguTime();
        while(lgGpioRead(handle, ECHO_PIN))
            ;

        const int32_t CONVERSION_CONSTANT = 17150;
        const float raw = (lguTime()-mark) * CONVERSION_CONSTANT;
        if(raw > MINIMUM_DISTANCE && raw < MAXIMUM_DISTANCE)
        {
            distance = raw;
        }
    }

    lgGpiochipClose(handle);

    if(distance != INVALID)
    {
        percentage_full = (TANK_DEPTH-distance)/TANK_DEPTH;
    }

    return percentage_full;
}