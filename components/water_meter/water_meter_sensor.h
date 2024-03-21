#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/components/i2c/i2c.h"

#include <cinttypes>

namespace esphome
{
  namespace water_meter
  {

    class WaterMeterSensor : public sensor::Sensor, public Component, public i2c::I2CDevice
    {
    public:
      void set_pin(InternalGPIOPin *pin) { this->pin_ = pin; }
      void set_multiply(float multiply) { this->total_multiply_ = multiply; }

      void set_total_pulses(float total);

      void setup() override;
      void loop() override;
      float get_setup_priority() const override;
      void dump_config() override;

    protected:
      static void edge_intr(WaterMeterSensor *sensor);

      InternalGPIOPin *pin_{nullptr};

      // Only use these variables in the ISR
      ISRInternalGPIOPin isr_pin_;

    private:
      uint32_t timer_ticks_ = 0;
      float total_ = 0;
      float total_multiply_ = 1;
      uint32_t eeprom_save_interval_ = 60000000;
      uint16_t eeprom_value_address = 0x0000;
      void increment_total();
      bool eeprom_write(float value);
      float eeprom_read();
    };

  } // namespace water_meter
} // namespace esphome
