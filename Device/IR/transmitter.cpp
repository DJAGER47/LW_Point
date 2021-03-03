/*
 * transmitter.cpp
 *
 *  Created on: Jan 6, 2020
 *      Author: Admin
 */

#include "transmitter.h"
#include "define.h"
#include "main.h"

static DMA_Channel_TypeDef* DMAy_Channelx = DMA2_Channel4;
static TIM_TypeDef* TIMs = TIM3;
static TIM_TypeDef* TIMm = TIM5;

static uint16_t tBuf[bufDataT];

extern "C" void DMA2_Channel4_IRQHandler(void){
	//011: Toggle - OC1REF toggles when TIMx_CNT=TIMx_CCR1.
	//100: Force inactive level - OC1REF is forced low.
	//101: Force active level - OC1REF is forced high.
	uint16_t tmpccmrx = TIMm->CCMR1;
	tmpccmrx &= (uint16_t) (~ TIM_CCMR1_OC2M);
	tmpccmrx |= (uint16_t) (TIM_OCMODE_FORCED_INACTIVE << 8);
	TIMm->CCMR1 = tmpccmrx;

	TIMm->CR1 &= (uint16_t) (~((uint16_t) TIM_CR1_CEN)); // off
	TIMs->CR1 &= (uint16_t) (~((uint16_t) TIM_CR1_CEN)); // off

	DMAy_Channelx->CCR &= (uint16_t) (~DMA_CCR_EN);

	const uint32_t DMA2_IT_TC4 = ((uint32_t)0x10002000);
	DMA2->IFCR = DMA2_IT_TC4; //DMA_ClearITPendingBit(DMA1_IT_TC5);

	GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TIM3_CH1_Transmit_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TIM3_CH1_Transmit_GPIO_Port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(TIM3_CH1_Transmit_GPIO_Port, TIM3_CH1_Transmit_Pin, GPIO_PIN_RESET);
}

void Transmitter::Init_TIM_Slave(){
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_AFIO_REMAP_TIM3_ENABLE();

	/*GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TIM3_CH1_Transmit_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TIM3_CH1_Transmit_GPIO_Port, &GPIO_InitStruct);*/

	{//TIM_TimeBaseInit(TIM3, &timer_base);
		uint16_t tmpcr1 = TIMs->CR1;
		// Select the Counter Mode
		tmpcr1 &= (uint16_t) (~((uint16_t) (TIM_CR1_DIR | TIM_CR1_CMS)));
		tmpcr1 |= (uint32_t) TIM_COUNTERMODE_UP;

		tmpcr1 &= (uint16_t) (~((uint16_t) TIM_CR1_CKD));
		tmpcr1 |= (uint32_t) TIM_CLOCKDIVISION_DIV1;
		TIMs->CR1 = tmpcr1;

		TIMs->ARR = 1285;
		TIMs->PSC = 0;
		TIMs->EGR = TIM_EVENTSOURCE_UPDATE;
	}

	{//TIM_OC1Init(TIM3, &OCInitStruct);
		// Disable the Channel 1: Reset the CC1E Bit
		TIMs->CCER &= (uint16_t) (~(uint16_t) TIM_CCER_CC1E);

		uint16_t tmpccmrx = TIMs->CCMR1;
		tmpccmrx &= (uint16_t) (~((uint16_t) TIM_CCMR1_OC1M));
		tmpccmrx &= (uint16_t) (~((uint16_t) TIM_CCMR1_CC1S));
		tmpccmrx |= TIM_OCMODE_PWM1;

		uint16_t tmpccer = TIMs->CCER;
		tmpccer &= (uint16_t) (~((uint16_t) TIM_CCER_CC1P));
		tmpccer |= TIM_OCPOLARITY_LOW;
		tmpccer |= TIM_OUTPUTSTATE_ENABLE;

		TIMs->CCMR1 = tmpccmrx;
		TIMs->CCR1 = 643;
		TIMs->CCER = tmpccer;
	}
	{//TIM_SelectInputTrigger(TIM3, TIM_TS_ITR2);
		uint16_t tmpsmcr = TIMs->SMCR;
		tmpsmcr &= (uint16_t) (~((uint16_t) TIM_SMCR_TS));
		tmpsmcr |= TIM_CLOCKSOURCE_ITR2;
		TIMs->SMCR = tmpsmcr;
	}

	{//TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);
		TIMs->SMCR &= (uint16_t) ~((uint16_t) TIM_SMCR_SMS);
		TIMs->SMCR |= TIM_SLAVEMODE_GATED;
	}
}

