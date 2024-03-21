#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "water_meter_sensor.h"

namespace esphome
{

  namespace water_meter
  {

    template <typename... Ts>
    class SetTotalPulsesAction : public Action<Ts...>
    {
    public:
      SetTotalPulsesAction(WaterMeterSensor *water_meter) : water_meter_(water_meter) {}

      TEMPLATABLE_VALUE(float, total_pulses)

      void play(Ts... x) override { this->water_meter_->set_total_pulses(this->total_pulses_.value(x...)); }

    protected:
      WaterMeterSensor *water_meter_;
    };

  } // namespace water_meter
} // namespace esphome
