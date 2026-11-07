#include "debug.h"

#include "qma6100p.h"
#include "ws2812b.h"

#include "loop.h"

data geo;
float accel;

void DCDC_Config(){
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOD, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_SetBits(GPIOD, GPIO_Pin_7);
}

void AUXBconfig() {
    RCC_PB1PeriphClockCmd(RCC_PB1Periph_TIM3, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_TIM1 | RCC_PB2Periph_AFIO |
            RCC_PB2Periph_GPIOC, ENABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_OCInitTypeDef TIM3_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_5);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Period        = 309;
    TIM_TimeBaseInitStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = 0;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    TIM_SelectInputTrigger(TIM1, TIM_TS_ITR2);
    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);

    TIM_TimeBaseInitStructure.TIM_Period        = 2175;
    TIM_TimeBaseInitStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    TIM3_OCInitStructure.TIM_OCMode      = TIM_OCMode_Timing;
    TIM3_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM3_OCInitStructure.TIM_Pulse       = 2100;
    TIM3_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OC3Init(TIM3, &TIM3_OCInitStructure);

    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
    TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM1->CH4CVR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)loop_raw;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = sizeof(loop_raw) / sizeof(loop_raw[0]);
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    TIM_DMACmd(TIM3, TIM_DMA_CC3, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    SDI_Printf_Enable();

//    printf("SystemClk:%d\r\n",SystemCoreClock);
//    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    while (initQMA6100P() < 0x80);
    DCDC_Config();
    WS2812bInit();
    AUXBconfig();
    GPIO_ResetBits(GPIOC, GPIO_Pin_4);

    RGB rgb = {
        255,
        0,
        0
    };

    WS2812bClear();
    WS2812_Send();
    Delay_Ms(50);
//    printf("ok\n");

//    for (u16 i = 0; i < NUM_LEDS-1; ++i) {
//        WS2812bSetLed(i, rgb);
//    }
    WS2812_Send();
    Delay_Ms(50);
    while(1){
        for (u16 i = 0; i < NUM_LEDS-1; ++i) {
            WS2812bClear();
            WS2812bSetLed(i, rgb);
            WS2812_Send();
            Delay_Ms(5);
        }
    };

    uint8_t state = 0;
    while(1)
    {
        geo = readData();
//        printf("Accel speed: %d.%02d\n", geo);
        Delay_Ms(10);
//        if(state)
//            GPIO_SetBits(GPIOC, GPIO_Pin_0);
//        else
//            GPIO_ResetBits(GPIOC, GPIO_Pin_0);
        state = !state;
    }
}