void Transmitter::Init_TIM_Master(){
	{//TIM_TimeBaseInit(TIM5, &timer_base);
		uint16_t tmpcr1 = TIMm->CR1;
		// Select the Counter Mode
		tmpcr1 &= (uint16_t) (~((uint16_t) (TIM_CR1_DIR | TIM_CR1_CMS)));
		tmpcr1 |= (uint32_t) TIM_COUNTERMODE_UP;

		tmpcr1 &= (uint16_t) (~((uint16_t) TIM_CR1_CKD));
		tmpcr1 |= (uint32_t) TIM_CLOCKDIVISION_DIV1;
		TIMm->CR1 = tmpcr1;

		TIMm->ARR = 0xFFFF;
		TIMm->PSC = 1285;
		TIMm->EGR = TIM_EVENTSOURCE_UPDATE;
	}

	{//TIM_OC2Init(TIM5, &OCInitStruct);
		TIMm->CCER &= (uint16_t) (~(uint16_t) TIM_CCER_CC2E);

		uint16_t tmpccmrx = TIMm->CCMR1;
		tmpccmrx &= (uint16_t) (~((uint16_t) TIM_CCMR1_OC2M));
		tmpccmrx &= (uint16_t) (~((uint16_t) TIM_CCMR1_CC2S));
		tmpccmrx |= (uint16_t) (TIM_OCMODE_FORCED_INACTIVE << 8);

		uint16_t tmpccer = TIMm->CCER;
		tmpccer &= (uint16_t) (~((uint16_t) TIM_CCER_CC2P));
		tmpccer |= (uint16_t) (TIM_OCPOLARITY_HIGH << 4);
		tmpccer |= (uint16_t) (TIM_OUTPUTSTATE_DISABLE << 4);

		TIMm->CCMR1 = tmpccmrx;
		TIMm->CCR2 = tBuf[0];
		TIMm->CCER = tmpccer;
	}

	{//TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Disable);
		TIMm->CCMR1 &= (uint16_t) ~((uint16_t) TIM_CCMR1_OC2PE);
	}

	{//TIM_SelectOutputTrigger(TIM5, TIM_TRGOSource_OC1Ref);
		TIMm->CR2 &= (uint16_t) ~((uint16_t) TIM_CR2_MMS);
		TIMm->CR2 |= TIM_TRGO_OC2REF;
	}

	//TIM_DMACmd(TIM5, TIM_DMA_CC1, ENABLE);
	TIMm->DIER |= TIM_DMA_CC1;
}


void Transmitter::init(uint8_t power){
	Init_TIM_Slave();
	Init_TIM_Master();

	{//DMA_Init(DMA2_TIM5_CH1, &DMA_InitStructure);
		uint32_t tmpreg = DMAy_Channelx->CCR;
		const uint32_t CCR_CLEAR_Mask = ((uint32_t)0xFFFF800F);
		tmpreg &= CCR_CLEAR_Mask;

		tmpreg |= DMA_MEMORY_TO_PERIPH
				| DMA_NORMAL
				| DMA_PINC_DISABLE
				| DMA_MINC_ENABLE
				| DMA_PDATAALIGN_HALFWORD
				| DMA_MDATAALIGN_HALFWORD
				| DMA_PRIORITY_HIGH;

		DMAy_Channelx->CCR = tmpreg;


		DMAy_Channelx->CNDTR = 0;
		DMAy_Channelx->CPAR = (uint32_t) &TIM5->CCR2;
		DMAy_Channelx->CMAR = (uint32_t) &(tBuf[1]);
	}

	{
	//DMA_ITConfig(DMA2_TIM5_CH1, DMA_IT_TC, ENABLE);
	//DMA_ITConfig(DMA2_TIM5_CH1, DMA_IT_HT | DMA_IT_TE , DISABLE);
		DMAy_Channelx->CCR |= DMA_IT_TC;
		DMAy_Channelx->CCR &= ~DMA_IT_HT | DMA_IT_TE;
	}

	HAL_NVIC_SetPriority(DMA2_Channel4_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Channel4_IRQn);

	tBuf[0] = msGap;
	tBuf[1] = msGap + ms2400;
	tBuf[2] = msGap + ms2400 + msGap;
	printf("Start Transmitter\r\n");
	updPower(power);
}

bool Transmitter::busy(){
	return  (bool)(DMAy_Channelx->CCR & DMA_CCR_EN);
}

void Transmitter::sendHardware(uint8_t len){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TIM3_CH1_Transmit_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TIM3_CH1_Transmit_GPIO_Port, &GPIO_InitStruct);

    TIMm->CCR1 = tBuf[0];
	DMAy_Channelx->CMAR = (uint32_t) &(tBuf[1]);
	DMAy_Channelx->CNDTR = len;
	DMAy_Channelx->CCR |= DMA_CCR_EN;
	TIMm->CCMR1 = (uint16_t) (TIM_OCMODE_TOGGLE << 8);

	TIMs->CNT = 0;
	TIMm->CNT = 0;
	TIMs->CR1 |= TIM_CR1_CEN;
	TIMm->CR1 |= TIM_CR1_CEN;
}

void Transmitter::send(IRR_DATA data){
	uint8_t index = 3;
	uint8_t max = data.shot.zero ? bufDataT : bufShotT;
	while (index < max) {
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
	sendHardware(max);
}

void Transmitter::updPower(uint8_t pwr) {
	uint16_t power = (uint16_t) (1285 - 643.0 * (pwr / 100.0));
	TIMs->CCR1 = power;//TIM_SetCompare1(TIM3, power);
	TIMs->CR1 &= ~TIM_CR1_CEN;
	TIMm->CR1 &= ~TIM_CR1_CEN;
}

