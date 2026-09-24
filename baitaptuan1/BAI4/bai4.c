#include "stm32f1xx.h"

void delay_ms(uint32_t ms) {
    SysTick->LOAD = 72000U - 1U;
    SysTick->VAL = 0;
    SysTick->CTRL = (1U << 2) | (1U << 0);
    (void)SysTick->CTRL;
    while (ms--) {
        while (!(SysTick->CTRL & (1U << 16))) {
        }
    }
    SysTick->CTRL = 0;
}

int main(void) {
    RCC->APB2ENR |= (1U << 4);

    GPIOC->CRH &= ~(0xFUL << 20);
    GPIOC->CRH |= (0x2UL << 20);
    GPIOC->CRH &= ~(0xFUL << 24);
    GPIOC->CRH |= (0x8UL << 24);

    GPIOC->ODR |= (1UL << 14);
    GPIOC->ODR |= (1UL << 13);

    uint8_t button_pressed = 0;

    while (1) {
        if (!(GPIOC->IDR & (1UL << 14))) {
            button_pressed = 1;
        }
        if ((GPIOC->IDR & (1UL << 14)) && button_pressed) {
            GPIOC->BSRR = (1UL << 13);
            button_pressed = 0;
            delay_ms(50);
        }
    }
}