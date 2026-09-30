#include <stdint.h>

#define RCC_APB2ENR (*(volatile uint32_t *)0x40021018)

#define AFIO_MAPR   (*(volatile uint32_t *)0x40010004)

#define GPIOA_CRL   (*(volatile uint32_t *)0x40010800)
#define GPIOA_CRH   (*(volatile uint32_t *)0x40010804)
#define GPIOA_IDR   (*(volatile uint32_t *)0x40010808)
#define GPIOA_ODR   (*(volatile uint32_t *)0x4001080C)

int main(void)
{
    /* Bat clock AFIO + GPIOA */
    RCC_APB2ENR |= (1 << 0);   // AFIO
    RCC_APB2ENR |= (1 << 2);   // GPIOA

    /*
     * Tat JTAG + SWD
     * De PA13, PA14, PA15 tro thanh GPIO
     */
    AFIO_MAPR &= ~(7 << 24);
    AFIO_MAPR |=  (4 << 24);

    /*
     * PA0 - PA7: Input Pull-up
     */
    GPIOA_CRL = 0x88888888;

    /*
     * PA8 - PA15: Output Push-Pull 2MHz
     */
    GPIOA_CRH = 0x22222222;

    /* Pull-up PA0 - PA7 */
    GPIOA_ODR |= 0x00FF;

    while (1)
    {
        uint16_t input;
        uint16_t output;

        input = GPIOA_IDR & 0x00FF;

        /* Dao bit */
        output = (~input) & 0x00FF;

        /* Dua ra PA8 - PA15 */
        GPIOA_ODR =
            (GPIOA_ODR & 0x00FF) |
            (output << 8);
    }
}
