/**
 * @file funkcja g³ówna programu inklinometru
 * @author Bart³omiej B³eszyñski
 * @brief Elektroniczna poziomica - funkcja g³ówna programu
 */



#include "stm32f10x.h"
#include "HD44780.h"
#include "stdlib.h"
#include "stm32f10x_gpio.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"
#include  "comm.h"
#include  "utils.h"
#include "definitions.h"


uint8_t recieved_data[RECIEVED_DATA_LENGHT]; //bufor odebranych danych

bool degrees_data_to_read=false; // flaga wywo³ania odczytu z przesy³em UART dla stopni (po odebraniu zapytania przez uart GET::DEG)
bool radian_data_to_read=false; // flaga wywo³ania odczytu z przesy³em UART dla radianów (po odebraniu zapytania przez uart GET::RAD)
bool single_measure_no_send=false; // flaga wywo³ania odczytu po naciscnieciu przycisku bez przes³ania UART dla stopni i radianów jednoczenie
bool single_measure_with_send=false; // flaga wywo³ania odczytu po naciscnieciu przycisku z przesy³em UART dla stopni i radianów jednoczenie

/**
 * @brief funkcja g³ówna
 */
int main(void)
{
config_pheripherials(recieved_data); //konfiguracja peryferiów

LED_R_OFF;
LED_L_OFF;

LCD_Initalize(); //inicjalizacja LCD
Enable_MEMS();; //inicjalizacja mems
display_static_menu(); //wywietla sta³e menu ("stopnie="  "radiany=")

	while(1)
	{
		operation(&degrees_data_to_read,&radian_data_to_read,&single_measure_no_send,&single_measure_with_send); //g³ówna funkcja
	}
}


DMA1_Channel5_IRQHandler()
{
	DMA1_Channel5->CCR &= ~(DMA_CCR1_EN); //wy³¹czenie dma
	DMA1_Channel5->CNDTR = RECIEVED_DATA_LENGHT; //ponowne zapisanie d³ugosci
	DMA1->IFCR |= (DMA_IFCR_CGIF5); // reset przerwania dma
	DMA1_Channel5->CMAR = recieved_data; //wskaŸnik na pocz¹tek tablicy

	char tab[15]; //bufor tymczasowy

	strcpy(tab,"GET::RAD\r");
	if(strcmp(tab,recieved_data)==0) //porownywanie ci¹gów otrzymanych przez UART
	{
		LED_L_TGGL;  //zmiana stanu diody
		radian_data_to_read = true;
	}
	strcpy(tab,"GET::DEG\r");
	if(strcmp(tab,recieved_data)==0)
	{
		LED_R_TGGL;  //zmiana stanu diody
		degrees_data_to_read = true;
	}
	DMA1_Channel5->CCR |= (DMA_CCR1_EN); //ponowne uruchomienie dma (oczekuje na kolejn¹ komendê)
}


EXTI15_10_IRQHandler()
{
	TIM3->CR1 |= (TIM_CR1_CEN); //czêsc debouncingu (start timera TIM3)
	EXTI->PR = (EXTI_PR_PR11)|(EXTI_PR_PR12); //usuwanie flag od przerwania zewnêtrzenego
}

//przerwanie debouncingu (po up³yniêci czasu timera TIM3 sprawdane s¹ stany na SW1 i SW2)
TIM3_IRQHandler()
{
	TIM3->SR &= ~(TIM_SR_UIF); // czyszenie flagi "update interupt"

	if(SW_1_ON){
		LED_P_TGGL;
		single_measure_no_send = true; //ustawienie flagi : naciniêty przycisk -> pomiar bez wys³ania
	}
	if(SW_2_ON)
	{
		LED_L_TGGL;
		single_measure_with_send =true; //ustawienie flagi : naciniêty przycisk -> pomiar z wys³aniem
	}
}

