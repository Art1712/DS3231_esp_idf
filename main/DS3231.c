/*
 * DS3231.c
 *
 *  Created on: 24 июл. 2025 г.
 *      Author: root
 */

#include "HEADER.h"


#define INTERR_PIN 27
//#define SQW		12
//#define _32K	9

const uint8_t SECONDS		= 0x00;
const uint8_t NINUTES		= 0x01;
const uint8_t HOURS			= 0x02;
const uint8_t DAY			= 0x03; // WEEK_DAY
const uint8_t DATA			= 0x04; // MONTH_DAY 
const uint8_t MONTH			= 0x05;
const uint8_t YEAR			= 0x06;
// ALARM1---------------------------------------------------------------------------
const uint8_t AL1_SEC		= 0x07;
#define A1M1	7
const uint8_t AL1_MIN		= 0x08;
#define A1M2	7
const uint8_t AL1_HOUR		= 0x09;
#define A1M3	7
const uint8_t AL1_DAY_DATA	= 0x0A;
#define A1M4	7
// ALARM2---------------------------------------------------------------------------
const uint8_t AL2_MIN		= 0x0B;
#define A2M2	7
const uint8_t AL2_HOUR		= 0x0C;
#define A2M3	7
const uint8_t AL2_DAY_DATA	= 0x0D;
#define A2M4	7
// ---------------------------------------------------------------------------------
const uint8_t CONTROL		= 0x0E;
#define EOSC	7
#define BBSQW	6
#define CONV	5
#define RS2		4
#define RS1		3
#define INTCN	2
#define A2IE	1
#define A1IE	0
/*
-------------------------------------------------------------------------------------------------
|	BIT 7	|	BIT 6	|	BIT 5	|	BIT 4	|	BIT 3	|	BIT 2	|	BIT 1	|	BIT 0	|
-------------------------------------------------------------------------------------------------
|	EOSC	|	BBSQW	|	CONV	|	RS2		|	RS1		|	INTCN	|	A2IE	|	A1IE	|
-------------------------------------------------------------------------------------------------
|	0		|	0		|	0		|	1		|	1		|	1		|	0		|	0		|
-------------------------------------------------------------------------------------------------

Bit 7	-	Enable Oscillator (EOSC). When set to logic 0, the oscillator is started. When set to logic 1, the oscillator
(EOSC)		is stopped when the DS3231 switches to VBAT. This bit is clear (logic 0) when power is first applied. When the
			DS3231 is powered by VCC, the oscillator is always on regardless of the status of the EOSC bit. When EOSC is
			disabled, all register data is static.

Bit 6	-	Battery-Backed Square-Wave Enable (BBSQW). 	When set to logic 1 with INTCN = 0 and VCC < VPF, this bit enables 
(BBSQW)		the square wave. When BBSQW is logic 0, the INT/SQW pin goes high impedance when VCC < VPF. This bit is 
			disabled (logic 0) when power is first applied.			
			
Bit 5	-	Convert Temperature (CONV). Setting this bit to 1 forces the temperature sensor to convert the temperature into 
(CONV)		digital code and execute the TCXO algorithm to update the capacitance array to the oscillator. This can only happen 
			when a conversion is not already in progress. The user should check the status bit BSY before forcing the 
			controller to start a new TCXO execution. A user-initiated temperature conversion does not affect the internal 
			64-second update cycle. A user-initiated temperature conversion does not affect the BSY bit for approximately 2ms. 
			The CONV bit remains at a 1 from the time it is written until the conversion is finished, at which time both CONV 
			and BSY go to 0. The CONV bit should be used when monitoring the status of a user-initiated conversion.

RS2, RS1  -	These bits control the frequency of the square-wave output when	the square wave has been enabled. The 
			following table shows the square-wave frequencies that can be selected with the RS bits. These bits 
			are both set to logic 1 (8.192kHz) when power is first applied.the square wave has been enabled. The 
			following table shows the square-wave frequencies that can be selected with the RS bits. These bits are 
			both set to logic 1 (8.192kHz) when power is first applied.

Bits 4 and 3  -	Rate Select (RS2 and RS1). These bits control the frequency of the square-wave output when the square 
(RS2 and RS1)	wave has been enabled. The following table shows the square-wave frequencies that can be selected
				with the RS bits. These bits are both set to logic 1 (8.192kHz) when power is first applied.

-----------------------------------------------------
|	RS2	|	RS1	|	SQUARE-WAVE OUTPUT FREQUENCY	|
-----------------------------------------------------
|	0	|	0	|				1Hz					|
-----------------------------------------------------
|	0	|	1	|				1.024kHz			|
-----------------------------------------------------
|	1	|	0	|				4.096kHz			|
-----------------------------------------------------
|	1	|	1	|				8.192kHz			|
-----------------------------------------------------

Bit 2	-	Interrupt Control (INTCN). This bit controls the INT/SQW signal. When the INTCN bit is set to logic 0,
(INTCN)		a square wave is output on the INT/SQW pin. When the INTCN bit is set to logic 1, then a match between 
			the time-keeping registers and either of the alarm registers activates the INT/SQW output (if the alarm 
			is also enabled). The corresponding alarm flag is always set regardless of the state of the INTCN bit. 
			The INTCN bit is set to logic 1 when power is first applied.

Bit 1	-	Alarm 2 Interrupt Enable (A2IE). When set to logic 1, this bit permits the alarm 2 flag (A2F) bit in the
(A2IE)		status register to assert INT/SQW (when INTCN = 1). When the A2IE bit is set to logic 0 or INTCN is set 
			to logic 0, the A2F bit does not initiate an interrupt signal. The A2IE bit is disabled (logic 0) 
			when power is first applied.

Bit 0	-	Alarm 1 Interrupt Enable (A1IE). When set to logic 1, this bit permits the alarm 1 flag (A1F) bit in the
(A1IE)		status register to assert INT/SQW (when INTCN = 1). When the A1IE bit is set to logic 0 or INTCN is set 
			to logic 0, the A1F bit does not initiate the INT/SQW signal. The A1IE bit is disabled (logic 0) when 
			power is first applied.
*/
// ---------------------------------------------------------------------------------
const uint8_t STATUS		= 0x0F; // CONTROL/STATUS
#define OSF		7
#define EN32kHz	3
#define BSY		2
#define A2F		1
#define A1F		0
/*
-------------------------------------------------------------------------------------------------
|	BIT 7	|	BIT 6	|	BIT 5	|	BIT 4	|	BIT 3	|	BIT 2	|	BIT 1	|	BIT 0	|
-------------------------------------------------------------------------------------------------
|	OSF		|	0		|	0		|	0		|	EN32kHz	|	BSY		|	A2F		|	A1F		|
-------------------------------------------------------------------------------------------------
|	1		|	0		|	0		|	0		|	1		|	X		|	X		|	X		|
-------------------------------------------------------------------------------------------------

Bit 7	-	Oscillator Stop Flag (OSF). A logic 1 in this bit indicates that the oscillator either is stopped or was 
(OSF)		stopped for some period and may be used to judge the validity of the timekeeping data. This bit is set 
			to logic 1 any time that the oscillator stops. The following are examples of conditions that can cause 
			the OSF bit to be set:
			1) The first time power is applied.
			2) The voltages present on both VCC and VBAT are insufficient to support oscillation.
			3) The EOSC bit is turned off in battery-backed mode.
			4) External influences on the crystal (i.e., noise, leakage, etc.).
			This bit remains at logic 1 until written to logic 0.

Bit 3	-	Enable 32kHz Output (EN32kHz). This bit controls the status of the 32kHz pin. When set to logic 1, the
(EN32kHz)	32kHz pin is enabled and outputs a 32.768kHz square-wave signal. When set to logic 0, the 32kHz pin goes 
			to a high-impedance state. The initial power-up state of this bit is logic 1, and a 32.768kHz square-wave 
			signal appears at the 32kHz pin after a power source is applied to the DS3231 (if the oscillator is running).

Bit 2	-	Busy (BSY). This bit indicates the device is busy executing TCXO functions. It goes to logic 1 when the 
(BSY)		conversion signal to the temperature sensor is asserted and then is cleared when the device is in the 
			1-minute idle state.
			
Bit 1	-	Alarm 2 Flag (A2F). A logic 1 in the alarm 2 flag bit indicates that the time matched the alarm 2 registers. 
(A2F)		If the A2IE bit is logic 1 and the INTCN bit is set to logic 1, the INT/SQW pin is also asserted. A2F is 
			cleared when written to logic 0. This bit can only be written to logic 0. Attempting to write to logic 1 
			leaves the value unchanged.

Bit 0	-	Alarm 1 Flag (A1F). A logic 1 in the alarm 1 flag bit indicates that the time matched the alarm 1 registers. 
(A1F)		If the A1IE bit is logic 1 and the INTCN bit is set to logic 1, the INT/SQW pin is also asserted. A1F is 
			cleared when written to logic 0. This bit can only be written to logic 0. Attempting to write to logic 1 
			leaves the value unchanged.
*/
// ---------------------------------------------------------------------------------
const uint8_t ADJUST		= 0x10; // AGING OFFSET
const uint8_t MSB_TEMP		= 0x11;
const uint8_t LSB_TEMP		= 0x12;

