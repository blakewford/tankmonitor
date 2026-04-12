#include <cstdint>
#include <ctime>

void handle_pump(float percentage_full)
{
    static time_t last_drain = time(nullptr);

    time_t now = time(nullptr);
    const int32_t HIGH_LEVEL_DRAIN_CUTOFF = 85.0f; // Tank alarms at 90%
    if(percentage_full < HIGH_LEVEL_DRAIN_CUTOFF)
    {
        const int32_t MAX_RUN_TIME = 20; // seconds

        time_t diff = now - last_drain;
        if(diff > MAX_RUN_TIME)
        {
//            mqtt::publish(g_sock, "STOP_GRINDER", "");
        }      
    }
    else
    {
        const int32_t MINIMUM_REFILL_TIME = 60; // seconds
        if((now - last_drain) > MINIMUM_REFILL_TIME) // We can't refill faster than this
        {
//            mqtt::publish(g_sock, "RUN_GRINDER", "");
            last_drain = time(nullptr);
        }
    }
}