import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation, pins
from esphome.components import i2c
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_INTERNAL_FILTER,
    CONF_INTERNAL_FILTER_MODE,
    CONF_MULTIPLY,
    CONF_PIN,
    CONF_NUMBER,
    CONF_TIMEOUT,
    CONF_TOTAL,
    CONF_VALUE,
    DEVICE_CLASS_WATER,
    ICON_WATER,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_PULSES,
    UNIT_PULSES_PER_MINUTE,
)
from esphome.core import CORE
from . const import (
    UNIT_COUNT_LITRE
)

CODEOWNERS = ["@golyakov"]
DEPENDENCIES = ["i2c"]

water_meter_ns = cg.esphome_ns.namespace("water_meter")


WaterMeterSensor = water_meter_ns.class_(
    "WaterMeterSensor", sensor.Sensor, cg.Component, i2c.I2CDevice
)

SetTotalPulsesAction = water_meter_ns.class_("SetTotalPulsesAction", automation.Action)


def validate_water_meter_pin(value):
    value = pins.internal_gpio_input_pin_schema(value)
    if CORE.is_esp8266 and value[CONF_NUMBER] >= 16:
        raise cv.Invalid(
            "Pins GPIO16 and GPIO17 cannot be used as pulse counters on ESP8266."
        )
    return value


CONFIG_SCHEMA = sensor.sensor_schema(
    WaterMeterSensor,
    unit_of_measurement=UNIT_COUNT_LITRE,
    icon=ICON_WATER,
    accuracy_decimals=3,
    device_class=DEVICE_CLASS_WATER,
    state_class=STATE_CLASS_TOTAL_INCREASING,
).extend(
    {
        cv.Required(CONF_PIN): validate_water_meter_pin,
        cv.Optional(CONF_MULTIPLY, default=1): cv.float_range(min=0.001),
    }
).extend(i2c.i2c_device_schema(0x50))


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(pin))
    cg.add(var.set_multiply(config[CONF_MULTIPLY]))
    await i2c.register_i2c_device(var, config)


@automation.register_action(
    "water_meter.set_total_pulses",
    SetTotalPulsesAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(WaterMeterSensor),
            cv.Required(CONF_VALUE): cv.templatable(cv.float_),
        }
    ),
)
async def set_total_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_VALUE], args, float)
    cg.add(var.set_total_pulses(template_))
    return var
