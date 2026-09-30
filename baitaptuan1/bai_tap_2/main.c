cat << 'EOF' > src/main.c
#include <stdint.h>

// Địa chỉ thanh ghi cấp xung nhịp (Clock)
#define RCC_APB2ENR (*((volatile uint32_t *)0x40021018))

// Địa chỉ thanh ghi Port A
#define GPIOA_CRL   (*((volatile uint32_t *)0x40010800)) // Cấu hình PA0 - PA7
#define GPIOA_ODR   (*((volatile uint32_t *)0x4001080C)) // Xuất dữ liệu Port A

void delay(volatile uint32_t count) {
    while (count--) {
        __asm__("nop");
    }
}

int main(void) {
    // 1. Cấp clock cho Port A (Bit 2 trong RCC_APB2ENR)
    RCC_APB2ENR |= (1 << 2);

    // 2. Cấu hình PA0 đến PA7 là Output Push-Pull 2MHz
    // Thanh ghi CRL quản lý 8 chân, mỗi chân chiếm 4 bit.
    // 0x2 (0b0010) là cấu hình Output 2MHz. 
    // Ghi 8 số 2 liền nhau sẽ cấu hình đồng loạt 8 chân PA0->PA7.
    GPIOA_CRL = 0x22222222;

    int position = 0;   // Vị trí LED đang sáng (0 đến 7 tương ứng PA0 đến PA7)
    int direction = 1;  // 1: Chạy tới (trái sang phải), -1: Chạy lùi (phải sang trái)

    while (1) {
        // Xóa sạch trạng thái 8 chân PA0-PA7 (Tắt hết LED)
        GPIOA_ODR &= ~0xFF; 

        // Bật sáng LED ở vị trí hiện tại
        GPIOA_ODR |= (1 << position);

        delay(300000); // Tốc độ chạy của LED

        // Cập nhật vị trí tiếp theo
        position += direction;

        // Kiểm tra chạm biên để đảo chiều chạy
        if (position >= 7) {
            direction = -1; // Chạm mép phải -> quay đầu
        } else if (position <= 0) {
            direction = 1;  // Chạm mép trái -> quay đầu
        }
    }
    return 0;
}
EOF
