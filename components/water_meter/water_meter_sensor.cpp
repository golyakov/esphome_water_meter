#include "water_meter_sensor.h"
#include <utility>
#include "esphome/core/log.h"

namespace esphome
{
  namespace water_meter
  {

    static const char *const TAG = "water_meter";

    void WaterMeterSensor::set_total_pulses(float total)
    {
      this->total_ = total;
      this->eeprom_write(this->total_);
      this->publish_state(this->total_);
    }

    void WaterMeterSensor::setup()
    {
      this->total_ = this->eeprom_read();

      this->pin_->setup();
      this->isr_pin_ = pin_->to_isr();

      this->pin_->attach_interrupt(WaterMeterSensor::edge_intr, this, gpio::INTERRUPT_RISING_EDGE);
    }

    void WaterMeterSensor::loop()
    {
      uint32_t now = micros();
      if (now - this->timer_ticks_ < this->eeprom_save_interval_)
      {
        return;
      }

      this->timer_ticks_ += this->eeprom_save_interval_;

      this->eeprom_write(this->total_);
      this->publish_state(this->total_);
      delay(1);
    }

    float WaterMeterSensor::get_setup_priority() const { return setup_priority::DATA; }

    void WaterMeterSensor::dump_config()
    {
      LOG_SENSOR("", "Water Meter", this);
      LOG_PIN("  Pin: ", this->pin_);
      ESP_LOGCONFIG(TAG, "  Multiply %f", this->total_multiply_);
      LOG_I2C_DEVICE(this);
    }

    void WaterMeterSensor::increment_total()
    {
      this->total_ += this->total_multiply_;
    }

    bool WaterMeterSensor::eeprom_write(float value)
    {
      float prevValue = this->eeprom_read();
      if (value == prevValue)
      {
        return true;
      }

      uint8_t wd[6];

      // set address
      uint8_t *p = &wd[0];
      memcpy(p, &this->eeprom_value_address, sizeof(this->eeprom_value_address));

      // set value
      p = &wd[2];
      memcpy(p, &value, sizeof(value));

      auto err = this->write(wd, sizeof(wd));
      return err == i2c::ERROR_OK;
    }

    float WaterMeterSensor::eeprom_read()
    {
      // set address
      this->write((const uint8_t *)&this->eeprom_value_address, sizeof(this->eeprom_value_address));

      // read value
      float value;
      this->read((uint8_t *)&value, sizeof(value));
      ESP_LOGD(TAG, "Value from eeprom: %f", value);

      return value;
    }

    void IRAM_ATTR WaterMeterSensor::edge_intr(WaterMeterSensor *sensor)
    {
      sensor->increment_total();
    }
  } // namespace water_meter
} // namespace esphome
