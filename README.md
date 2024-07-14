##  Ultra-low-power Mini Indoor Thermometer and Hygrometer with E-Paper Display

### Device
<p align="center"> 
  <img src="https://github.com/uldispo/e-paper/blob/main/docs/device.jpg" alt="device image" width="80%" height="80%"/>
</p>

### Board
<p align="center"> 
  <img src="https://github.com/uldispo/e-paper/blob/main/docs/board.jpg" alt="board image" width="80%" height="80%"/>
</p>

### Features
1. STM32U073 controller
2. BME280 temperature sensor
3. 1.54 inch e-paper display
4. AB1805 real-time clock
5. Two SIP32431DR3 switches
6. Current consumption in sleep mode less than 150nA
7. Device is powered from battery CR123A or CR17450

The temperature is measured once a minute. During the sleep period, the RTC microcontroller, temperature sensor, and display are disconnected from the power supply using two SIP32431DR3 switches.

After waking up and initializing, the temperature is measured. If the result differs from the previous measurement, the e-paper display is turned on and the new value is displayed. If the temperature has not changed, the display remains off. The RTC is then programmed to turn on the microcontroller after one minute, and everything goes to sleep except for the RTC. Humidity is output every 15 minutes. Battery voltage is output once an hour.

## Power Consumption

### Sleep Mode Only
<p align="center"> 
  <img src="https://github.com/uldispo/e-paper/blob/main/docs/sleep.PNG" alt="sleep mode image" width="80%" height="80%"/>
</p>

### Sleep, Wakeup, No Output to Display
<p align="center"> 
  <img src="https://github.com/uldispo/e-paper/blob/main/docs/no_output_sleep.PNG" alt="no output sleep mode image" width="80%" height="80%"/>
</p>

### Sleep, Wakeup, Output Data to Display
<p align="center"> 
  <img src="https://github.com/uldispo/e-paper/blob/main/docs/sleep_output_temp.PNG" alt="output data sleep mode image" width="80%" height="80%"/>
</p>

## Hardware

The PCB for this project is the first version. Therefore, there are additional elements on the board for configuration changes and debugging.

### Not Populated:
1. R18 - The e-paper is powered with switch IC5, and it works.
2. Y2, C4, C5 - Precise timing for RTC is not necessary.
3. U1 RST pin 1 is not connected. When U1 was powered up, the pin went low and did not return to high. The low state of this pin blocked the microcontroller startup. I couldn't find a solution for this.

### Debugging and Optional Components:
- J1 header is for debugging.
- IC2, R3, R4 - Optional for the STS40 ultra-low-power temperature sensor (not tested with this sensor).
