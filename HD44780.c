//-------------------------------------------------------------------------------------------------
// Wy�wietlacz alfanumeryczny ze sterownikiem HD44780
// Sterowanie w trybie 4-bitowym bez odczytu flagi zaj�to�ci
// z dowolnym przypisaniem sygna��w steruj�cych
// Plik : HD44780.c	
// Mikrokontroler : Atmel AVR
// Kompilator : avr-gcc
// Autor : Rados�aw Kwiecie�
// �r�d�o : http://radzio.dxp.pl/hd44780/
// Data : 24.03.2007
// Modyfikacja i przeniesienie na STM32 : Bart�omiej B�eszy�ski
// Data modyfikacji 25.02.2017
//-------------------------------------------------------------------------------------------------

#include "HD44780.h"
//-------------------------------------------------------------------------------------------------
//
// Funkcja wystawiaj�ca p�bajt na magistral� danych
//
//-------------------------------------------------------------------------------------------------

void _delay_ms(unsigned int in)
{
	TIM2->PSC = 72000;
	TIM2->ARR = in*16;
	TIM2->CR1 |= (TIM_CR1_DIR)|(TIM_CR1_OPM);

	TIM2->CR1 |= (TIM_CR1_CEN);
	while(!(TIM2->SR & TIM_SR_UIF));
	TIM2->SR &= ~TIM_SR_UIF;

}


void _LCD_OutNibble(unsigned char nibbleToWrite)
{
if(nibbleToWrite & 0x01)
	LCD_DB4_PORT |= LCD_DB4;
else
	LCD_DB4_PORT  &= ~LCD_DB4;

if(nibbleToWrite & 0x02)
	LCD_DB5_PORT |= LCD_DB5;
else
	LCD_DB5_PORT  &= ~LCD_DB5;

if(nibbleToWrite & 0x04)
	LCD_DB6_PORT |= LCD_DB6;
else
	LCD_DB6_PORT  &= ~LCD_DB6;

if(nibbleToWrite & 0x08)
	LCD_DB7_PORT |= LCD_DB7;
else
	LCD_DB7_PORT  &= ~LCD_DB7;
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu bajtu do wy�wietacza (bez rozr�nienia instrukcja/dane).
//
//-------------------------------------------------------------------------------------------------
void _LCD_Write(unsigned char dataToWrite)
{
LCD_E_PORT |= LCD_E;
_LCD_OutNibble(dataToWrite >> 4);
LCD_E_PORT &= ~LCD_E;
LCD_E_PORT |= LCD_E;
_LCD_OutNibble(dataToWrite);
LCD_E_PORT &= ~LCD_E;
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu rozkazu do wy�wietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteCommand(unsigned char commandToWrite)
{
LCD_RS_PORT &= ~LCD_RS;
_delay_ms(2);
_LCD_Write(commandToWrite);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu danych do pami�ci wy�wietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteData(unsigned char dataToWrite)
{
LCD_RS_PORT |= LCD_RS;
_delay_ms(2);
_LCD_Write(dataToWrite);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja wy�wietlenia napisu na wyswietlaczu.
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteText(char * text)
{
while(*text)
  LCD_WriteData(*text++);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja ustawienia wsp�rz�dnych ekranowych
//
//-------------------------------------------------------------------------------------------------
void LCD_GoTo(unsigned char x, unsigned char y)
{
LCD_WriteCommand(HD44780_DDRAM_SET | (x + (0x40 * y)));
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja czyszczenia ekranu wy�wietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Clear(void)
{
LCD_WriteCommand(HD44780_CLEAR);
_delay_ms(2);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja przywr�cenia pocz�tkowych wsp�rz�dnych wy�wietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Home(void)
{
LCD_WriteCommand(HD44780_HOME);
_delay_ms(2);
}
//-------------------------------------------------------------------------------------------------
//
// Procedura inicjalizacji kontrolera HD44780.
//
//-------------------------------------------------------------------------------------------------
void LCD_Initalize(void)
{
unsigned char i;

_delay_ms(1000); // oczekiwanie na ustalibizowanie si� napiecia zasilajacego
LCD_RS_PORT &= ~LCD_RS; // wyzerowanie linii RS
LCD_E_PORT &= ~LCD_E;  // wyzerowanie linii E

for(i = 0; i < 3; i++) // trzykrotne powt�rzenie bloku instrukcji
  {
  LCD_E_PORT |= LCD_E; //  E = 1
  _LCD_OutNibble(0x03); // tryb 8-bitowy
  LCD_E_PORT &= ~LCD_E; // E = 0
  _delay_ms(5); // czekaj 5ms
  }

LCD_E_PORT |= LCD_E; // E = 1
_delay_ms(2);
_LCD_OutNibble(0x02); // tryb 4-bitowy
_delay_ms(2);
LCD_E_PORT &= ~LCD_E; // E = 0

_delay_ms(2); // czekaj 1ms
LCD_WriteCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT); // interfejs 4-bity, 2-linie, znak 5x7
LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF); // wy��czenie wyswietlacza
LCD_WriteCommand(HD44780_CLEAR); // czyszczenie zawartos�i pamieci DDRAM
_delay_ms(2);
LCD_WriteCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT);// inkrementaja adresu i przesuwanie kursora
LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK); // w��cz LCD, bez kursora i mrugania
}

//-------------------------------------------------------------------------------------------------
//
// Koniec pliku HD44780.c
//
//-------------------------------------------------------------------------------------------------
