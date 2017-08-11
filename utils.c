
/**
 * @file funkcje g³ówne - obs³uga zdarzeñ (odbiór naciniêcie)
 * @author bart³omiej B³eszyñski
 * @brief konfiguracja i obs³uga zdarzeñ
 *
 */
#include "utils.h"




void operation(bool *degrees_data_to_read, bool *radian_data_to_read, bool *single_measure_no_send, bool *single_measure_with_send)

{
	int val = 0;
	/**
	 * pobieranie próbek z akcelerometru i ich urednianie w celu prostej filtracji dolnoprzepustowej
	 */
	for(int i=0;i<100;i++)
	{
		val = val + Get_Data_MEMS(ACCEL_YOUT_H, ACCEL_YOUT_L); // wartoæ z rejestrów przyspieszenia osi Y
	}

	float conv;
	conv = (float)val;
	conv = conv/100;
	float angle_deg,angle_rad;
	angle_deg = (conv/16384)*90; // konwersja na stopnie
	angle_rad = (conv/16384)*0.5*3.14; //konwersja na radiany

	char tex_a[50],tex_b[50];
	unsigned int number_degrees,number_radians;
	number_degrees = sprintf (tex_a, "%.2f",angle_deg); //formatowanie na postaæ textu
	number_radians = sprintf (tex_b, "%.2f",angle_rad); //formatowanie na postaæ textu

	if(*single_measure_no_send || *single_measure_with_send || *degrees_data_to_read || *radian_data_to_read){
		LCD_GoTo(8,0);
		LCD_WriteText("       "); // szybsza wersja czyszczenia ekranu
		LCD_GoTo(8,0);
		LCD_WriteText(tex_a);
		LCD_WriteData(0b11011111); //znak stopni
		LCD_GoTo(8,1);
		LCD_WriteText("       "); // szybsza wersja czyszczenia ekranu
		LCD_GoTo(8,1);
		LCD_WriteText(tex_b);
		*single_measure_no_send = false;
	}

	//tresc do wys³ania scalona z wynikiem w stopniach
	const char t_1[] = " stopni";
	strcat(tex_a,t_1);
	strcat(tex_a,"\r");

	//tresc do wys³ania scalona z wynikiem w radianach
	const char t_2[] = " radianow";
	strcat(tex_b,t_2);
	strcat(tex_b,"\r");

	/*
	 * obs³uga zdarzenia : naciniêty przycisk pomiaru z wysy³k¹ lub rz¹danie wys³ania stopni
	 */
	if(*degrees_data_to_read || *single_measure_with_send)
	{
		DMA1_Channel4->CCR &= ~(DMA_CCR1_EN);
		DMA1_Channel4->CNDTR = number_degrees + (int)strlen(t_1)+1;
		DMA1->IFCR |= DMA_IFCR_CGIF4;
		USART1->SR &= ~(USART_SR_TC);
		DMA1_Channel4->CMAR = tex_a;
		DMA1_Channel4->CCR |= (DMA_CCR1_EN);
		while (!(DMA1->ISR & DMA_ISR_TCIF4));
		*degrees_data_to_read=false;
	}

	/*
	 * obs³uga zdarzenia : naciniêty przycisk pomiaru z wysy³k¹ lub rz¹danie wys³ania radianów
	 */
	if(*radian_data_to_read || *single_measure_with_send)
	{
		DMA1_Channel4->CCR &= ~(DMA_CCR1_EN);
		DMA1_Channel4->CNDTR = number_radians + (int)strlen(t_2)+1;
		DMA1->IFCR |= DMA_IFCR_CGIF4;
		USART1->SR &= ~(USART_SR_TC);
		DMA1_Channel4->CMAR = tex_b;
		DMA1_Channel4->CCR |= (DMA_CCR1_EN);
		while (!(DMA1->ISR & DMA_ISR_TCIF4));
		*radian_data_to_read = false;
	}

	/*
	 * obs³uga zdarzenia : naciniêty przycisk pomiaru bez wysy³ki
	 */
	if(*single_measure_with_send)
	{
		*single_measure_with_send=false;
	}
}

/**
 * @brief inicjalizacja uk³adów peryferyjnych mikrokontrolera
 */
