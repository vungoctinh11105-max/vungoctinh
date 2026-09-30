#include "stm32f10x.h"
#include "uart.h"

/* ================= CAU HINH ================= */
#define ID_LOP   "20261-03"
#define ID_NHOM  "Nhom09"

#define BUTTON_PORT   GPIOA
#define BUTTON_PIN    GPIO_Pin_0
#define BUTTON_RCC    RCC_APB2Periph_GPIOA

/* ================= BIEN TOAN CUC ================= */
volatile uint16_t btn_count = 0;
char tx_buffer[64];

/* Ham rong de ngan GCC goi SystemInit tu system_stm32f10x.c */
void SystemInit(void) {
}

/* Cau hinh Clock he thong chay o 72MHz su dung thach anh ngoai */
void SystemClock_Config(void) {
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
    RCC->CFGR |= (RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

/* Delay don gian bang vong lap (khong dung Timer/SysTick) */
void Delay_ms(uint32_t ms) {
    volatile uint32_t i;
    for (; ms > 0; ms--) {
        for (i = 0; i < 6000; i++);
    }
}

/* ================= CAU HINH NUT NHAN (PA0, pull-up noi) ================= */
void Button_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(BUTTON_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = BUTTON_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   /* Input pull-up */
    GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);
}

/* Doc trang thai nut: tra ve 1 neu dang duoc nhan (keo xuong GND) */
uint8_t Button_IsPressed(void) {
    return (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_RESET);
}

/* ================= CAU HINH DMA CHO USART1 TX ================= */
/* USART1_TX dung DMA1 Channel4 tren STM32F103 */
void DMA1_USART1_TX_Config(void) {
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)tx_buffer;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = 0;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    /* Cho phep USART1 nhan yeu cau DMA khi truyen (TX) */
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

/* Gui du lieu qua DMA (KHONG dung ham cho UART/DMA - chi khoi tao va bat) */
void DMA_SendString(char* str) {
    uint16_t len = 0;
    while (str[len] != '\0') len++;

    /* Tat kenh de nap lai thong so truyen moi */
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC4);
    DMA_SetCurrDataCounter(DMA1_Channel4, len);
    DMA_Cmd(DMA1_Channel4, ENABLE);
}

/* Chuyen so nguyen sang chuoi ky tu, tra ve so ky tu da ghi */
uint8_t UintToStr(uint16_t value, char* out) {
    char tmp[6];
    uint8_t n = 0, i;

    if (value == 0) {
        out[0] = '0';
        return 1;
    }
    while (value > 0) {
        tmp[n++] = (value % 10) + '0';
        value /= 10;
    }
    for (i = 0; i < n; i++) {
        out[i] = tmp[n - 1 - i];
    }
    return n;
}

/* Ghep ban tin theo dung cau truc: <ID-Lop><ID-Nhom>:BTN:<Gia tri>\n\r */
void BuildAndSendMessage(uint16_t value) {
    uint8_t idx = 0;
    const char* p;
    char num_str[6];
    uint8_t num_len, i;

    p = ID_LOP;
    while (*p) tx_buffer[idx++] = *p++;

    p = ID_NHOM;
    while (*p) tx_buffer[idx++] = *p++;

    p = ":BTN:";
    while (*p) tx_buffer[idx++] = *p++;

    num_len = UintToStr(value, num_str);
    for (i = 0; i < num_len; i++) tx_buffer[idx++] = num_str[i];

    tx_buffer[idx++] = '\n';
    tx_buffer[idx++] = '\r';
    tx_buffer[idx] = '\0';

    /* Doi lan gui truoc do hoan tat de tranh chong du lieu */
    while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET &&
           DMA_GetCurrDataCounter(DMA1_Channel4) != 0);

    DMA_SendString(tx_buffer);
}

int main(void) {
    uint8_t last_state = 0; /* 0 = nha, 1 = dang nhan */

    SystemClock_Config();

    USART1_Init(115200);
    Button_Init();
    DMA1_USART1_TX_Config();

    while (1) {
        if (Button_IsPressed()) {
            if (last_state == 0) {
                Delay_ms(30); /* chong doi phim (debounce) */
                if (Button_IsPressed()) {
                    last_state = 1;
                    btn_count++;
                    BuildAndSendMessage(btn_count);
                }
            }
        } else {
            last_state = 0;
        }
    }
}
