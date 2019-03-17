#include "stm32l476xx.h"
#include "ds18b20.h"
#include <stdio.h>
#include <stdlib.h>

#define SET_REG(REG, SELECT, VAL) {((REG)=((REG)&(~(SELECT))) | (VAL));};

extern int delay_5ms();

int counter = 0, mode = 0;

#define PortSeries GPIOA
#define Port 5
DS18B20_Resolution_t resolution = TM_DS18B20_Resolution_11bits;
OneWire_t *OneWire;

void WriteToLCD(int input, int iscmd)	{
	if(iscmd == 1)	{											// cmd or not
		SET_REG(GPIOB->ODR, GPIO_ODR_ODR_13, 0);
	}
	else	{
		SET_REG(GPIOB->ODR, GPIO_ODR_ODR_13, GPIO_ODR_ODR_13);
	}
	SET_REG(GPIOB->ODR, GPIO_ODR_ODR_14, 0);					// read mode

	SET_REG(GPIOB->ODR, 0xFF, input);							// give 8 bit data

	SET_REG(GPIOB->ODR, GPIO_ODR_ODR_15, GPIO_ODR_ODR_15);		// enable
	delay_5ms();
	SET_REG(GPIOB->ODR, GPIO_ODR_ODR_15, 0);
	delay_5ms();

}

void createfont()	{
	WriteToLCD(0x40, 1);
	WriteToLCD(0x06, 0);
	WriteToLCD(0x41, 1);
	WriteToLCD(0x09, 0);
	WriteToLCD(0x42, 1);
	WriteToLCD(0x11, 0);
	WriteToLCD(0x43, 1);
	WriteToLCD(0x10, 0);
	WriteToLCD(0x44, 1);
	WriteToLCD(0x08, 0);
	WriteToLCD(0x45, 1);
	WriteToLCD(0x04, 0);
	WriteToLCD(0x46, 1);
	WriteToLCD(0x02, 0);
	WriteToLCD(0x47, 1);
	WriteToLCD(0x01, 0);

	WriteToLCD(0x48, 1);
	WriteToLCD(0x0C, 0);
	WriteToLCD(0x49, 1);
	WriteToLCD(0x12, 0);
	WriteToLCD(0x4A, 1);
	WriteToLCD(0x11, 0);
	WriteToLCD(0x4B, 1);
	WriteToLCD(0x01, 0);
	WriteToLCD(0x4C, 1);
	WriteToLCD(0x02, 0);
	WriteToLCD(0x4D, 1);
	WriteToLCD(0x04, 0);
	WriteToLCD(0x4E, 1);
	WriteToLCD(0x08, 0);
	WriteToLCD(0x4F, 1);
	WriteToLCD(0x10, 0);
}

void PrintFloatNum(int *num){
	int integer = *num >> 4, mask = 10, digit, flag = 0;
	int decimal = *num - (integer << 4);
	if((*num & (1 << 15)) == 1)	{
		WriteToLCD(0x2D, 0);
	}
	for(int i = 0; i < 2; i++)	{
		if(integer / mask != 0 || flag == 1)	{
			WriteToLCD(integer/mask + 48, 0);
			integer -= integer/mask * mask;
			flag = 1;
		}
		mask /= 10;
	}
	if(flag == 0)	{
		WriteToLCD(48, 0);
	}
	if(decimal != 0)	{
		WriteToLCD(0x2E, 0);
		while(decimal % 16 != 0)	{
			decimal *= 10;
			digit = decimal >> 4;
			WriteToLCD(digit + 48, 0);
			decimal -= digit << 4;
		}
	}
	WriteToLCD(0xDF, 0);
	WriteToLCD(0x63, 0);
}

int* GetTemperture()	{
	int *ans = malloc(sizeof(int) * 1);
	*ans = 0;
	DS18B20_ConvT(OneWire, resolution);
	//delay_us(800000);
	while(OneWire_ReadBit(OneWire) != 1)	{
		delay_us(2);
	}
	OneWire_Output(OneWire);
	DS18B20_Read(OneWire, ans);
return ans;
}

