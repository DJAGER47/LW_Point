/*
 * transmitter.cpp
 *
 *  Created on: Jan 6, 2020
 *      Author: Admin
 */

#include "transmitter.h"
#include "define.h"

#define DMA_TIM5 hdma_tim5_ch2
extern DMA_HandleTypeDef DMA_TIM5;

static uint16_t tBuf[bufDataT];

/*extern "C" void DMA2_Channel5_IRQHandler(void){ //#define DMA2_TIM5_CH1	DMA2_Channel5
	//011: Toggle - OC1REF toggles when TIMx_CNT=TIMx_CCR1.
	//100: Force inactive level - OC1REF is forced low.
	//101: Force active level - OC1REF is forced high.
	TIM5->CCMR1 = (TIM5->CCMR1 & (~ TIM_CCMR1_OC1M)) | TIM_CCMR1_OC1M_2; //OC1REF is forced low.

	TIM5->CR1 &= (uint16_t) (~((uint16_t) TIM_CR1_CEN)); // off
	TIM3->CR1 &= (uint16_t) (~((uint16_t) TIM_CR1_CEN)); // off

	DMA2_Channel5->CCR &= (uint16_t) (~DMA_CCR1_EN); //DMA_Cmd(DMA2_Channel5, DISABLE);

	DMA2->IFCR = DMA2_IT_TC5; //DMA_ClearITPendingBit(DMA1_IT_TC5);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = TIM3_1_IRT_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM3_1_IRT_Port, &GPIO_InitStructure);
	GPIO_ResetBits(TIM3_1_IRT_Port, TIM3_1_IRT_Pin);
}*/

/*void Transmitter::Init_TIM_Slave(){
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = TIM3_1_IRT_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM3_1_IRT_Port, &GPIO_InitStructure);


	TIM_TimeBaseInitTypeDef timer_base;
	TIM_TimeBaseStructInit(&timer_base);
	timer_base.TIM_Period = 1285;
	timer_base.TIM_Prescaler = 0;
	TIM_TimeBaseInit(TIM3, &timer_base);

	TIM_OCInitTypeDef OCInitStruct;
	TIM_OCStructInit(&OCInitStruct);
	OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	OCInitStruct.TIM_Pulse = 643;
	OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM3, &OCInitStruct);

	TIM_SelectInputTrigger(TIM3, TIM_TS_ITR2);
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);
}

void Transmitter::Init_TIM_Master(){
	TIM_TimeBaseInitTypeDef timer_base;
	TIM_TimeBaseStructInit(&timer_base);
	timer_base.TIM_Period = 0xFFFF;
	timer_base.TIM_Prescaler = 1285;
	TIM_TimeBaseInit(TIM5, &timer_base);

	TIM_OCInitTypeDef OCInitStruct;
	TIM_OCStructInit(&OCInitStruct);
	OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle;
	OCInitStruct.TIM_OutputState = TIM_OutputState_Disable; //Enable
	OCInitStruct.TIM_Pulse = tBuf[0];
	OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM5, &OCInitStruct);
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Disable);

	TIM_SelectOutputTrigger(TIM5, TIM_TRGOSource_OC1Ref);

	TIM_DMACmd(TIM5, TIM_DMA_CC1, ENABLE);
	TIM5->CCMR1 = (TIM5->CCMR1 & (~ TIM_CCMR1_OC1M)) | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0; //OC1REF is forced low.
}*/

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){

}

void Transmitter::init(uint8_t power){
	HAL_TIMEx_OCN_Start_DMA(&DMA_TIM5, TIM_CHANNEL_2, &(tBuf[1]), Length);


	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &TIM5->CCR1;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &(tBuf[1]);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	//DMA_InitStructure.DMA_BufferSize = 1 ;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA2_TIM5_CH1, &DMA_InitStructure);

	DMA_ITConfig(DMA2_TIM5_CH1, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA2_TIM5_CH1, DMA_IT_HT | DMA_IT_TE , DISABLE);
	NVIC_EnableIRQ(DMA2_Channel5_IRQn);

	tBuf[0] = msGap;
	tBuf[1] = msGap + ms2400;
	tBuf[2] = msGap + ms2400 + msGap;
	INFO("Start Transmitter\n");
	updPower(power);
}

bool Transmitter::busy(){
	return  (bool)(DMA2_Channel5->CCR & DMA_CCR1_EN);
}

void Transmitter::send(IRR_DATA data){
	uint8_t index = 3;
	uint8_t max = data.shot.zero ? bufDataT : bufShotT;
	while (index <= max) {
		if (data.data & 0x800000) {
			tBuf[index] = tBuf[index - 1] + ms1200;
			tBuf[index + 1] = tBuf[index] + msGap;
		} else {
			tBuf[index] = tBuf[index - 1] + ms600;
			tBuf[index + 1] = tBuf[index] + msGap;
		}
		data.data = data.data << 1;
		index += 2;
	}


	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = TIM3_1_IRT_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM3_1_IRT_Port, &GPIO_InitStructure);

	TIM5->CCR1 = tBuf[0];
	DMA2_TIM5_CH1->CMAR = (uint32_t) &(tBuf[1]);
	DMA2_TIM5_CH1->CNDTR = max;
	DMA2_TIM5_CH1->CCR |= DMA_CCR1_EN;
	TIM5->CCMR1 = (TIM5->CCMR1 & (~ TIM_CCMR1_OC1M)) | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0;
	TIM3->CNT = 0;
	TIM5->CNT = 0;
	TIM3->CR1 |= TIM_CR1_CEN;
	TIM5->CR1 |= TIM_CR1_CEN;
}

void Transmitter::updPower(uint8_t pwr) {
	uint16_t power = (uint16_t) (1285 - 643.0 * (pwr / 100.0));
	TIM_SetCompare1(TIM3, power);
	TIM3->CR1 &= ~TIM_CR1_CEN;
	TIM5->CR1 &= ~TIM_CR1_CEN;
}

