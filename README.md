# Mini Indoor Thermometer, Hygrometer with E-Paper Display

![Project Image](path/to/your/image.png)

Features:
1. STM32U073 controller
2. BME280 temperature sensor
3. 1.54 inch e-paper display
4. AB1805 real-time clock
5. Two SIP32431DR3 switches
6. Current consumption in sleep mode less 150nA


The temperature is measured once a minute. During the rest of the time, the RTC microcontroller, temperature sensor, and display are disconnected from the power supply. Two SIP32431DR3 switches are used for this purpose. 

After waking up and initializing, the temperature is measured. If the result differs from the previous measurement, the e-paper display is turned on and the new value is displayed. If the temperature has not changed, the display remains off. The RTC is then programmed to turn on the microcontroller after one minute, and everything is turned off except for the RTC.
Humidity is output once a 15 minutes. Battery voltage is output once a hour.

## Power Consumption

## Sleep mode
<p align="center"> <img 
src="https://github.com/uldispo/e-paper/blob/main/docs/sleep.PNG"
alt="drawing"  width="80%" height="80%"/></p>                     
Sleep mode

## Sleep, no output
<p align="center"> <img
src="https://github.com/uldispo/e-paper/blob/main/docs/no_output_sleep.PNG"
alt="drawing"  width="80%" height="80%"/></p>                     
Sleep no output

## Sleep output
<p align="center"> <img
src="https://github.com/uldispo/e-paper/blob/main/docs/sleep_output_temp.PNG"
alt="drawing"  width="80%" height="80%"/></p>                     
Sleep output temperature
