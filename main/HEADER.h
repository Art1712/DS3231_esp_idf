/*
 * HEADER.h
 *
 *  Created on: 24 июл. 2025 г.
 *      Author: root
 */

#ifndef MAIN_HEADER_H_
#define MAIN_HEADER_H_


#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include <string.h>
#include "driver/gpio.h"
#include "freertos/projdefs.h"
#include "esp_err.h"

typedef struct {
	uint8_t HOUR;
	uint8_t MINUTE;
	uint8_t SECOND;
	uint8_t WEEK_DAY;
	uint8_t DATA;
	uint8_t MONTH;
	uint8_t YEAR;
	char *W_D;
	char *M_TH;
	float temperature;
	float humidity;
} DAT;


// Prototypes
// I2C
void I2C_init(uint8_t rtc_addr, uint8_t bme280_addr);
void I2C_write(uint8_t dev_number, const uint8_t *p, const uint8_t write_size);
void I2C_read(uint8_t dev_number, uint8_t *p, uint8_t read_size);
void I2C_write_read(uint8_t dev_number, const uint8_t *write, const uint8_t size_write, uint8_t *read, const uint8_t size_read);

// DS3231
void DS3231_init(void);
void DS3231_set_time(uint8_t hour, uint8_t minute, uint8_t second);
void DS3231_set_data(uint8_t week_day, uint8_t data, uint8_t month, uint8_t year);
void DS3231_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second);
void DS3231_get_data(uint8_t *day, uint8_t *data, uint8_t *month, uint8_t *year);
void DS3231_set_alarm1(int8_t sec, int8_t min, int8_t hour, int8_t data, int8_t week_day);
void DS3231_alarm1_deinit(void);
void DS3231_set_alarm2(const int8_t sec, const int8_t min, const int8_t hour, const int8_t data, const int8_t week_day);
void DS3231_alarm2_deinit(void);

// BME280
void BME280_init(void);
double BME280_compensate_T_double(int32_t adc_T);
double BME280_compensate_T_int32(int32_t adc_T);
double BME280_compensate_P_double(int32_t adc_P);
double BME280_compensate_P_int64(int32_t adc_P);
double bme280_compensate_H_double(int32_t adc_H);
double bme280_compensate_H_int32(int32_t adc_H);
void read_calib_Data(void);
uint8_t BME280_ReadStatus(void);
double get_temperature(void);
double get_pressure(void);
double get_humidity(void);

// SD card
void sdspi_init(void);
void file_write(uint8_t hour, uint8_t minute, uint8_t second, uint8_t data, uint8_t month, uint8_t year, float temperature, float humidity);
void SD_write_file_txt(char *str);
void SD_write_file_dat(DAT data);
void sdcard_mount(void);
void sdcard_unmount(void);

// LIGHT SLEEP
esp_err_t Light_Sleep_register_gpio_wakeup(const uint8_t BOOT_BUTTON_NUM, const uint8_t GPIO_WAKEUP_LEVEL);
void Light_Sleep_wait_gpio_inactive(void);
esp_err_t example_register_timer_wakeup(uint16_t us);

void deep_sleep_register_ext0_wakeup(void);

uint8_t DS3231_get_year(void);
void DS3231_set_year(uint8_t year);


#endif /* MAIN_HEADER_H_ */
