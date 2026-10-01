/*
 * LED sensor driver - Implementation
 */

#define DT_DRV_COMPAT lv_led_sensor

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include <led_sensor/led_sensor.h>

LOG_MODULE_REGISTER(lv_led_sensor, CONFIG_SENSOR_LOG_LEVEL);

struct led_sensor_config {
    struct gpio_dt_spec led; /* read-only, from DTS */
};

struct led_sensor_data {
    bool led_on; /* last level written to the LED */
};

static int led_sensor_set_led(const struct device *dev, bool on)
{
    const struct led_sensor_config *cfg = dev->config;
    struct led_sensor_data *data = dev->data;

    int ret = gpio_pin_set_dt(&cfg->led, on ? 1 : 0);

    if (ret < 0) {
        LOG_ERR("%s: failed to set LED (%d)", dev->name, ret);
        return ret;
    }

    data->led_on = on;
    return 0;
}

static int led_sensor_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    if (chan != SENSOR_CHAN_ALL && chan != (enum sensor_channel)SENSOR_CHAN_LED_STATE) {
        return -ENOTSUP;
    }

    return led_sensor_set_led(dev, true);
}

static int led_sensor_channel_get(const struct device *dev, enum sensor_channel chan,
                  struct sensor_value *val)
{
    struct led_sensor_data *data = dev->data;

    if (chan != (enum sensor_channel)SENSOR_CHAN_LED_STATE) {
        return -ENOTSUP;
    }

    /* Report the state left by the last fetch, then turn the LED off. */
    val->val1 = data->led_on ? 1 : 0;
    val->val2 = 0;

    return led_sensor_set_led(dev, false);
}

static DEVICE_API(sensor, led_sensor_api) = {
    .sample_fetch = led_sensor_sample_fetch,
    .channel_get = led_sensor_channel_get,
};

static int led_sensor_init(const struct device *dev)
{
    const struct led_sensor_config *cfg = dev->config;
    struct led_sensor_data *data = dev->data;

    if (!gpio_is_ready_dt(&cfg->led)) {
        LOG_ERR("%s: GPIO controller not ready", dev->name);
        return -ENODEV;
    }

    int ret = gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);

    if (ret < 0) {
        LOG_ERR("%s: failed to configure LED pin (%d)", dev->name, ret);
        return ret;
    }

    data->led_on = false;

    LOG_INF("%s ready", dev->name);
    return 0;
}

#define LED_SENSOR_DEFINE(inst)                                         \
    static struct led_sensor_data led_sensor_data_##inst;               \
                                                                        \
    static const struct led_sensor_config led_sensor_config_##inst = {  \
        .led = GPIO_DT_SPEC_GET(DT_INST_PHANDLE(inst, led), gpios),     \
    };                                                                  \
                                                                        \
    DEVICE_DT_INST_DEFINE(inst, led_sensor_init, NULL,                  \
                  &led_sensor_data_##inst,                              \
                  &led_sensor_config_##inst,                            \
                  POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,             \
                  &led_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_DEFINE)
