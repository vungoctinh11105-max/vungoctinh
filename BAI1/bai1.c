#include "stm32f1xx.h"

#define LED_PIN     13
#define LED_PORT    GPIOC

void SysTick_Delay(uint32_t ms) {
    SysTick->LOAD = (SystemCoreClock / 1000) - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_ENABLE_Msk;

    for (uint32_t i = 0; i < ms; i++) {
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }

    SysTick->CTRL = 0;
}

int main(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    LED_PORT->CRH &= ~(0xFUL << 20);
    LED_PORT->CRH |= (0x2UL << 20);

    while (1) {
        LED_PORT->BRR = (1UL << LED_PIN);
        SysTick_Delay(500);

        LED_PORT->BSRR = (1UL << LED_PIN);
        SysTick_Delay(500);
    }
}
