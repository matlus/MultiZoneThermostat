# MultizoneThermostat
At its core, the Matlus Multi-Zone Thermostat allows you to control the temperature of specific rooms in your home. The Thermostat has a sensor of its own, but it can be set so that it controls the HVAC unit based upon the temperature of other room in your home. Essentially, you have the main unit or the “thermostat”. The code in this repo is for the main unit. This main unit has a temperature sensor. In addition, you can have ancillary units (microcontrollers with just temperature sensors) in other rooms (zones) in your home that you’d like the thermostat to use as “input” and control the HVAC unit based on the temperature in any one of the other zones. An MQTT broker is the central communication hub. All devices communicate via this MQTT broker. The MQTT broker could be local to your home network on one that is on the Internet.

### Overview
For an overview of What and How about this Thermostat, please take a look at this video on YouTube.

[![Alexa Demystified - Explaining the Flow with an IoT Thermostat](http://img.youtube.com/vi/ITUisKjxcCc/0.jpg)](https://youtu.be/ITUisKjxcCc)

### MultizoneThermostatMain.ino
This is the main file/class that has the setup and loop functions. This file also includes some of the classes from the Matlus Multi-Zone Thermostat library. There are settings you need to change in this file prior to flashing your microcontroller.

`mqttBrokerUrl` – Change this to the URL of the MQTT broker you’re using. There is a free (public domain MQTT broker you can use. If you’d like to use this initially, set the value of the mqttBrokerUrl to `iot.eclipse.org`.

`commandTopic` – All topics need to be *unique* across the entire MQTT broker. So if you’re using a public domain MQTT broker, I recommend prefixing a unique identifier of your choosing to this and the other topic names. Let’s say you decide on the prefix – `zeus`. Then the `commandTopic` should be set to: `zeus/thermostat/cmd`. Use the prefix for the `eventTopic` and `informationTopic` variables as well.

### Publishing Temperature and Information
By default, the thermostat publishes its temperature and other information every 5 seconds. This can be changed by changing the `publishInterval` variable in the `MultizoneThermostatMain.ino` file.

#### The `publishTemperatureAndHumidity()` method
The method `publishTemperatureAndHumidity()` publishes the thermostat’s onboard sensor’s data to the eventTopic. This information is in the format: `Z=Family Room&T=72.34&H=56.42`

In fact, all remote sensors should publish their temperature (and humidity) data to the eventTopic in exactly the same format since the thermostat subscribes to the eventTopic and thus receives this data and maintains a list of all available zones.

#### Changing settings on the Thermostat Unit
You change settings of the thermostat unit by publishing messages to the `commandTopic`. The format of messages sent to the command topic is as follows:
`cmd=set&Z=Master Bedroom&M=Heat&T=68`
This command sets the *Zone* to **Master Bedroom**. The *Mode* to **Heat** and the *Temperature* to **68.00**. Of course this assumes that there is a zone called Master Bedroom. After receiving this message, the thermostat will use the temperature sensor information it is receiving from the Master Bedroom to control the HVAC unit as per set temperature while comparing it with the temperature in the Master Bedroom.
