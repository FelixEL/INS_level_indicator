#ifndef __COMM_H
#define __COMM_H


#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"


/*
 * \brief I2C - peryferia
 */
#define MPU6050I2C 		I2C1


/*
 * \brief Adresy do nadawiania i odbioru z czujnika MEMS
 */
#define MPU6050R 0xD1
#define MPU6050W 0xD0


/*
 * \brief Adresy rejestrow czujnika MEMS
 */
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C*/
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C


/*
 *\brief konfiguracja GPIO do I2C dla komunikacji z czujnikiem MEMS
 */
void I2C_GPIO_Config(void);


/*
 * \brief konfiguracja I2C do komunikacji z czujnikiem MEMS
 */
void I2C_Config(void);

/*  \brief  Odczyt danych z czujnika MEMS
 *  \param Address adres rejestru do którego siê odwo³uje
 *  \return				dane zawarte w zaadresowanym rejestrze
 */
unsigned char ReadMEMS(unsigned char Address);

/*
 *\brief  zapis wartoci do zaadresowanego rejestru
 * \param Address 	adres rejestru do kturego zapisywane s¹ dane
 * \param Value		wartoæ zapisywana do rejestru
 */
void WriteMEMS(unsigned char Address, unsigned char Value);

/*
 * \brief Inicjalizacja czujnika MEMS
 */
extern void Enable_MEMS(void);

/*
 * \brief wy³¹czenie czujnika MEMS
 */
extern void Disable_MEMS(void);

/*
 * \brief Deinicjalizacja czujnika MEMS
 */
extern void Deinit_MEMS(void);

/*
 * \brief pomiar temperatury czujnika
 */
extern float Get_MEMS_Temp(void);

/*
 * \brief Pobieranie danych z obu rejestrów,
 * \param AddressH - starszy bajt
 * \param AddressL - m³odszy bajt
 */
extern short Get_Data_MEMS(unsigned char AddressH, unsigned char AddressL);


#endif COMM_H


