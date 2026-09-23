#include "stm32f10x.h"

/* =========================
   Delay khoảng 1 ms
   ========================= */
void delay_ms(uint32_t ms)
{
    uint32_t i;
    uint32_t j;

    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 7200; j++)
        {
            __NOP();
        }
    }
}

/* =========================
   USART1 gửi 1 ký tự
   PA9 = TX
   PA10 = RX
   ========================= */
void USART1_SendChar(char c)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }

    USART_SendData(USART1, c);
}

/* =========================
   USART1 gửi chuỗi
   ========================= */
void USART1_SendString(char *str)
{
    while (*str != '\0')
    {
        USART1_SendChar(*str);
        str++;
    }
}

/* =========================
   Gửi số nguyên qua UART
   ========================= */
void USART1_SendNumber(uint32_t number)
{
    char buffer[10];
    uint8_t i = 0;

    if (number == 0)
    {
        USART1_SendChar('0');
        return;
    }

    while (number > 0)
    {
        buffer[i] = '0' + (number % 10);
        number = number / 10;
        i++;
    }

    while (i > 0)
    {
        i--;
        USART1_SendChar(buffer[i]);
    }
}

/* =========================
   Cấu hình USART1
   ========================= */
void USART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1,
        ENABLE
    );

    /* PA9 = TX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PA10 = RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode =
        USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

/* =========================
   Cấu hình ADC1 - PA0
   PA0 = ADC1 Channel 0
   ========================= */
void ADC1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    /* Bật clock GPIOA + ADC1 */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_ADC1,
        ENABLE
    );

    /* ADC clock = 72 / 6 = 12 MHz */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    /* PA0 = Analog Input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Cấu hình ADC1 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;

    ADC_InitStructure.ADC_ScanConvMode = DISABLE;

    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;

    ADC_InitStructure.ADC_ExternalTrigConv =
        ADC_ExternalTrigConv_None;

    ADC_InitStructure.ADC_DataAlign =
        ADC_DataAlign_Right;

    ADC_InitStructure.ADC_NbrOfChannel = 1;

    ADC_Init(ADC1, &ADC_InitStructure);

    /* PA0 = ADC Channel 0 */
    ADC_RegularChannelConfig(
        ADC1,
        ADC_Channel_0,
        1,
        ADC_SampleTime_55Cycles5
    );

    /* Bật ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* Reset calibration */
    ADC_ResetCalibration(ADC1);

    while (ADC_GetResetCalibrationStatus(ADC1))
    {
    }

    /* Calibration */
    ADC_StartCalibration(ADC1);

    while (ADC_GetCalibrationStatus(ADC1))
    {
    }
}

/* =========================
   Đọc ADC
   ========================= */
uint16_t ADC1_Read(void)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
    {
    }

    return ADC_GetConversionValue(ADC1);
}

/* =========================
   MAIN
   ========================= */
int main(void)
{
    uint16_t adc_value;

    uint32_t voltage_mV;
    uint32_t voltage_V;
    uint32_t voltage_decimal;

    USART1_Config();
    ADC1_Config();

    USART1_SendString("STM32 ADC PA0 READY\r\n");

    while (1)
    {
        /* Đọc ADC */
        adc_value = ADC1_Read();

        /* Tính điện áp:
           Voltage = ADC * 3300 / 4095
        */
        voltage_mV =
            ((uint32_t)adc_value * 3300) / 4095;

        voltage_V = voltage_mV / 1000;

        voltage_decimal =
            (voltage_mV % 1000) / 10;

        /* Gửi ADC */
        USART1_SendString("ADC = ");
        USART1_SendNumber(adc_value);
        USART1_SendString("\r\n");

        /* Gửi điện áp */
        USART1_SendString("Voltage = ");
        USART1_SendNumber(voltage_V);
        USART1_SendChar('.');

        if (voltage_decimal < 10)
        {
            USART1_SendChar('0');
        }

        USART1_SendNumber(voltage_decimal);

        USART1_SendString(" V\r\n");

        /* Khoảng 1 giây */
        delay_ms(1000);
    }
}