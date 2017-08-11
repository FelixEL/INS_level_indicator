/**
 * @file plik z obs³ug¹ akcelerometru
 * @author Bart³omiej B³eszyñski
 * @brief obs³uga akcelerometru (wysy³anie odbieranie konfiguracja)
 *
 */


#include "comm.h"


void I2C_GPIO_Config() {

	GPIO_InitTypeDef def;
	def.GPIO_Mode = GPIO_Mode_AF_OD;
	def.GPIO_Speed = GPIO_Speed_50MHz;
	def.GPIO_Pin =GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_Init(GPIOB,&def);
	AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP; //remap na scl/pb8 , sda/pb9 GPIO_Pin_10;
	GPIO_Init(GPIOB,&def);

}

void I2C_Config() {
	I2C_InitTypeDef i2c;
	i2c.I2C_Ack = I2C_Ack_Enable;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //adres 7 bit
	i2c.I2C_ClockSpeed = 50000; //czêstotliwoæ fSCL=50kHz
	i2c.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c.I2C_Mode = I2C_Mode_I2C;
	i2c.I2C_OwnAddress1 = 0xAA;
	I2C_Init(MPU6050I2C, &i2c);
	I2C_Cmd(MPU6050I2C, ENABLE);
}


/**
 * @brief odczytywanie z pod adresu
 */
unsigned char ReadMEMS(unsigned char Address) {
	unsigned char Data;
	I2C_GenerateSTART(MPU6050I2C, ENABLE);
	while (!I2C_CheckEvent(MPU6050I2C, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(MPU6050I2C, MPU6050W, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(MPU6050I2C,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(MPU6050I2C, Address);
	while (!I2C_CheckEvent(MPU6050I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(MPU6050I2C, ENABLE);
	while (!I2C_CheckEvent(MPU6050I2C, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(MPU6050I2C, MPU6050R, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(MPU6050I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	I2C_AcknowledgeConfig(MPU6050I2C, DISABLE);
	I2C_GenerateSTOP(MPU6050I2C, ENABLE);
	while (!(I2C_CheckEvent(MPU6050I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)));

	Data = I2C_ReceiveData(MPU6050I2C);
	I2C_AcknowledgeConfig(MPU6050I2C, ENABLE);
	return Data;
}

/**
 * @brief wpisywanie pod adres
 */
void WriteMEMS(unsigned char Address, unsigned char Value) {
	I2C_GenerateSTART(MPU6050I2C, ENABLE);
	while (!I2C_CheckEvent(MPU6050I2C, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(MPU6050I2C, MPU6050W, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(MPU6050I2C,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(MPU6050I2C, Address);
	while (!I2C_CheckEvent(MPU6050I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(MPU6050I2C, Value);
	while (!I2C_CheckEvent(MPU6050I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTOP(MPU6050I2C, ENABLE);
}

/**
 * @brief inicjalizacja MEMS
 */
void Enable_MEMS() {

	I2C_GPIO_Config();
	I2C_Config();
	WriteMEMS(PWR_MGMT_1, 0x00);
}

/**
 * @brief wy³¹czenie uk³adu
 */
void Disable_MEMS() {
	WriteMEMS(PWR_MGMT_2, 0x40);
}

/**
 * @brief deinicjalizacja uk³adu
 */
void Deinit_MEMS() {
	WriteMEMS(PWR_MGMT_2, 0xC0);
}

/**
 * @brief pobieranie temperatury z czyjnika w obudownie
 */
float Get_MEMS_Temp() {
	float temp;
	temp = Get_Data_MEMS(TEMP_OUT_H,TEMP_OUT_L);
	temp = temp / 340 + 36.53;
	return temp;
}

/**
 * @brief pobieranie danej 16 bitowej
 */
short Get_Data_MEMS(unsigned char AddressH, unsigned char AddressL) {
	short DataH;
	char DataL;
	DataH = ReadMEMS(AddressH);
	DataL = ReadMEMS(AddressL);
	return ((DataH << 8) + DataL);
}
