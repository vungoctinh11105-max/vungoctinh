#include "stm32f10x.h"
#include <stdint.h>

int main(void)
{
    uint32_t button_state;
    uint32_t old_button_state = 0;
    uint32_t led_state = 0;

    /* =========================
       Bật clock GPIOA
       ========================= */
    RCC->APB2ENR |= (1 << 2);

    /* =========================
       PA0 = Input Pull-down
       CNF = 10, MODE = 00
       => 0x8
       ========================= */
    GPIOA->CRL &= ~(0xF << 0);
    GPIOA->CRL |=  (0x8 << 0);

    /* Pull-down */
    GPIOA->ODR &= ~(1 << 0);

    /* =========================
       PA1 = Output Push-Pull 50MHz
       MODE = 11, CNF = 00
       => 0x3
       ========================= */
    GPIOA->CRL &= ~(0xF << 4);
    GPIOA->CRL |=  (0x3 << 4);

    /* LED ban đầu tắt */
    GPIOA->ODR &= ~(1 << 1);

    /* =========================
       Vòng lặp chính
       ========================= */
    while (1)
    {
        /* Đọc PA0 */
        button_state = GPIOA->IDR & (1 << 0);

        /* Phát hiện nhấn */
        if (button_state && !old_button_state)
        {
            /* Chờ người dùng nhả nút */
            while (GPIOA->IDR & (1 << 0))
            {
                /* Không làm gì */
            }

            /* Đảo trạng thái LED */
            led_state = !led_state;

            if (led_state)
            {
                GPIOA->ODR |= (1 << 1);
            }
            else
            {
                GPIOA->ODR &= ~(1 << 1);
            }
        }

        old_button_state = button_state;
    }
}