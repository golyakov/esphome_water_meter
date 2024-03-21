# ESPHome Water Meter

## Example configuration:
```yaml
external_components:
  - source:
      type: git
      url: https://github.com/golyakov/esphome_water_meter
      ref: master
    components: [ water_meter ]

api:
  services:
    - service: set_total
      variables:
        new_total: float
      then:
        - water_meter.set_total_pulses:
            id: sensor_water_meter
            value: !lambda 'return new_total;'

i2c:
  sda: 4
  scl: 5

sensor:
  - platform: water_meter
    pin:
      number: GPIO13
      mode:
        input: true
        pullup: true
    multiply: 1
    name: "Water Meter"
    id: sensor_water_meter
```