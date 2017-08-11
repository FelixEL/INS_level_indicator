/**
 * @file funkcja g��wna programu inklinometru
 * @author Bart�omiej B�eszy�ski
 * @brief Elektroniczna poziomica - funkcja g��wna programu
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

bool degrees_data_to_read=false; // flaga wywo�ania odczytu z przesy�em UART dla stopni (po odebraniu zapytania przez uart GET::DEG)
bool radian_data_to_read=false; // flaga wywo�ania odczytu z przesy�em UART dla radian�w (po odebraniu zapytania przez uart GET::RAD)
bool single_measure_no_send=false; // flaga wywo�ania odczytu po naciscnieciu przycisku bez przes�ania UART dla stopni i radian�w jednoczenie
bool single_measure_with_send=false; // flaga wywo�ania odczytu po naciscnieciu przycisku z przesy�em UART dla stopni i radian�w jednoczenie

/**
 * @brief funkcja g��wna
 */
int main(void)
{
config_pheripherials(recieved_data); //konfiguracja peryferi�w

LED_R_OFF;
LED_L_OFF;

LCD_Initalize(); //inicjalizacja LCD
Enable_MEMS();; //inicjalizacja mems
display_static_menu(); //wywietla sta�e menu ("stopnie="  "radiany=")

	while(1)
	{
		operation(&degrees_data_to_read,&radian_data_to_read,&single_measure_no_send,&single_measure_with_send); //g��wna funkcja
	}
}


DMA1_Channel5_IRQHandler()
{
	DMA1_Channel5->CCR &= ~(DMA_CCR1_EN); //wy��czenie dma
	DMA1_Channel5->CNDTR = RECIEVED_DATA_LENGHT; //ponowne zapisanie d�ugosci
	DMA1->IFCR |= (DMA_IFCR_CGIF5); // reset przerwania dma
	DMA1_Channel5->CMAR = recieved_data; //wska�nik na pocz�tek tablicy

	char tab[15]; //bufor tymczasowy

	strcpy(tab,"GET::RAD\r");
	if(strcmp(tab,recieved_data)==0) //porownywanie ci�g�w otrzymanych przez UART
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
	DMA1_Channel5->CCR |= (DMA_CCR1_EN); //ponowne uruchomienie dma (oczekuje na kolejn� komend�)
}


EXTI15_10_IRQHandler()
{
	TIM3->CR1 |= (TIM_CR1_CEN); //cz�sc debouncingu (start timera TIM3)
	EXTI->PR = (EXTI_PR_PR11)|(EXTI_PR_PR12); //usuwanie flag od przerwania zewn�trzenego
}

//przerwanie debouncingu (po up�yni�ci czasu timera TIM3 sprawdane s� stany na SW1 i SW2)
TIM3_IRQHandler()
{
	TIM3->SR &= ~(TIM_SR_UIF); // czyszenie flagi "update interupt"

	if(SW_1_ON){
		LED_P_TGGL;
		single_measure_no_send = true; //ustawienie flagi : nacini�ty przycisk -> pomiar bez wys�ania
	}
	if(SW_2_ON)
	{
		LED_L_TGGL;
		single_measure_with_send =true; //ustawienie flagi : nacini�ty przycisk -> pomiar z wys�aniem
	}
}

