#pragma once

#include <unistd.h>

#define LG_OKAY 0

int lgGpiochipOpen(int gpioDev)
{
    return LG_OKAY;
}

int lgGpioClaimOutput(int handle, int lFlags, int gpio, int level)
{
    return LG_OKAY;
}

int lgGpioClaimInput(int handle, int lFlags, int gpio)
{
    return LG_OKAY;
}

int lgGpioSetDebounce(int handle, int gpio, int debounce_us)
{
    return LG_OKAY;
}

int lgGpioWrite(int handle, int gpio, int level)
{
    return LG_OKAY;
}

int lgGpioRead(int handle, int gpio)
{
    static int64_t iteration = 0;

    int success = iteration%2 == 0 ? -1: LG_OKAY;
    iteration++;

    return success;
}

double lguTime(void)
{
    usleep(100*1000);

    static int64_t iteration = 0;

    double time = iteration%2 == 0 ? 0.0: 0.0006;
    iteration++;

    return time;
}

int lgGpiochipClose(int handle)
{
    return LG_OKAY;
}