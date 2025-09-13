#include "HEADER.h"
#include "I2C_sourse.c"
#include "DS3231.c"
#include "BME280.c"
#include "AHT10.c"
#include "SD_Card.c"
#include "driver/uart.h"
#include "Light_Sleep.c"

DAT current_data;
/*
// Переменная очереди
QueueHandle_t interputQueue;

// Обработчик прерывания
static void IRAM_ATTR isrButtonPress(void* arg)
{
	fl = !fl;
	//int pinNumber = (int)arg;
	// Передаём в очередь значение флага fl
	xQueueSendFromISR(interputQueue, &fl, NULL);
}
*/
void ttime(void *pvParameters)
{
	ESP_LOGI("ttime", "ttime started");

	float temp = 0.0, hum = 0.0;
	uint8_t H = 0x00;
	uint8_t M = 0x00;
	uint8_t S = 0x00;
	
	uint8_t Da = 0x00;
	uint8_t Dt = 0x00;
	uint8_t MON = 0x00;
	uint8_t YEA = 0x00;

	char week_day[11];
	char month[11];
	
	char buf[100];
	
	AHT10_init();
	uint8_t st[2] = {STATUS, 0b10001000};
	
	for (;;)
	{
	//if (xQueueReceive(interputQueue, &flag, portMAX_DELAY) == pdPASS)
	//{
		// Ожидаем пока все передачи по UART завершатся
		uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
		//vTaskDelay(90);
		// Переходим в режим LIGHT SLEEP
		esp_light_sleep_start();
		//esp_deep_sleep_start();
		
		// Сбрасываем флаги прерываний RTC после пробуждения 
		I2C_write(1, st, 2);
		printf("===================================\n");
		// Сбрасываем флаги прерываний
		AHT10_get_data(&temp, &hum);
		DS3231_get_time(&H, &M, &S);
		DS3231_get_data(&Da, &Dt, &MON, &YEA);
		/*
		{// Determine wake up reason
		const char* wakeup_reason;
			switch (esp_sleep_get_wakeup_cause()) {
			case ESP_SLEEP_WAKEUP_TIMER:
				wakeup_reason = "timer";
			break;
			case ESP_SLEEP_WAKEUP_GPIO:
				wakeup_reason = "pin";
			break;
			case ESP_SLEEP_WAKEUP_UART:
				wakeup_reason = "uart";
				// Hang-up for a while to switch and execute the uart task
				// Otherwise the chip may fall sleep again before running uart task
				vTaskDelay(1);
			break;
			default:
				wakeup_reason = "other";
			break;
			}
		printf("%s\n", wakeup_reason);
		}
		*/
			switch (Da)
			{
				case 1: strcpy(week_day, "Monday\n");
				break;
				case 2: strcpy(week_day, "Tuesday\n");
				break;
				case 3: strcpy(week_day, "Wednesday\n");
				break;
				case 4: strcpy(week_day, "Thirsday\n");
				break;
				case 5: strcpy(week_day, "Friday\n");
				break;
				case 6: strcpy(week_day, "Saturday\n");
				break;
				case 7: strcpy(week_day, "Sunday\n");
				break;
			}
			
			switch (MON)
			{
				case 1: strcpy(month, "January\n");
				break;
				case 2: strcpy(month, "February\n");
				break;
				case 3: strcpy(month, "March\n");
				break;
				case 4: strcpy(month, "Aprel\n");
				break;
				case 5: strcpy(month, "May\n");
				break;
				case 6: strcpy(month, "June\n");
				break;
				case 7: strcpy(month, "July\n");
				break;
				case 8: strcpy(month, "August\n");
				break;
				case 9: strcpy(month, "September\n");
				break;
				case 10: strcpy(month, "October\n");
				break;
				case 11: strcpy(month, "November\n");
				break;
				case 12: strcpy(month, "December\n");
				break;
			}
		printf("%02u:%02u:%02u\n", H, M, S);
		printf("%u %s %u\n", Dt, month, 2000 + YEA);
		printf("Temp = %.0f deg C\tHum = %.0f%%\n", temp, hum);
		
		sprintf(buf, "%02u:%02u:%02u %u.%u.%u\nTemp = %.0f deg C\tHum = %.0f%%\n", H, M, S, Dt, MON, YEA, temp, hum);
		SD_write_file_txt(buf);
		
		current_data.HOUR = H;
		current_data.MINUTE = M;
		current_data.SECOND = S;
		current_data.DATA = Dt;
		current_data.MONTH = MON;
		current_data.YEAR = YEA;
		current_data.W_D = week_day;
		current_data.M_TH = month;
		current_data.temperature = temp;
		current_data.humidity = hum;
		SD_write_file_dat(current_data);

		//esp_task_wdt_reset();
	}
	ESP_LOGI("temp_measurement", "temp_measurement error");
	// Демонтируем флешку при ошибке задачи
	sdcard_unmount();
	//ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));
	vTaskDelete(NULL);
}

void app_main(void)
{
	I2C_init(0x68, 0x38); // Инициируем RTC
	DS3231_init();
	sdspi_init();
	Light_Sleep_register_gpio_wakeup(27, 0);
	//deep_sleep_register_ext0_wakeup();
	//DS3231_alarm1_deinit();
	//DS3231_alarm2_deinit();
	DS3231_set_alarm1(0, 0, -1, -1, -1);
	//DS3231_set_alarm2(-1, 50, 12, -1, 2);
	
	/*
	// Создаём очередь
	interputQueue = xQueueCreate(2, sizeof(uint8_t));
	if (interputQueue == NULL) {
	  printf("Error creating message queue");
	};
	*/
		
	xTaskCreate(ttime, "ttimes", 2048, NULL, 0, NULL);

}
