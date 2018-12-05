#include "pwm.h"
#include "tim.h"


void FourPtAvg(uint8_t N, uint8_t * ADC_data, uint8_t ADC_data_buffer[][5], uint8_t * _ADC_buffer_pt, uint8_t * avg_data){
	uint16_t temp;
	for(int i=0; i<N; i++){
		ADC_data_buffer[*_ADC_buffer_pt][i] = ADC_data[i];
		temp=0;
		for(int j=0; j<4; j++){
			//sum up
			temp += ADC_data_buffer[j][i];
		}
		//do avg
		temp = temp >> 2;
		avg_data[i] = temp;
	}
	*_ADC_buffer_pt += 1;
}

void PWM_Peltier_Control(uint8_t * avg_in_data, uint8_t * avg_fb_data, int * errorSum){
	int PWM = 0;
	int tempPWM;
	uint16_t PeltierN = 1;
	int e;
	uint8_t HC = _COOLING;
	for(int i=0; i<5; i++){
			e = TemperatureLinearizeTable[avg_in_data[i]]-TemperatureLinearizeTable[avg_fb_data[i]];
			errorSum[i] = errorSum[i]+e;
			//limit the maximum number
			if (errorSum[i]>10000){errorSum[i]=10000;}
			else if(errorSum[i]<-10000){errorSum[i]=-10000;}

			tempPWM = P_H*e + I_H*(errorSum[i])/I_D_H;
			//choose Heating or Cooling and limit the PWM duty below period
			if(tempPWM>PWM_LIMIT) {
				PWM=PWM_LIMIT;
				HC = _HEATING;
			}else if(tempPWM>0) {
				PWM=tempPWM;
				HC = _HEATING;
			}else if(tempPWM>(PWM_LIMIT *(-1))) {
				PWM=tempPWM*(-1);
				HC = _COOLING;
			}else {
				PWM= PWM_LIMIT;
				HC = _COOLING;
			}
			PWM_Peltier_SetPWM(PeltierN, HC, PWM);

//			PWM_Peltier_SetPWM(PeltierN, _COOLING, 0);

		PWM_Peltier_Start(PeltierN);
		PeltierN = PeltierN << 1; //switch to next Peltier Cooler
	}
}

void PWM_Peltier_SetPWM(uint16_t PeltierN, uint8_t HC, uint16_t PWM){
	if(PeltierN == _PELTIER_1){
		if(HC == _HEATING){
			__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1, PWM);
			__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2, 0);
		}else{
			__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1, 0);
			__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2, PWM);
		}

	}else if(PeltierN == _PELTIER_2){
		//PELTIER_2
	}else if(PeltierN == _PELTIER_3){
		//PELTIER_3
	}else if(PeltierN == _PELTIER_4){
		//PELTIER_4
	}else{
		//PELTIER_5
	}
}

void PWM_Peltier_Start(uint16_t PeltierN){
	if(PeltierN == _PELTIER_1){
		HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
	}else if(PeltierN == _PELTIER_2){
		//PELTIER_2
	}else if(PeltierN == _PELTIER_3){
		//PELTIER_3
	}else if(PeltierN == _PELTIER_4){
		//PELTIER_4
	}else{
		//PELTIER_5
	}
}

void PWM_Peltier_SetPeltierArray(uint16_t PeltierArray, uint16_t PeltierArrayHC, uint16_t * PeltierArrayPWM){
	if(PeltierArray & _PELTIER_1){
		//set Peltier 1 pwm value
		PWM_Peltier_SetPWM(_PELTIER_1, PeltierArrayHC & _PELTIER_1, PeltierArrayPWM[0]);
	}if(PeltierArray & _PELTIER_2){
		//PELTIER_2
		PWM_Peltier_SetPWM(_PELTIER_2, PeltierArrayHC & _PELTIER_2, PeltierArrayPWM[1]);
	}if(PeltierArray & _PELTIER_3){
		//PELTIER_3
		PWM_Peltier_SetPWM(_PELTIER_3, PeltierArrayHC & _PELTIER_3, PeltierArrayPWM[2]);
	}if(PeltierArray & _PELTIER_4){
		//PELTIER_4
		PWM_Peltier_SetPWM(_PELTIER_4, PeltierArrayHC & _PELTIER_4, PeltierArrayPWM[3]);
	}if(PeltierArray & _PELTIER_5){
		//PELTIER_5
		PWM_Peltier_SetPWM(_PELTIER_5, PeltierArrayHC & _PELTIER_5, PeltierArrayPWM[4]);
	}
}

