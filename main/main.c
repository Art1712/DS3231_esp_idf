#include "HEADER.h"
#include "I2C_sourse.c"
#include "DS3231.c"


void time(void *pvParameters)
{
	// Subscribe this task to TWDT, then check if it is subscribed
	ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
	ESP_ERROR_CHECK(esp_task_wdt_status(NULL));
	ESP_LOGI("time", "time started");

	uint8_t H = 0x00;
	uint8_t M = 0x00;
	uint8_t S = 0x00;
	
	uint8_t Da = 0x00;
	uint8_t Dt = 0x00;
	uint8_t MON = 0x00;
	uint8_t YEA = 0x00;

	char week_day[11];
	char month[11];
	
	uint8_t sec_prev = 0, Da_prev = 0, MON_prev = 0;
	
	for (;;)
	{
		DS3231_get_time(&H, &M, &S);
		DS3231_get_data(&Da, &Dt, &MON, &YEA);
		
		if (Da_prev != Da)
		{
			// Преобразуем день недели в текстовое название
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
			Da_prev = Da;
		}
		
		if (MON_prev != MON)
		{
			// Преобразуем месяц в текстовое название
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
			MON_prev = MON;
		}
		
		if (sec_prev != S)
		{
			sec_prev = S;
			// Выводим полученные данные в UART порт
			printf("%02u:%02u:%02u\n", H, M, S);
			printf("%s %s %u\n", week_day, month, 2000 + YEA);
			printf("===================================\n");
		}
		
		esp_task_wdt_reset();
	}
	
	// Сюда мы не должны добраться никогда. Но если "что-то пошло не так" - нужно всё-таки удалить задачу из памяти
	ESP_LOGI("time", "time error");
	// Удаляем задачу для WDT
	ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));
	vTaskDelete(NULL);
}

void app_main(void)
{
	I2C_init(0x68); // Инициируем RTC
	DS3231_init();
	
	// Установка времени
	//DS3231_set_time(10, 10, 10);
	//DS3231_set_data(6, 27, 9, 25);
	
	// Настройка будильников
	//DS3231_set_alarm1(0, 0, -1, -1, -1);
	//DS3231_set_alarm2(-1, 50, 12, -1, 2);
	
	// Создаём задачу
	xTaskCreate(time, "time", 2048, NULL, 0, NULL);

}
