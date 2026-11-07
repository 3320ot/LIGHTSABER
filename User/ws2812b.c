#include <debug.h>
#include <ws2812b.h>
#include <stdlib.h>

static volatile uint8_t led_data[BUF_SIZE] = {0}; // 24bits per led x 3x encoding

void WS2812bInit() {
    RCC_PB2PeriphClockCmd(
            RCC_PB2Periph_GPIOC |
            RCC_PB2Periph_SPI1,
            ENABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    NVIC_EnableIRQ(SPI1_IRQn);
    SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    SPI_Cmd(SPI1, ENABLE);

    Delay_Ms(50);
}

void SPI1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SPI1_IRQHandler(void) {
    if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) != RESET) {
        SPI_I2S_ClearFlag(SPI1, SPI_I2S_IT_TXE);
        if (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == RESET) {
            SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_BSY);
            GPIO_InitTypeDef GPIO_InitStructure = {0};

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
            GPIO_Init(GPIOC, &GPIO_InitStructure);

            SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
        } /* TODO reset flag */
    }
}

void WS2812_Send()
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    DMA_Cmd(DMA1_Channel3, DISABLE);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)led_data;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = BUF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    DMA_ClearFlag(DMA1_FLAG_TC3 | DMA1_FLAG_HT3 | DMA1_FLAG_TE3);

    DMA_Cmd(DMA1_Channel3, ENABLE);

//    for (int i = 0; i < NUM_LEDS; ++i) {
//        printf("%d: ", i);
//        for (int var = 0; var < 9; ++var) {
//            u8 byte = led_data[i*9 + var];
//            printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(byte));
//        }
//        printf("\n");
//    }
//    printf("\n");
}

void DMA1_Channel3_IRQHandler(void)__attribute__((interrupt));
void DMA1_Channel3_IRQHandler(void){
    if(DMA1->INTFR & DMA_TCIF1){
        DMA1->INTFCR = DMA_TCIF1;
    }
}

void WS2812bFillSolid(RGB color){
    for (u16 led = 0; led < NUM_LEDS; ++led){
    }
}

void WS2812bClear(){
    static const uint8_t pattern[3] = { 0x24, 0x92, 0x49 }; // "001" x 8
    for (uint32_t i = 0; i < BUF_SIZE; ++i)
        led_data[i] = pattern[i % 3];
    for (uint32_t i = BUF_SIZE; i < BUF_SIZE; ++i)
        led_data[i] = 0;
}

void WS2812bSetLed(u16 index, RGB color) {
    const u16 offset = index * 9;

    const uint32_t v = ((uint32_t)color.r)
                     | ((uint32_t)color.g << 7)
                     | ((uint32_t)color.b << 15);

    for (int i = 0; i < 9; ++i)
        led_data[offset + i] = 0;

    uint8_t spi_bit = 0;
    for (int8_t v_bit = 23; v_bit >= 0; --v_bit) {
        if (v & (1u << v_bit)) {
            // "110"
//            printf("110 ");
            led_data[offset + (spi_bit >> 3)] |= (uint8_t)(1u << (7 - (spi_bit & 7)));
            ++spi_bit;
            led_data[offset + (spi_bit >> 3)] |= (uint8_t)(1u << (7 - (spi_bit & 7)));
            ++spi_bit;
            ++spi_bit;
        } else {
            // "001"
//            printf("001 ");
            ++spi_bit;
            ++spi_bit;
            led_data[offset + (spi_bit >> 3)] |= (uint8_t)(1u << (7 - (spi_bit & 7)));
            ++spi_bit;
        }
    }
//    printf("%d", spi_bit);
//    for (int i = 0; i < 3; ++i) {
//        printf("%d: ", i);
//        for (int var = 0; var < 9; ++var) {
//            u8 byte = led_data[i*9 + var];
//            printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(byte));
//        }
//        printf("\n");
//    }
//    printf("\n");
}
