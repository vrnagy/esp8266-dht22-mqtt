ESP8266 based Temperature and Humidity sensor with MQTT and DHT22
==============================

The default configuration uses the chip id as the MQTT channel.

Example configuration for Home Assistant:
```yml
sensor:
  - platform: mqtt
    state_topic: "esp/46812306"
    name: "Room temperature"
    unit_of_measurement: "°C"
    value_template: '{{ value_json.temperature }}'

  - platform: mqtt
    state_topic: "esp/46812306"
    name: "Room humidity"
    unit_of_measurement: "%"
    value_template: '{{ value_json.humidity }}'
```