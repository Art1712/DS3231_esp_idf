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

// Prototypes
// I2C
void I2C_init(uint8_t dev_addr);
void I2C_write(const uint8_t *p, const uint8_t write_size);
void I2C_read(uint8_t *p, uint8_t read_size);
void I2C_write_read(const uint8_t *write, const uint8_t size_write, uint8_t *read, const uint8_t size_read);

// DS3231
void DS3231_init(void);
void DS3231_set_time(uint8_t hour, uint8_t minute, uint8_t second);
void DS3231_set_data(uint8_t week_day, uint8_t data, uint8_t month, uint8_t year);
void DS3231_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second);
void DS3231_get_data(uint8_t *day, uint8_t *data, uint8_t *month, uint8_t *year);
void DS3231_set_alarm1(int8_t sec, int8_t min, int8_t hour, int8_t data, int8_t week_day);
void DS3231_set_alarm2(const int8_t sec, const int8_t min, const int8_t hour, const int8_t data, const int8_t week_day);
void DS3231_alarm1_deinit(void);
void DS3231_alarm2_deinit(void);



#endif /* MAIN_HEADER_H_ */
