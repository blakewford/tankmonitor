#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include <unistd.h>

#include "constants.h"
#include "mqtt.h"
#include "platform.h"
#include "pump.h"
#include "sensor.h"

#define MQTT_BROKER_HOSTNAME "homeassistant"
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

int g_sock = INVALID;

int main()
{
    int gpio_device = get_active_gpio();
    while(true)
    {
        float percentage_full = calculate_liquid_ratio(gpio_device);

        g_sock = mqtt::connect(MQTT_BROKER_HOSTNAME, MQTT_USERNAME, MQTT_PASSWORD);
        if(g_sock == -1) continue;

        //mqtt::publish(g_sock, "LIQUID", buffer);
        printf("%.0f\n", percentage_full);

        handle_pump(percentage_full);

        mqtt::disconnect(g_sock);
        close(g_sock);

        g_sock = INVALID;
    }
}