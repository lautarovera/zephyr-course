/*
 * Shell commands for the LED sensor driver
 */

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

#include <led_sensor/led_sensor.h>

static const struct device *const led_sensor = DEVICE_DT_GET_ANY(lv_led_sensor);

/* Valid values for "sensor set": 0 = normal, 1 = inverted. */
#define SET_VALUE_MIN 0
#define SET_VALUE_MAX 1

static bool sensor_is_ready(const struct shell *sh)
{
    if (!device_is_ready(led_sensor)) {
        shell_error(sh, "LED sensor not ready");
        return false;
    }

    return true;
}

static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (!sensor_is_ready(sh)) {
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(led_sensor);

    if (ret < 0) {
        shell_error(sh, "sensor_sample_fetch failed (%d)", ret);
        return ret;
    }

    shell_print(sh, "Sample fetched");
    return 0;
}

static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (!sensor_is_ready(sh)) {
        return -ENODEV;
    }

    struct sensor_value val;
    int ret = sensor_channel_get(led_sensor, (enum sensor_channel)SENSOR_CHAN_LED_STATE, &val);

    if (ret < 0) {
        shell_error(sh, "sensor_channel_get failed (%d)", ret);
        return ret;
    }

    shell_print(sh, "LED state after last fetch: %d", val.val1);
    return 0;
}

static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (led_sensor == NULL) {
        shell_error(sh, "No lv,led-sensor node in the devicetree");
        return -ENODEV;
    }

    shell_print(sh, "Device: %s", led_sensor->name);
    shell_print(sh, "Ready:  %s", device_is_ready(led_sensor) ? "yes" : "no");
    return 0;
}

static int cmd_sensor_set(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);

    int err = 0;
    long value = shell_strtol(argv[1], 10, &err);

    if (err != 0 || value < SET_VALUE_MIN || value > SET_VALUE_MAX) {
        shell_error(sh, "Invalid value '%s': expected %d (normal) or %d (inverted)",
                    argv[1], SET_VALUE_MIN, SET_VALUE_MAX);
        return -EINVAL;
    }

    if (!sensor_is_ready(sh)) {
        return -ENODEV;
    }

    int ret = led_sensor_set_inverted(led_sensor, value == SET_VALUE_MAX);

    if (ret < 0) {
        shell_error(sh, "led_sensor_set_inverted failed (%d)", ret);
        return ret;
    }

    shell_print(sh, "Inverted mode: %s", value == SET_VALUE_MAX ? "ON" : "OFF");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD(fetch, NULL, "Fetch a sample (sensor_sample_fetch).", cmd_sensor_fetch),
    SHELL_CMD(read, NULL, "Read the LED state channel (sensor_channel_get).", cmd_sensor_read),
    SHELL_CMD(info, NULL, "Show device name and ready state.", cmd_sensor_info),
    SHELL_CMD_ARG(set, NULL, "Set inverted mode: set <0|1>.", cmd_sensor_set, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sub_sensor, "LED sensor commands", NULL);
