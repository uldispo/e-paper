/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PAINT_SENSOR_H
#define __PAINT_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    void temperature_out(uint16_t tempr);
    void humidity_out(uint16_t hum);
    int ESP_Init(void);
    int ESP_Init_after_sleep(void);

    void battery_out(uint16_t bat);
    void final_message(uint16_t bat_voltage);
    void Show_RTC_Calendar(void);

#ifdef __cplusplus
}
#endif

#endif /* __PAINT_SENSOR_H */