//TIM_OCPOLARITY_HIGH & TIM_OCPOLARITY_LOW
//void PWM_SetPolarity(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t OCPolarity){
//	uint32_t tmpccer = 0;
//
//	if(Channel == TIM_CHANNEL_1){
//		/* Disable the Channel 1: Reset the CC1E Bit */
//		TIMx->CCER &= ~TIM_CCER_CC1E;
//		/* Get the TIMx CCER register value */
//		tmpccer = TIMx->CCER;
//		/* Reset the Output Polarity level */
//		tmpccer &= ~TIM_CCER_CC1P;
//		/* Set the Output Compare Polarity */
//		tmpccer |= OCPolarity;
//		/* Write to TIMx CCER */
//		TIMx->CCER = tmpccer;
//	}else if(Channel == TIM_CHANNEL_2){
//		/* Disable the Channel 2: Reset the CC2E Bit */
//		TIMx->CCER &= ~TIM_CCER_CC2E;
//		/* Get the TIMx CCER register value */
//		tmpccer = TIMx->CCER;
//		/* Reset the Output Polarity level */
//		tmpccer &= ~TIM_CCER_CC2P;
//		/* Set the Output Compare Polarity */
//		tmpccer |= (OCPolarity << 4);
//		/* Write to TIMx CCER */
//		TIMx->CCER = tmpccer;
//	}else if(Channel == TIM_CHANNEL_3){
//		/* Disable the Channel 3: Reset the CC3E Bit */
//		TIMx->CCER &= ~TIM_CCER_CC3E;
//		/* Get the TIMx CCER register value */
//		tmpccer = TIMx->CCER;
//		/* Reset the Output Polarity level */
//		tmpccer &= ~TIM_CCER_CC3P;
//		/* Set the Output Compare Polarity */
//		tmpccer |= (OCPolarity << 8);
//		/* Write to TIMx CCER */
//		TIMx->CCER = tmpccer;
//	}else{
//		/* Disable the Channel 4: Reset the CC4E Bit */
//		TIMx->CCER &= ~TIM_CCER_CC4E;
//		/* Get the TIMx CCER register value */
//		tmpccer = TIMx->CCER;
//		/* Reset the Output Polarity level */
//		tmpccer &= ~TIM_CCER_CC4P;
//		/* Set the Output Compare Polarity */
//		tmpccer |= (OCPolarity << 12);
//		/* Write to TIMx CCER */
//		TIMx->CCER = tmpccer;
//	}
//}
//

//void PWM_Peltier_SetHC(uint16_t PeltierN, uint8_t HC){
//	if(PeltierN == _PELTIER_1){
//		if(HC == _HEATING){
//			PWM_SetPolarity(TIM5, TIM_CHANNEL_1, TIM_OCPOLARITY_HIGH);
//			PWM_SetPolarity(TIM5, TIM_CHANNEL_2, TIM_OCPOLARITY_LOW);
//		}else{
//			PWM_SetPolarity(TIM5, TIM_CHANNEL_1, TIM_OCPOLARITY_LOW);
//			PWM_SetPolarity(TIM5, TIM_CHANNEL_2, TIM_OCPOLARITY_HIGH);
//		}
//	}else if(PeltierN == _PELTIER_2){
//		//PELTIER_2
//	}else if(PeltierN == _PELTIER_3){
//		//PELTIER_3
//	}else if(PeltierN == _PELTIER_4){
//		//PELTIER_4
//	}else{
//		//PELTIER_5
//	}
//}




//void PWM_Init_PeltierArray(){
//	PWM_Peltier_SetHC(_PELTIER_1, _COOLING);
//	PWM_Peltier_SetPWM(_PELTIER_1, _COOLING,0);
//	PWM_Peltier_SetHC(_PELTIER_2, _COOLING);
//	PWM_Peltier_SetPWM(_PELTIER_2, _COOLING,0);
//	PWM_Peltier_SetHC(-PELTIER_3, _COOLING);
//	PWM_Peltier_SetPWM(_PELTIER_3, _COOLING,0);
//	PWM_Peltier_SetHC(-PELTIER_4, _COOLING);
//	PWM_Peltier_SetPWM(_PELTIER_4, _COOLING,0);
//	PWM_Peltier_SetHC(_PELTIER_5, _COOLING);
//	PWM_Peltier_SetPWM(_PELTIER_5, _COOLING,0);
//}


