

#define SW_1_ON ((GPIOA->IDR & (GPIO_IDR_IDR11) )==0 )
#define SW_2_ON ((GPIOA->IDR & (GPIO_IDR_IDR12) )==0 )
#define LED_L_ON GPIOB->ODR &= ~(GPIO_ODR_ODR15)
#define LED_R_ON GPIOA->ODR &= ~(GPIO_ODR_ODR8)
#define LED_L_OFF GPIOB->ODR |= (GPIO_ODR_ODR15)
#define LED_R_OFF GPIOA->ODR |= (GPIO_ODR_ODR8)
#define LED_L_TGGL GPIOB->ODR ^= (GPIO_ODR_ODR15)
#define LED_R_TGGL GPIOA->ODR ^= (GPIO_ODR_ODR8)
