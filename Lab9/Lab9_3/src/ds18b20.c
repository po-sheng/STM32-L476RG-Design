#include "ds18b20.h"

/* Send ConvT through OneWire with resolution
 * param:
 *   OneWire: send through this
 *   resolution: temperature resolution
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
int DS18B20_ConvT(OneWire_t* OneWire, DS18B20_Resolution_t resolution) {
	//DS18B20_SetResolution(OneWire, resolution);
	OneWire_Init(OneWire, OneWire->GPIOx, OneWire->GPIO_Pin);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, 0x44);
	return 0;
}

/* Read temperature from OneWire
 * param:
 *   OneWire: send through this
 *   destination: output temperature
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
uint8_t DS18B20_Read(OneWire_t* OneWire, int *destination) {
	OneWire_Init(OneWire, OneWire->GPIOx, OneWire->GPIO_Pin);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, 0xBE);
	uint8_t low = OneWire_ReadByte(OneWire);
	uint8_t high = OneWire_ReadByte(OneWire);
	OneWire_Init(OneWire, OneWire->GPIOx, OneWire->GPIO_Pin);
	unsigned int ans = high;
	ans = ans << 8;
	ans |= low;
	if(ans == 0)
		*destination = 0;
	else
		*destination = ans;
	return 0;
}

/* Set resolution of the DS18B20
 * param:
 *   OneWire: send through this
 *   resolution: set to this resolution
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
uint8_t DS18B20_SetResolution(OneWire_t* OneWire, DS18B20_Resolution_t resolution) {
	OneWire_Init(OneWire, OneWire->GPIOx, OneWire->GPIO_Pin);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, 0x4E);
	OneWire_WriteByte(OneWire, 125);
	OneWire_WriteByte(OneWire, -55);
	switch(resolution)	{
		case TM_DS18B20_Resolution_9bits:
			OneWire_WriteByte(OneWire, 0x1F);
			break;
		case TM_DS18B20_Resolution_10bits:
			OneWire_WriteByte(OneWire, 0x3F);
			break;
		case TM_DS18B20_Resolution_11bits:
			OneWire_WriteByte(OneWire, 0x5F);
			break;
		case TM_DS18B20_Resolution_12bits:
			OneWire_WriteByte(OneWire, 0x7F);
			break;
		default:
			return 1;
	}
	OneWire_Init(OneWire, OneWire->GPIOx, OneWire->GPIO_Pin);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, 0x48);
	delay_us(15000);
	return 0;
}

/* Check if the temperature conversion is done or not
 * param:
 *   OneWire: send through this
 * retval:
 *    0 -> OK
 *    1 -> Not yet
 */
uint8_t DS18B20_Done(OneWire_t* OneWire) {
	OneWire_Init(OneWire, OneWire->GPIOx, OneWire->GPIO_Pin);
	OneWire_SkipROM(OneWire);
	uint8_t done = OneWire_ReadBit(OneWire);
	if(done == 1)
		return 0;
	else
		return 1;
}
