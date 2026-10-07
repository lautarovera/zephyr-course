/*
 * LED sensor driver - Public API
 */

#ifndef LED_SENSOR_LED_SENSOR_H_
#define LED_SENSOR_LED_SENSOR_H_

#include <stdbool.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Driver-specific sensor channel: LED state (val1 = 1 on, 0 off). */
enum led_sensor_channel {
    SENSOR_CHAN_LED_STATE = SENSOR_CHAN_PRIV_START,
};

/**
 * Invert the LED behaviour of sample_fetch / channel_get.
 *
 * @param dev       LED sensor device.
 * @param inverted  true: fetch turns the LED off and get turns it on.
 * @return 0 on success.
 */
int led_sensor_set_inverted(const struct device *dev, bool inverted);

#ifdef __cplusplus
}
#endif

#endif /* LED_SENSOR_LED_SENSOR_H_ */
