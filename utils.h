#include "stm32f10x.h"
#include "HD44780.h"
#include "stdlib.h"
#include "stm32f10x_gpio.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"
#include  "comm.h"

#define  RECIEVED_DATA_LENGHT 9 //d³ugoæ komendy z komputera

void operation(bool *degrees_data_to_read, bool *radian_data_to_read, bool *single_measure_no_send, bool *single_measure_with_send);
void config_pheripherials(uint8_t  *recieved_data);
void display_static_menu();

