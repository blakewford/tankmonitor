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

#include <string>

#define MQTT_BROKER_HOSTNAME "homeassistant"
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

int g_sock = INVALID;

int main()
{
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

//        mqtt::publish(g_sock, LEVEL, std::to_string(percentage_full).c_str());

        std::string detail = "";
        switch(state)
        {
            case NORMAL:
                break;
            case COOL_DOWN:
                detail += std::to_string(state_count);
                detail += ",Non-ideal. Blocked additional RUN trigger because the level was previously unaffected";
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
    }
}