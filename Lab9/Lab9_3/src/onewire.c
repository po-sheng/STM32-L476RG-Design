#include "onewire.h"

/* Init OneWire Struct with GPIO information
 * param:
 *   OneWire: struct to be initialized
 *   GPIOx: Base of the GPIO DQ used, e.g. GPIOA
 *   GPIO_Pin: The pin GPIO DQ used, e.g. 5
 */
void OneWire_Init(OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin) {
	int count = 0;
	OneWire_High(OneWireStruct);
	delay_us(2);
	OneWire_Low(OneWireStruct);
	delay_us(500);
	OneWire_High(OneWireStruct);
	OneWire_Input(OneWireStruct);
	delay_us(70);
	while(OneWire_ReadBit(OneWireStruct) == 1)	{
		delay_us(2);
		count++;
		if(count > 1000)
			break;
	}
	OneWire_Output(OneWireStruct);
	delay_us(430);
}

/* Send reset through OneWireStruct
 * Please implement the reset protocol
 * param:
 *   OneWireStruct: wire to send
 * retval:
 *    0 -> Reset OK
 *    1 -> Reset Failed
 */
uint8_t OneWire_Reset(OneWire_t* OneWireStruct) {
return 0;
}

/* Write 1 bit through OneWireStruct
 * Please implement the send 1-bit protocol
 * param:
 *   OneWireStruct: wire to send
 *   bit: bit to send
 */
void OneWire_WriteBit(OneWire_t* OneWireStruct, uint8_t bit) {
	OneWire_High(OneWireStruct);
	delay_us(2);
	if(bit == 1)	{
		OneWire_Low(OneWireStruct);
		delay_us(2);
		OneWire_High(OneWireStruct);
		delay_us(62);
	}
	else	{
		OneWire_Low(OneWireStruct);
		delay_us(2);
		delay_us(62);
		OneWire_High(OneWireStruct);
		delay_us(2);
	}
}

/* Read 1 bit through OneWireStruct
 * Please implement the read 1-bit protocol
 * param:
 *   OneWireStruct: wire to read from
 */
uint8_t OneWire_ReadBit(OneWire_t* OneWireStruct) {
	uint8_t Read_Data_Bit;
	OneWire_High(OneWireStruct);
	delay_us(2);
	OneWire_Low(OneWireStruct);
	delay_us(2);
	OneWire_High(OneWireStruct);
	OneWire_Input(OneWireStruct);
	delay_us(4);
	if(((OneWireStruct->GPIOx->IDR) & (1<<(OneWireStruct->GPIO_Pin))) != 0)
		Read_Data_Bit = 1;
	else
		Read_Data_Bit = 0;
	OneWire_Output(OneWireStruct);
	//OneWire_High(OneWireStruct);
	delay_us(62);
return Read_Data_Bit;
}

/* A convenient API to write 1 byte through OneWireStruct
 * Please use OneWire_WriteBit to implement
 * param:
 *   OneWireStruct: wire to send
 *   byte: byte to send
 */
void OneWire_WriteByte(OneWire_t* OneWireStruct, uint8_t byte) {
	for(int i=0; i<=7; i++)	{
		if((byte & (1<<i)) != 0)	{
			OneWire_WriteBit(OneWireStruct, 1);
		}
		else	{
			OneWire_WriteBit(OneWireStruct, 0);
		}
		delay_us(2);
	}
}

/* A convenient API to read 1 byte through OneWireStruct
 * Please use OneWire_ReadBit to implement
 * param:
 *   OneWireStruct: wire to read from
 */
uint8_t OneWire_ReadByte(OneWire_t* OneWireStruct) {
	uint8_t Read_Data_Byte = 0;
	for(int i=0; i<8; i++)	{
		Read_Data_Byte |= OneWire_ReadBit(OneWireStruct) << i;
		delay_us(2);
	}
return Read_Data_Byte;
}

/* Send ROM Command, Skip ROM, through OneWireStruct
 * You can use OneWire_WriteByte to implement
 */
void OneWire_SkipROM(OneWire_t* OneWireStruct) {
	OneWire_WriteByte(OneWireStruct, 0xCC);
}

void OneWire_High(OneWire_t* OneWireStruct)	{
	SET_REG(OneWireStruct->GPIOx->ODR, 1<<OneWireStruct->GPIO_Pin, 1<<OneWireStruct->GPIO_Pin);
}

void OneWire_Low(OneWire_t* OneWireStruct)	{
	SET_REG(OneWireStruct->GPIOx->ODR, 1<<OneWireStruct->GPIO_Pin, 0);
}

void OneWire_Input(OneWire_t* OneWireStruct)	{
	SET_REG(OneWireStruct->GPIOx->MODER, 3<<(OneWireStruct->GPIO_Pin * 2), 0);
}

void OneWire_Output(OneWire_t* OneWireStruct)	{
	SET_REG(OneWireStruct->GPIOx->MODER, 3<<(OneWireStruct->GPIO_Pin * 2), 1<<(OneWireStruct->GPIO_Pin * 2));
	SET_REG(OneWireStruct->GPIOx->OSPEEDR, 3<<(OneWireStruct->GPIO_Pin * 2), 1<<(OneWireStruct->GPIO_Pin * 2));
	SET_REG(OneWireStruct->GPIOx->PUPDR, 3<<(OneWireStruct->GPIO_Pin * 2), 1<<(OneWireStruct->GPIO_Pin * 2));
}
