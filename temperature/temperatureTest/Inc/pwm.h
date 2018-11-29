#ifndef __pwm_H_
#define __pwm_H_

#include "stm32l1xx_hal.h"
//#include "main.h"




//TIM_OCPOLARITY_HIGH & TIM_OCPOLARITY_LOW
//TIM_CHANNEL_x
//void PWM_SetPolarity(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t OCPolarity);

//void PWM_Peltier_SetHC(uint16_t PeltierN, uint8_t HC);

//void PWM_Init_PeltierArray();

void PWM_Peltier_SetPWM(uint16_t PeltierN, uint8_t HC, uint16_t PWM);

//PeltierArray = PELTIER_1 + PELTIER_2 + PELTIER_3 +....
//PeltierArrayHC_** = bxxxxx
//PeltierArrayHC_now is a global variable that stores the HC information of Peltier coolers
//PeltierArrayPWM is a global array that stores the PWM information of Peltier coolers
//void PWM_Peltier_SetPeltierArray(uint16_t PeltierArray, uint16_t PeltierArrayHC_next, uint16_t * PeltierArrayHC_now, uint16_t * PeltierArrayPWM);
void PWM_Peltier_SetPeltierArray(uint16_t PeltierArray, uint16_t PeltierArrayHC, uint16_t * PeltierArrayPWM);

#endif
