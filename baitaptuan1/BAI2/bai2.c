#include "stm32f1xx.h"

#define LED_PORT    GPIOA
#define LED_ALL     0xFF

void delay(uint32_t count) {
    while (count--);
}

int main(void) {
    RCC->APB2ENR |= (1 << 2);
    LED_PORT->CRL = 0x22222222;

    while (1) {
        for (int i = 0; i < 8; i++) {
            LED_PORT->BRR = LED_ALL;
            LED_PORT->BSRR = (1UL << i);
            delay(500000);
        }
        for (int i = 6; i >= 1; i--) {
            LED_PORT->BRR = LED_ALL;
            LED_PORT->BSRR = (1UL << i);
            delay(500000);
        }
    }
}