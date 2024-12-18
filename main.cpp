#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <lgpio.h>
#include <unistd.h>

#define INVALID_DISTANCE -1
#define CONVERSION_CONSTANT 17150

int main()
{
    const int GPIO_DEVICE = 4;

    const int TRIG_PIN = 5;
    const int ECHO_PIN = 22;

    const int MINIMUM_TRIGGER_WIDTH = 10; //us
    const int MINIMUM_PULSE_WIDTH = 150;  //ms

    const int MINIMUM_DISTANCE = 2;
    const int MAXIMUM_DISTANCE = 400; //cm

    while(true)
    {
        auto handle = lgGpiochipOpen(GPIO_DEVICE);
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
            double spent = 0;
            double start = lguTimestamp();
            while(!lgGpioRead(handle, ECHO_PIN))
            {
                spent = (lguTimestamp() - start)/(1000*1000);
            }

            printf("Spent %f\n", spent);

            spent = 0;
            start = lguTimestamp();
            double mark = lguTime();
            while(lgGpioRead(handle, ECHO_PIN))
            {
                spent = (lguTimestamp() - start)/(1000*1000);
            }

            printf("Spent %f\n", spent);

            float distance = INVALID_DISTANCE;
            const float raw = (lguTime()-mark) * CONVERSION_CONSTANT;
            if(raw > MINIMUM_DISTANCE && raw < MAXIMUM_DISTANCE)
            {
                distance = raw;
            }
            printf("%.0f\n", distance);
        }
        lgGpiochipClose(handle);
    }
}
