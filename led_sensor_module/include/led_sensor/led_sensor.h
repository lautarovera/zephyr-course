/*
 * LED sensor driver - Public API
 */

#ifndef LED_SENSOR_LED_SENSOR_H_
#define LED_SENSOR_LED_SENSOR_H_

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Driver-specific sensor channel: LED state (val1 = 1 on, 0 off). */
enum led_sensor_channel {
    SENSOR_CHAN_LED_STATE = SENSOR_CHAN_PRIV_START,
};

#ifdef __cplusplus
}
#endif

#endif /* LED_SENSOR_LED_SENSOR_H_ */
