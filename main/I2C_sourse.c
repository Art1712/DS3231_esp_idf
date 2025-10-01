/*
* I2C_sourse.c
*
*  Created on: 16 июл. 2025 г.
*      Author: root
*/


#include "HEADER.h"
#include "driver/i2c_master.h"

#define I2C_PORT	-1
#define I2C_SCL		22
#define I2C_SDA		21

// Переменные I2C 
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t rtc_handle;
i2c_master_dev_handle_t bme280_handle;
//===================================================================================================================
void I2C_init(uint8_t dev_addr)
{
	i2c_master_bus_config_t i2c_mst_config = {
	.clk_source = I2C_CLK_SRC_APB,
	.i2c_port = I2C_PORT,
	.scl_io_num = I2C_SCL,
	.sda_io_num = I2C_SDA,
	.glitch_ignore_cnt = 7,
	.flags.enable_internal_pullup = true,
	};
	
	ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
		
	i2c_device_config_t dev_rtc = {
	.dev_addr_length = I2C_ADDR_BIT_LEN_7,
	.device_address = dev_addr,
	.scl_speed_hz = 100000,
	};
		
	ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_rtc, &rtc_handle));
		
	}
//===================================================================================================================
void I2C_write(const uint8_t *p, const uint8_t write_size)
{
	ESP_ERROR_CHECK(i2c_master_transmit(rtc_handle, p, write_size, -1));
	vTaskDelay(1 / portTICK_PERIOD_MS);

}
//===================================================================================================================
void I2C_read(uint8_t *p, uint8_t read_size)
{
	ESP_ERROR_CHECK(i2c_master_receive(rtc_handle, p, read_size, -1));
	vTaskDelay(1 / portTICK_PERIOD_MS);

}
//===================================================================================================================
void I2C_write_read(const uint8_t *write, const uint8_t size_write, uint8_t *read, const uint8_t size_read)
{
	ESP_ERROR_CHECK(i2c_master_transmit_receive(rtc_handle, write, size_write, read, size_read, -1));
	vTaskDelay(1 / portTICK_PERIOD_MS);

}