void config_pheripherials(uint8_t  *recieved_data)
{
///uruchomienie DMA
				RCC->AHBENR |=
					(RCC_AHBENR_DMA1EN);

///uruchomienie liczników
				RCC->APB1ENR |=
					(RCC_APB1ENR_TIM2EN)|(RCC_APB1ENR_TIM3EN)|
					(RCC_APB1ENR_TIM4EN)|(RCC_APB1ENR_I2C1EN);

///uruchomienie GPIO,UART,Timer,rejetru AF
				RCC->APB2ENR |=
					(RCC_APB2ENR_IOPAEN)|(RCC_APB2ENR_IOPBEN)|
					(RCC_APB2ENR_IOPCEN)|(RCC_APB2ENR_IOPDEN)|
					(RCC_APB2ENR_IOPEEN)|
					(RCC_APB2ENR_TIM1EN)|(RCC_APB2ENR_AFIOEN)|
					(RCC_APB2ENR_USART1EN);


				GPIO_InitTypeDef def;
				//UART RX
				def.GPIO_Mode = GPIO_Mode_IN_FLOATING;
				def.GPIO_Speed = GPIO_Speed_50MHz;
				def.GPIO_Pin = GPIO_Pin_10;
				GPIO_Init(GPIOA,&def);

				//UART TX
				def.GPIO_Mode = GPIO_Mode_AF_PP;
				def.GPIO_Speed = GPIO_Speed_50MHz;
				def.GPIO_Pin = GPIO_Pin_9;
				GPIO_Init(GPIOA,&def);

				//LCD (15-LCD) (8 - LED)
				def.GPIO_Mode = GPIO_Mode_Out_PP;
				def.GPIO_Speed = GPIO_Speed_50MHz;
				def.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_8;
				GPIO_Init(GPIOA,&def);

				//Przyciski
				def.GPIO_Mode = GPIO_Mode_IPU;
				def.GPIO_Speed = GPIO_Speed_50MHz;
				def.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
				GPIO_Init(GPIOA,&def);

				//port B (15 LED) (reszta lcd)
				def.GPIO_Mode = GPIO_Mode_Out_PP;
				def.GPIO_Speed = GPIO_Speed_50MHz;
				def.GPIO_Pin =GPIO_Pin_15 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
				GPIO_Init(GPIOB,&def);


				//jtag wy³¹czony, SWD pozostawione (bez tego zachodzi konflikt alternatywnej funkcji AF dla UART1)
				AFIO->MAPR |= (AFIO_MAPR_SWJ_CFG_JTAGDISABLE);


				USART1->CR1 |= (USART_CR1_TE)|(USART_CR1_RE); // uruchomiony nadajnik i odbiornik UART
				USART1->BRR = 3750; //baudrate 19200
				USART1->CR3 |= (USART_CR3_DMAR)|(USART_CR3_DMAT);
				USART1->CR1 |= (USART_CR1_UE); // Uruchomione peryferia


				//nadajnik UART  (konfiguracja DMA)
				DMA1_Channel4->CCR |= (DMA_CCR1_MINC)|(DMA_CCR1_PL_0)|(DMA_CCR1_DIR); //sredni priorytet, pamiêæ->UART
				DMA1_Channel4->CPAR = &USART1->DR; // przypisanie wska¿nika

				//odbiornik UART  (konfiguracja DMA)
				DMA1_Channel5->CCR |= (DMA_CCR1_MINC)|(DMA_CCR1_PL_0)|(DMA_CCR1_TCIE); //sredni priorytet, UART->pamiêæ, przerwanie po odbiorze
				NVIC_EnableIRQ(DMA1_Channel5_IRQn); //kontroler przerwania ustawiony aby odbieraæ TCIE
				DMA1_Channel5->CMAR = recieved_data;//wskaŸnik do odebrania danych (bufor odbioru)
				DMA1_Channel5->CNDTR = RECIEVED_DATA_LENGHT; //d³ugoæ danych
				DMA1_Channel5->CPAR = &USART1->DR; //przypisanie wskaŸnika
				DMA1_Channel5->CCR |= (DMA_CCR2_EN); //DMA uruchomione, czeka na pakiet

				//timer do debouncingu przycisków (¿eby nie dawaæ delay w przerwaniach)
				TIM3->PSC = 72000; //prescaler
				TIM3->ARR = 800;//50ms
				TIM3->CR1 |= (TIM_CR1_DIR)|(TIM_CR1_OPM); //ustawiony kierunek w dó³, one-pulse-mode = stop po osi¹gniêcia zero
				TIM3->DIER |= (TIM_DIER_UIE); //przerwanie w przypadku odswierzenia timera
				NVIC_EnableIRQ(TIM3_IRQn); //wlaczone przerwanie od timera


				//sw1 (przerwanie zewnêtrzne)
				AFIO->EXTICR[2] |= (AFIO_EXTICR3_EXTI11_PA);
				EXTI->IMR |= (EXTI_IMR_MR11);
				EXTI->FTSR |= (EXTI_RTSR_TR11);
				EXTI->SWIER |= (EXTI_SWIER_SWIER11);

				//sw2  (przerwanie zewnêtrzne)
				AFIO->EXTICR[3] |= (AFIO_EXTICR4_EXTI12_PA);
				EXTI->IMR |= (EXTI_IMR_MR12);
				EXTI->FTSR |= (EXTI_RTSR_TR12);
				EXTI->SWIER |= (EXTI_SWIER_SWIER12);
				NVIC_EnableIRQ(EXTI15_10_IRQn);

}



/**
 *\brief sta³y tekst wyswietlany na LCD (nieodswierzaniy)
 */

void display_static_menu()
{
	LCD_Clear();
	LCD_GoTo(0,0);
	LCD_WriteText("stopnie=");
	LCD_GoTo(0,1);
	LCD_WriteText("radiany=");
}



