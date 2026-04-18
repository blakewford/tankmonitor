#include <cstdio>
#include <cstdint>
#include <ctime>

#include "constants.h"
#include "mqtt.h"

extern int g_sock;

PUMP_STATE handle_pump(float percentage_full)
{
    PUMP_STATE state = UNKNOWN;

    static time_t last_drain = time(nullptr);

    time_t now = time(nullptr);
    const int32_t HIGH_LEVEL_DRAIN_CUTOFF = 70.0f; // Tank alarms around here

    time_t diff = now - last_drain;
    if(percentage_full < HIGH_LEVEL_DRAIN_CUTOFF)
    {
        const int32_t MAX_RUN_TIME = 15; // seconds
        if(diff > MAX_RUN_TIME)
        {
            state = NORMAL;
        }
        else
        {
            // Report an accurate pump state past the start point
            state = RUNNING;
        }
    }
    else
    {
        const int32_t MINIMUM_REFILL_TIME = 60; // seconds
        if(diff > MINIMUM_REFILL_TIME) // We can't refill faster than this
        {
            mqtt::publish(g_sock, RUN, "");
            last_drain = time(nullptr);
            state = RUNNING;
        }
        else
        {
            state = COOL_DOWN;
        }
    }

    return state;
}