void EXTI_config()	{
	SET_REG(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN, RCC_APB2ENR_SYSCFGEN);
	SET_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13, SYSCFG_EXTICR4_EXTI13_PC);
	SET_REG(EXTI->RTSR1, EXTI_RTSR1_RT13, EXTI_RTSR1_RT13);
	SET_REG(EXTI->IMR1, EXTI_IMR1_IM13, EXTI_IMR1_IM13);
}

void NVIC_config(){
	// enable IRQ
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, 47);
}

void Temperture_config()	{
	OneWire = malloc(sizeof(OneWire_t)*1);
	OneWire->GPIOx = PortSeries;
	OneWire->GPIO_Pin = Port;
	DS18B20_SetResolution(OneWire, resolution);
}

void EXTI15_10_IRQHandler()	{
	if(mode == 0)	{
		mode = 1;
		SET_REG(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, 0);
		WriteToLCD(0x01, 1);
		WriteToLCD(0x80, 1);
		PrintFloatNum(GetTemperture());
		if(resolution == TM_DS18B20_Resolution_12bits)
			SysTick_Config(5000000UL);
		else
			SysTick_Config(4000000UL);
		SET_REG(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, SysTick_CTRL_ENABLE_Msk);
	}
	else	{
		mode = 0;
		SET_REG(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, 0);
		counter = 0;
		WriteToLCD(0x01, 1);
		WriteToLCD(0x80, 1);
		WriteToLCD(0x00, 0);
		WriteToLCD(0x01, 0);
		SysTick_Config(1300000UL);
		SET_REG(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, SysTick_CTRL_ENABLE_Msk);
	}
	SET_REG(EXTI->PR1, EXTI_PR1_PIF13, EXTI_PR1_PIF13);
}

void SysTick_Handler()	{
	if(mode == 0)	{
		counter = counter + 1;
		if(counter == 16)	{
			counter = 0;
			WriteToLCD(0x01, 1);
			WriteToLCD(0x80, 1);
			WriteToLCD(0x00, 0);
			WriteToLCD(0x01, 0);
		}
		else	{
			WriteToLCD(0x1C, 1);
		}
	}
	else	{
		WriteToLCD(0x01, 1);
		WriteToLCD(0x80, 1);
		PrintFloatNum(GetTemperture());
	}
}

void init()	{
	// SET keypad gpio OUTPUT
	RCC->AHB2ENR = 0x7;

	//Set PA5 as output mode	// 0111 1111 1111
	SET_REG(GPIOA->MODER, 0xC00, 0x400);
	//Set PA5 as medium speed //0100 0000 0000
	SET_REG(GPIOA->OSPEEDR, 0xC00, 0x400);
	//set PA5 is Pull-up	// 0100 0000 0000
	SET_REG(GPIOA->PUPDR, 0xC00, 0x400);

	//Set PB0,1,2,3,4,5,6,7,13,14,15 as output mode	//0101 0111 1111 1111 0101 0101 0101 0101
	SET_REG(GPIOB->MODER, 0xFC00FFFF, 0x54005555);
	//Set PB0,1,2,3,4,5,6,7,13,14,15 as medium speed //0101 0100 0000 0000 0101 0101 0101 0101
	SET_REG(GPIOB->OSPEEDR, 0xFC00FFFF, 0x54005555);
	//set PB0,1,2,3,4,5,6,7,13,14,15 is Pull-down	//1010 1000 0000 0000 1010 1010 1010 1010
	SET_REG(GPIOB->PUPDR, 0xFC00FFFF, 0xA800AAAA);

	//Set PC13 as input mode	//1111 0011 1111 1111 1111 1111 1111 1111
	SET_REG(GPIOC->MODER, 0xC000000, 0);

	WriteToLCD(0x38, 1);		//function setting
	WriteToLCD(0x06, 1); 		//entering mode
	WriteToLCD(0x0C, 1);		//display on
	WriteToLCD(0x01, 1);		//clear screen
	WriteToLCD(0x80, 1);		//move to top left

}

int main()	{
	init();
	NVIC_config();
	EXTI_config();
	createfont();
	Temperture_config();
	WriteToLCD(0x00, 0);
	WriteToLCD(0x01, 0);
	SysTick_Config(1300000UL);
	while(1) {}
return 0;
}
