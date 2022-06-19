**MTTQ:**

The device must first publish on its configuration topic, this should be done only on startup.
Example for energy:
* Topic: "homeassistant/sensor/testkWh/config" 
* JSON payload: '{"name": "testkWh", "device_class": "energy", "unit_of_measurement":"kWh", "value_template":"{{ value_json.energy }}", "state_topic": "test/testkWh"}'

Once the configuration is published, the device can update its state:
* Topic: "test/testkWh" 
* JSON payload: '{"energy":120.1}'

Configuration example for Power:
* Topic: "homeassistant/sensor/testPower/config" 
* JSON payload: '{"name": "testPower", "device_class": "power", "unit_of_measurement":"W", "value_template":"{{ value_json.power }}", "state_topic": "test/testPower"}'

State update:
* Topic: "test/testPower" 
* JSON payload: '{"power":1972}'

Note that it is possible for the device to combine the states for energy and power. The state_topic must be shared (e.g. "test/testMeter"). 
* Topic: "test/testMeter" 
* JSON payload: '{"energy":120.1, "power":1972}'


