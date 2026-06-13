#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <unistd.h>

#include "constants.h"
#include "mqtt.h"
#include "platform.h"
#include "pump.h"
#include "report.h"
#include "sensor.h"
#include "wion.h"

#include <string>

const char* DEVICE_SERIAL = "";

#define MQTT_BROKER_HOSTNAME "homeassistant"
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

int g_sock = INVALID;
int g_lockout = false;

int main()
{
    usleep(PACE_MS*1000);

    wion::discover(DEVICE_SERIAL);

    int32_t state_count = 0;
    auto previous_state = UNKNOWN;
    int gpio_device = get_active_gpio();
    while(true)
    {
        float percentage_full = calculate_liquid_ratio(gpio_device)*100;

        g_sock = mqtt::connect(MQTT_BROKER_HOSTNAME, MQTT_USERNAME, MQTT_PASSWORD);
        if(g_sock == -1) continue;

        auto state = handle_pump(percentage_full);
        state_count = (previous_state == state) ? ++state_count: 0;
        previous_state = state;

        mqtt::publish(g_sock, LEVEL, std::to_string(percentage_full).c_str());

        std::string detail = "";
        switch(state)
        {
            case NORMAL:
                g_lockout = false;
                if(state_count%15 == 0)
                {
                    mqtt::publish(g_sock, ESTOP, "");
                }
                break;
            case INEFFECTIVE:
                detail += std::to_string(state_count);
                detail += ",Non-ideal. Blocked additional RUN triggers because the level was previously unaffected";
                g_lockout = true;
                break;
            case RUNNING:
            case UNKNOWN:
            default:
                detail += std::to_string(state_count);
                break;
        }

        std::string status = STATE_STRINGS[state];
        update_status(percentage_full, state, detail.c_str());

        mqtt::disconnect(g_sock);
        close(g_sock);

        g_sock = INVALID;

        usleep(PACE_MS*1000); // No need to run any faster
    }
}