// dev_number 1 mean RTC !!!!!!
// dev_number 2 mean BME280 !!!!!!
//===================================================================================================================
void DS3231_init(void)
{
	uint8_t cont;
	//gpio_set_direction(SQW, GPIO_MODE_INPUT);
	//gpio_set_direction(_32K, GPIO_MODE_INPUT);
	
	//gpio_set_pull_mode(SQW, GPIO_FLOATING);
	//gpio_set_pull_mode(_32K, GPIO_FLOATING);
	
	// Читаем состояние регистров CONTROL и STATUS
	gpio_intr_disable(27);
	I2C_write_read(&CONTROL, 1, &cont, 1);
	cont &= ~((1 << EOSC) | (1 << BBSQW) | (1 << INTCN) | (1 << A2IE) | (1 << A1IE));
	// ~(1 << EOSC); // Часы продолжают идти даже когда отключено внешнее питание EOSC в "0"
	//cont |= (1 << BBSQW);
	// На выводе SQW выключаем прямоугольный сигнал
	uint8_t C[2] = {CONTROL, cont};
	I2C_write(C, 2);
}
//===================================================================================================================
void DS3231_set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
	uint8_t ten, one;
	
	ten = hour / 10;
	one = hour % 10;
	uint8_t HOUR[2] = {HOURS, ((ten << 4) | one)};
	ten = minute / 10;
	one = minute % 10;
	uint8_t MIN[2] = {NINUTES, ((ten << 4) | one)};
	ten = second / 10;
	one = second % 10;
	uint8_t SEC[2] = {SECONDS, ((ten << 4) | one)};
	I2C_write(HOUR, 2);
	I2C_write(MIN, 2);
	I2C_write(SEC, 2);
}
//===================================================================================================================
void DS3231_set_data(uint8_t week_day, uint8_t data, uint8_t month, uint8_t year)
{
	uint8_t ten, one;
	/*
	week_day:
		1 - monday
		2 - tuesday
		3 - wednesday
		4 - thirsday
		5 - friday
		6 - saturday
	*/
	/*
		month:
		1 - january
		2 - february
		3 - march
		4 - april
		5 - may
		6 - june
		7 - july
		8 - august
		9 - september
		10 - october
		11 - nothember
		12 - desember
	*/
	/*
	year: from 0 to 255
	*/
	//uint8_t ten = year / 10;
	//uint8_t one = year % 10;
		
	uint8_t W_day[2] = {DAY, week_day};
	ten = data / 10;
	one = data % 10;
	uint8_t Data[2] = {DATA, ((ten << 4) | one)};
	ten = month / 10;
	one = month % 10;
	uint8_t MON[2] = {MONTH, ((ten << 4) | one)};
	//ten = year / 10;
	//one = year % 10;
	uint8_t YER[2] = {YEAR, year};
	I2C_write(W_day, 2);
	I2C_write(Data, 2);
	I2C_write(MON, 2);
	I2C_write(YER, 2);
}
//===================================================================================================================
void DS3231_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
	uint8_t hr, min, sec;
	I2C_write_read(&HOURS, 1, &hr, 1);
	*hour = (((hr & 0b00110000) >> 4) * 10 + (hr & 0b00001111));
	I2C_write_read(&NINUTES, 1, &min, 1);
	*minute = ((min >> 4)*10 + (min & 0b00001111));
	I2C_write_read(&SECONDS, 1, &sec, 1);
	*second = ((sec >> 4)*10 + (sec & 0b00001111));
}
//===================================================================================================================
void DS3231_get_data(uint8_t *day, uint8_t *data, uint8_t *month, uint8_t *year)
{
	uint8_t dat, mon, yr;
	I2C_write_read(&DAY, 1, day, 1);
	
	I2C_write_read(&DATA, 1, &dat, 1);
	*data = ((dat >> 4)*10 + (dat & 0b00001111));
	I2C_write_read(&MONTH, 1, &mon, 1);
	*month = ((mon >> 4)*10 + (mon & 0b00001111));
	I2C_write_read(&YEAR, 1, &yr, 1);
	//*year = ((yr >> 4)*10 + (yr & 0b00001111));
	*year = yr;
}
//===================================================================================================================
uint8_t DS3231_get_year(void)
{
	uint8_t year;
	//uint8_t ten = 0;
	//uint8_t one = 0;
	I2C_write_read(&YEAR, 1, &year, 1);
	//ten = (year >> 4) * 10;
	//one = year & 0b00001111;
	//return (ten + one);
	//return ((year >> 4)*10 + (year & 0b00001111)); // Преобразуем год
	return year;
}
//===================================================================================================================
void DS3231_set_year(uint8_t year)
{
	// Преобразование 
	//uint8_t ten = year / 10;
	//uint8_t one = year % 10;
	//uint8_t year_convert = ((ten << 4) | one);
	uint8_t YER[2] = {YEAR, year};
	I2C_write(YER, 2);
}
//===================================================================================================================
void DS3231_set_alarm1(int8_t sec, int8_t min, int8_t hour, int8_t data, int8_t week_day)
{
	/*
		sec == -1, min == -1, hour == -1, data == -1, week_day == -1 - Звонок каждую секунду
		sec >= 0, min == -1, hour == -1, data == -1, week_day == -1 - Звонок в назначенную секунду
		sec >= 0, min >= 0, hour == -1, data == -1, week_day == -1 - Звонок в назначенную минуту и секунду
		sec >= 01, min >= 0, hour >= 0, data == -1, week_day == -1 - Звонок в назначенную секунду, минуту и час
		sec >= 0, min >= 0, hour >= 0, data >= 0, week_day == -1 - Звонок в назначенную секунду, минуту, час и день месяца
		sec >= 0, min >= 0, hour >= 0, data == -1, week_day >= 0 - Звонок в назначенную секунду минуту, час и день недели
	*/
	// Включаем глобальные прерывания
	gpio_intr_enable(27);
	// Включаем будильник и прерывания на ножке SQW
	//uint8_t data;
	uint8_t ten;
	uint8_t one;
	uint8_t DAT[2] = {CONTROL};
	I2C_write_read(&CONTROL, 1, &DAT[1], 1);
	DAT[1] |= ((1 << INTCN) | (1 << A1IE)); // Включаем прерывания на ножке SQW и будильник 1
	I2C_write(DAT, 2);
	// Сбрасываем возможные прерывыния от будильников 1 и 2
	DAT[0] = STATUS;
	I2C_write_read(&STATUS, 1, &DAT[1], 1);
	DAT[1] &= ~((1 << A1F) | (1 << A2F));
	I2C_write(DAT, 2);
		
	// Звонок каждую секунду
	if ((sec == -1) || (min == -1) || (hour == -1) || (data == -1) || (week_day == -1))
	{
		DAT[0] = AL1_SEC;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
		DAT[0] = AL1_MIN;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
		DAT[0] = AL1_HOUR;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
		DAT[0] = AL1_DAY_DATA;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
	}
	// Звонок в назначенную секунду
	if ((sec >= 0) || (min == -1) || (hour == -1) || (data == -1) || (week_day == -1))
	{
		ten = sec / 10;
		one = sec % 10;
		DAT[0] = AL1_SEC;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		DAT[0] = AL1_MIN;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
		DAT[0] = AL1_HOUR;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
		DAT[0] = AL1_DAY_DATA;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
	}
	// Звонок в назначенную минуту и секунду
	if ((sec >= 0) || (min >= 0) || (hour == -1) || (data == -1) || (week_day == -1))
	{
		ten = sec / 10;
		one = sec % 10;
		DAT[0] = AL1_SEC;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		ten = min / 10;
		one = min % 10;
		DAT[0] = AL1_MIN;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		DAT[0] = AL1_HOUR;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
		DAT[0] = AL1_DAY_DATA;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
	}
	// Звонок в назначенную секунду, минуту и час
	if ((sec >= 0) || (min >= 0) || (hour >= 0) || (data == -1) || (week_day == -1))
	{
		ten = sec / 10;
		one = sec % 10;
		DAT[0] = AL1_SEC;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		ten = min / 10;
		one = min % 10;
		DAT[0] = AL1_MIN;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		ten = hour / 10;
		one = hour % 10;
		DAT[0] = AL1_HOUR;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		DAT[0] = AL1_DAY_DATA;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
	}
	// Звонок в назначенную секунду, минуту, час и день месяца
	if ((sec >= 0) || (min >= 0) || (hour >= 0) || (data >= 0) || (week_day == -1))
	{
		ten = sec / 10;
		one = sec % 10;
		DAT[0] = AL1_SEC;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		ten = min / 10;
		one = min % 10;
		DAT[0] = AL1_MIN;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		ten = hour / 10;
		one = hour % 10;
		DAT[0] = AL1_HOUR;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		ten = data / 10;
		one = data % 10;
		DAT[0] = AL1_DAY_DATA;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
	}
	// Звонок в назначенную секунду минуту, час и день недели
	if ((sec >= 0) || (min >= 0) || (hour >= 0) || (data == -1) || (week_day >= 0))
	{
		ten = sec / 10;
		one = sec % 10;
		DAT[0] = AL1_SEC;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		ten = min / 10;
		one = min % 10;
		DAT[0] = AL1_MIN;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		ten = hour / 10;
		one = hour % 10;
		DAT[0] = AL1_HOUR;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		//ten = data / 10;
		//one = data % 10;
		DAT[0] = AL1_DAY_DATA;
		DAT[1] = (0b01000000 | week_day);
		I2C_write(DAT, 2);
	}	
}
//===================================================================================================================
void DS3231_alarm1_deinit(void)
{
	uint8_t DAT[2] = {CONTROL};
	I2C_write_read(&CONTROL, 1, &DAT[1], 1);
	if (~DAT[1] & (1 << A2F)) // Если при этом 2 будильник уже отключен то отключаем прерывания
	{
		gpio_intr_disable(INTERR_PIN);
	}
	DAT[1] &= ~((1 << INTCN) | (1 << A1IE));
	I2C_write(DAT, 2);
}
//===================================================================================================================
void DS3231_set_alarm2(const int8_t sec, const int8_t min, const int8_t hour, const int8_t data, const int8_t week_day)
{
	/*
	sec == -1, min == -1, hour == -1, data == -1, week_day == -1 - ошибка ввода, отключаем будильник
	sec == 0, min == -1, hour == -1, data == -1, week_day == -1 - Звонок каждую минуту (0 сек)
	sec == -1, min >= 0, hour == -1, data == -1, week_day == -1 - Звонок в назначенную минуту
	sec == -1, min >= 0, hour >= 0, data == -1, week_day == -1 - Звонок в назначенную минуту и час
	sec == -1, min >= 0, hour >= 0, data >= 0, week_day == -1 - Звонок в назначенную минуту, час и день месяца
	sec == -1, min >= 0, hour >= 0, data == -1, week_day >= 0 - Звонок в назначенную минуту, час и день недели
	*/
	// Включаем глобальные прерывания
	gpio_intr_enable(27);
	// Включаем будильник и прерывания на ножке SQW
	//uint8_t data;
	uint8_t ten;
	uint8_t one;
	uint8_t DAT[2];
	I2C_write_read(&CONTROL, 1, &DAT[1], 1);
	DAT[0] = CONTROL;
	DAT[1] |= (1 << INTCN) | (1 << A2IE);	
	I2C_write(DAT, 2);
	// Сбрасываем возможные прерывыния от будильников 1 и 2
	DAT[0] = STATUS;
	I2C_write_read(&STATUS, 1, &DAT[1], 1);
	DAT[1] &= ~((1 << A1F) | (1 << A2F));
	I2C_write(DAT, 2);
	// Ошибка ввода - отключаем будильник
	if ((sec == -1) || (min == -1) || (hour == -1) || (data == -1) || (week_day == -1)) DS3231_alarm2_deinit();
	
	// Звонок каждую минуту (0 сек)
	if ((sec == 0) || (min == -1) || (hour == -1) || (data == -1) || (week_day == -1))
	{
		DAT[0] = AL2_MIN;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
		DAT[0] = AL2_HOUR;
		I2C_write(DAT, 2);
		DAT[0] = AL2_DAY_DATA;
		I2C_write(DAT, 2);
	}
	// Звонок в назначенную минуту
	if ((sec == -1) || (min >= 0) || (hour == -1) || (data == -1) || (week_day == -1))
	{
		ten = min / 10;
		one = min % 10;
		DAT[0] = AL2_MIN;
		DAT[1] = (0b10000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		DAT[0] = AL2_HOUR;
		DAT[1] = 0b10000000;
		I2C_write(DAT, 2);
		DAT[0] = AL2_DAY_DATA;
		DAT[1] = 0b00000000;
		I2C_write(DAT, 2);
	}
	// Звонок в назначенную минуту и час
	if ((sec == -1) || (min >= 0) || (hour >= 0) || (data == -1) || (week_day == -1))
	{
		ten = min / 10;
		one = min % 10;
		DAT[0] = AL2_MIN;
		DAT[1] = (0b10000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		ten = hour / 10;
		one = hour % 10;
		DAT[0] = AL2_HOUR;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		DAT[0] = AL2_DAY_DATA;
		DAT[1] = 0b00000000;
		I2C_write(DAT, 2);
	}
	// Звонок в назначенную минуту, час и день месяца
	if ((sec == -1) || (min >= 0) || (hour >= 0) || (data >= 0) || (week_day == -1))
	{
		ten = min / 10;
		one = min % 10;
		DAT[0] = AL2_MIN;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		ten = hour / 10;
		one = hour % 10;
		DAT[0] = AL2_HOUR;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		ten = data / 10;
		one = data % 10;
		DAT[0] = AL2_DAY_DATA;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
	}
	// Звонок в назначенную минуту, час и день недели
	if ((sec == -1) || (min >= 0) || (hour >= 0) || (data == -1) || (week_day >= 0))
	{
		ten = min / 10;
		one = min % 10;
		DAT[0] = AL2_MIN;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		ten = hour / 10;
		one = hour % 10;
		DAT[0] = AL2_HOUR;
		DAT[1] = (0b00000000 | (ten << 4) | one);
		I2C_write(DAT, 2);
		//ten = data / 10;
		//one = data % 10;
		DAT[0] = AL2_DAY_DATA;
		DAT[1] = (0b01000000 | week_day);
		I2C_write(DAT, 2);
	}	
}
//===================================================================================================================
void DS3231_alarm2_deinit(void)
{
	uint8_t DAT[2] = {CONTROL};
	I2C_write_read(&CONTROL, 1, &DAT[1], 1);	
	if (~DAT[1] & (1 << A1F)) // Если при этом 1 будильник уже отключен то отключаем прерывания
	{
		gpio_intr_disable(INTERR_PIN);
	}
	DAT[1] &= ~((1 << INTCN) | (1 << A2IE));	
	I2C_write(DAT, 2);
}


//===================================================================================================================

