#include <stdint.h>

// Định nghĩa địa chỉ thanh ghi Port A trên STM32F103
#define RCC_APB2ENR (*((volatile uint32_t *)0x40021018))
#define GPIOA_CRL   (*((volatile uint32_t *)0x40010800)) // PA0 - PA7
#define GPIOA_CRH   (*((volatile uint32_t *)0x40010804)) // PA8 - PA15
#define GPIOA_IDR   (*((volatile uint32_t *)0x40010808)) // Đọc dữ liệu Input
#define GPIOA_ODR   (*((volatile uint32_t *)0x4001080C)) // Ghi dữ liệu Output

int main(void) {
    // Cấp xung nhịp cho IOPBEN / IOPAEN (Port A)
    RCC_APB2ENR |= (1 << 2);

    // Cấu hình PA0 - PA7 là Input Floating (Mã cấu hình CRL: 0x4)
    // 0x44444444 tương ứng cho 8 chân từ PA0 đến PA7
    GPIOA_CRL = 0x44444444;

    // Cấu hình PA8 - PA15 là Output Push-Pull tốc độ 2MHz (Mã cấu hình CRH: 0x2)
    // 0x22222222 tương ứng cho 8 chân từ PA8 đến PA15
    GPIOA_CRH = 0x22222222;

    while (1) {
        // Đọc trạng thái từ PA0-PA7 (lấy 8 bit thấp)
        uint32_t input_val = GPIOA_IDR & 0xFF;

        // Đảo trạng thái dữ liệu (0 thành 1, 1 thành 0)
        uint32_t inverted_val = (~input_val) & 0xFF;

        // Dịch trái 8 bit để đưa dữ liệu từ cụm PA0-PA7 lên cụm PA8-PA15 cho LED
        uint32_t output_val = inverted_val << 8;

        // Ghi ra các chân LED từ PA8 đến PA15
        GPIOA_ODR = (GPIOA_ODR & 0x00FF) | output_val;
    }

    return 0;
}
