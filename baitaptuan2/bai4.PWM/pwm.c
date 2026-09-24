#include <stdint.h>

/* =========================================================
 * RCC & GPIOA
 * ========================================================= */
#define RCC_BASE        0x40021000UL
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x1C))

#define GPIOA_BASE      0x40010800UL
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))

/* =========================================================
 * TIM2
 * ========================================================= */
#define TIM2_BASE       0x40000000UL
#define TIM2_CR1        (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM2_EGR        (*(volatile uint32_t *)(TIM2_BASE + 0x14))
#define TIM2_CCMR1      (*(volatile uint32_t *)(TIM2_BASE + 0x18))
#define TIM2_CCMR2      (*(volatile uint32_t *)(TIM2_BASE + 0x1C))
#define TIM2_CCER       (*(volatile uint32_t *)(TIM2_BASE + 0x20))
#define TIM2_PSC        (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM2_ARR        (*(volatile uint32_t *)(TIM2_BASE + 0x2C))
#define TIM2_CCR1       (*(volatile uint32_t *)(TIM2_BASE + 0x34))
#define TIM2_CCR2       (*(volatile uint32_t *)(TIM2_BASE + 0x38))
#define TIM2_CCR3       (*(volatile uint32_t *)(TIM2_BASE + 0x3C))
#define TIM2_CCR4       (*(volatile uint32_t *)(TIM2_BASE + 0x40))

int main(void) {
    // 1. Enable clock GPIOA (bit 2) & TIM2 (bit 0)
    RCC_APB2ENR |= (1U << 2);
    RCC_APB1ENR |= (1U << 0);

    // 2. Configure PA0, PA1, PA2, PA3 -> Alternate Function Push-Pull (0xB)
    GPIOA_CRL &= ~0xFFFFU;
    GPIOA_CRL |= (0xBU << 0) | (0xBU << 4) | (0xBU << 8) | (0xBU << 12);

    // 3. Timer frequency: 8MHz / (7 + 1) = 1MHz -> 1 count = 1us
    TIM2_PSC = 7;

    // 4. PWM period: 1000 counts = 1ms -> Freq = 1kHz
    TIM2_ARR = 999;

    // 5. Duty cycle: 10%, 30%, 50%, 70%
    TIM2_CCR1 = 100;
    TIM2_CCR2 = 300;
    TIM2_CCR3 = 500;
    TIM2_CCR4 = 700;

    // 6. PWM mode 1 & Enable Preload (CH1, CH2, CH3, CH4)
    TIM2_CCMR1 = (6U << 4) | (1U << 3) | (6U << 12) | (1U << 11);
    TIM2_CCMR2 = (6U << 4) | (1U << 3) | (6U << 12) | (1U << 11);

    // 7. Enable all 4 PWM outputs
    TIM2_CCER = (1U << 0) | (1U << 4) | (1U << 8) | (1U << 12);

    // 8. Generate update event & Start TIM2
    TIM2_EGR = 1;
    TIM2_CR1 = (1U << 7) | (1U << 0);

    while (1) {
        // Vòng lặp rỗng, phần cứng tự động xuất PWM
    }
    return 0;
}