//void PWM_Peltier_SetPeltierArray(uint16_t PeltierArray, uint16_t PeltierArrayHC_next, uint16_t *PeltierArrayHC_now, uint16_t *PeltierArrayPWM){
//	if(PeltierArray & _PELTIER_1){
//		//change cooling or heating if needed
//		if((PeltierArrayHC_next & _PELTIER_1) != (*PeltierArrayHC_now & _PELTIER_1)){
//			PWM_Peltier_SetHC(_PELTIER_1, PeltierArrayHC_next & _PELTIER_1);
//			*PeltierArrayHC_now = PeltierArrayHC_next;
//		}
//		//set Peltier 1 pwm value
//		PWM_Peltier_SetPWM(_PELTIER_1, *PeltierArrayHC_now & _PELTIER_1, PeltierArrayPWM[0]);
//
//		}if(PeltierArray & _PELTIER_2){
//			//PELTIER_2
//		}if(PeltierArray & _PELTIER_3){
//			//PELTIER_3
//		}if(PeltierArray & _PELTIER_4){
//			//PELTIER_4
//		}if(PeltierArray & _PELTIER_5){
//			//PELTIER_5
//		}
//}

















///**
//  * @brief  Time Ouput Compare 1 configuration
//  * @param  TIMx to select the TIM peripheral
//  * @param  OC_Config: The ouput configuration structure
//  * @retval None
//  */
//static void TIM_OC1_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config)
//{
//  uint32_t tmpccmrx = 0;
//  uint32_t tmpccer = 0;
//  uint32_t tmpcr2 = 0;
//
//   /* Disable the Channel 1: Reset the CC1E Bit */
//  TIMx->CCER &= ~TIM_CCER_CC1E;
//
//  /* Get the TIMx CCER register value */
//  tmpccer = TIMx->CCER;
//  /* Get the TIMx CR2 register value */
//  tmpcr2 =  TIMx->CR2;
//
//  /* Get the TIMx CCMR1 register value */
//  tmpccmrx = TIMx->CCMR1;
//
//  /* Reset the Output Compare Mode Bits */
//  tmpccmrx &= ~TIM_CCMR1_OC1M;
//  tmpccmrx &= ~TIM_CCMR1_CC1S;
//  /* Select the Output Compare Mode */
//  tmpccmrx |= OC_Config->OCMode;
//
//  /* Reset the Output Polarity level */
//  tmpccer &= ~TIM_CCER_CC1P;
//  /* Set the Output Compare Polarity */
//  tmpccer |= OC_Config->OCPolarity;
//
//  /* Write to TIMx CR2 */
//  TIMx->CR2 = tmpcr2;
//
//  /* Write to TIMx CCMR1 */
//  TIMx->CCMR1 = tmpccmrx;
//
//  /* Set the Capture Compare Register value */
//  TIMx->CCR1 = OC_Config->Pulse;
//
//  /* Write to TIMx CCER */
//  TIMx->CCER = tmpccer;
//}
//
///**
//  * @brief  Time Ouput Compare 2 configuration
//  * @param  TIMx to select the TIM peripheral
//  * @param  OC_Config: The ouput configuration structure
//  * @retval None
//  */
//static void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config)
//{
//  uint32_t tmpccmrx = 0;
//  uint32_t tmpccer = 0;
//  uint32_t tmpcr2 = 0;
//
//  /* Disable the Channel 2: Reset the CC2E Bit */
//  TIMx->CCER &= ~TIM_CCER_CC2E;
//
//  /* Get the TIMx CCER register value */
//  tmpccer = TIMx->CCER;
//  /* Get the TIMx CR2 register value */
//  tmpcr2 =  TIMx->CR2;
//
//  /* Get the TIMx CCMR1 register value */
//  tmpccmrx = TIMx->CCMR1;
//
//  /* Reset the Output Compare mode and Capture/Compare selection Bits */
//  tmpccmrx &= ~TIM_CCMR1_OC2M;
//  tmpccmrx &= ~TIM_CCMR1_CC2S;
//
//  /* Select the Output Compare Mode */
//  tmpccmrx |= (OC_Config->OCMode << 8);
//
//  /* Reset the Output Polarity level */
//  tmpccer &= ~TIM_CCER_CC2P;
//  /* Set the Output Compare Polarity */
//  tmpccer |= (OC_Config->OCPolarity << 4);
//
//  /* Write to TIMx CR2 */
//  TIMx->CR2 = tmpcr2;
//
//  /* Write to TIMx CCMR1 */
//  TIMx->CCMR1 = tmpccmrx;
//
//  /* Set the Capture Compare Register value */
//  TIMx->CCR2 = OC_Config->Pulse;
//
//  /* Write to TIMx CCER */
//  TIMx->CCER = tmpccer;
//}
