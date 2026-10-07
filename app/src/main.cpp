#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <led_sensor/led_sensor.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* Number of fetch/get cycles between flips of the inverted mode. */
#define INVERT_EVERY_N_CYCLES 5U

static const struct device *const led_sensor = DEVICE_DT_GET_ANY(lv_led_sensor);

int main(void)
{
    if (!device_is_ready(led_sensor)) {
        LOG_ERR("LED sensor not ready");
        return 0;
    }

    const enum sensor_channel led_chan = static_cast<enum sensor_channel>(SENSOR_CHAN_LED_STATE);
    struct sensor_value state;
    bool inverted = false;
    uint32_t cycle = 0U;

    while (true) {
        if (sensor_sample_fetch(led_sensor) < 0) return 0;
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        if (sensor_channel_get(led_sensor, led_chan, &state) < 0) return 0;
        LOG_INF("LED state after fetch: %d", state.val1);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        cycle++;
        if (cycle % INVERT_EVERY_N_CYCLES == 0U) {
            inverted = !inverted;
            led_sensor_set_inverted(led_sensor, inverted);
            LOG_INF("Inverted mode: %s", inverted ? "ON" : "OFF");
        }
    }
    return 0;
}
