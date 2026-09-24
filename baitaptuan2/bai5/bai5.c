#include <stdint.h>

// ====================================================================
// 1. BẢNG VECTOR NGẮT (Chỉ định ngắt USART1 ở vị trí số 53)
// ====================================================================
#define SRAM_END 0x20005000UL
extern int main(void);
void USART1_IRQHandler(void);
void Reset_Handler(void) { main(); while(1); }

typedef void (*isr_t)(void);
__attribute__((used, section(".vectors")))
isr_t const vector_table[54] = {
    (isr_t)SRAM_END,
    Reset_Handler,
    [53] = USART1_IRQHandler // Đăng ký hàm ngắt cho USART1 (IRQ 37)
};

// ====================================================================
// 2. ĐỊA CHỈ THANH GHI
// ====================================================================
#define RCC_BASE        0x40021000UL
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x1C))

#define GPIOA_BASE      0x40010800UL
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_CRH       (*(volatile uint32_t *)(GPIOA_BASE + 0x04))

#define TIM2_BASE       0x40000000UL
#define TIM2_CR1        (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM2_EGR        (*(volatile uint32_t *)(TIM2_BASE + 0x14))
#define TIM2_CCMR1      (*(volatile uint32_t *)(TIM2_BASE + 0x18))
#define TIM2_CCER       (*(volatile uint32_t *)(TIM2_BASE + 0x20))
#define TIM2_PSC        (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM2_ARR        (*(volatile uint32_t *)(TIM2_BASE + 0x2C))
#define TIM2_CCR1       (*(volatile uint32_t *)(TIM2_BASE + 0x34))

#define USART1_BASE     0x40013800UL
#define USART1_SR       (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_DR       (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_BRR      (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_CR1      (*(volatile uint32_t *)(USART1_BASE + 0x0C))

#define NVIC_ISER1      (*(volatile uint32_t *)0xE000E104UL)

// ====================================================================
// 3. BIẾN TOÀN CỤC & LOGIC ĐIỀU KHIỂN
// ====================================================================
volatile uint8_t current_pwm = 50;  // Lưu cấu hình PWM (mặc định 50%)
volatile uint8_t is_on = 0;         // Trạng thái bật/tắt (0 = OFF, 1 = ON)
char rx_buf[32];                    // Bộ đệm chứa chuỗi gửi từ PC
volatile uint8_t rx_idx = 0;

// Các hàm gửi phản hồi qua UART
void uart_send_char(char c) {
    while (!(USART1_SR & (1U << 7))); 
    USART1_DR = c;
}
void uart_send_string(const char *str) {
    while (*str) uart_send_char(*str++);
}
void uart_send_num(uint8_t num) {
    if (num >= 100) uart_send_char('0' + (num / 100));
    if (num >= 10)  uart_send_char('0' + ((num / 10) % 10));
    uart_send_char('0' + (num % 10));
}

// Cập nhật độ sáng thực tế
void update_led(void) {
    if (is_on) TIM2_CCR1 = current_pwm * 10; 
    else TIM2_CCR1 = 0;
}

// Hàm so sánh chuỗi (tự viết để tránh lỗi thiếu thư viện string.h)
int custom_strncmp(const char *s1, const char *s2, int n) {
    while (n-- > 0) {
        if (*s1 != *s2) return *s1 - *s2;
        if (*s1 == '\0') return 0;
        s1++; s2++;
    }
    return 0;
}

// Xử lý lệnh khi nhận đủ ký tự '!'
void process_cmd(void) {
    rx_buf[rx_idx] = '\0'; 
    
    if (custom_strncmp(rx_buf, "ON", 2) == 0) {
        is_on = 1;
        update_led();
        uart_send_string(">> Da bat LED\r\n");
    } 
    else if (custom_strncmp(rx_buf, "OFF", 3) == 0) {
        is_on = 0;
        update_led();
        uart_send_string(">> Da tat LED\r\n");
    } 
    else if (custom_strncmp(rx_buf, "PWM:", 4) == 0) {
        uint8_t val = 0;
        for(int i=4; rx_buf[i] != '%' && rx_buf[i] != '\0'; i++) {
            if (rx_buf[i] >= '0' && rx_buf[i] <= '9') {
                val = val * 10 + (rx_buf[i] - '0');
            }
        }
        if (val > 100) val = 100;
        current_pwm = val; 
        update_led(); 
        uart_send_string(">> Da luu PWM moi\r\n");
    } 
    else if (custom_strncmp(rx_buf, "Status", 6) == 0) {
        uart_send_string(">> Trang thai: ");
        uart_send_string(is_on ? "ON, " : "OFF, ");
        uart_send_string("PWM = ");
        uart_send_num(current_pwm);
        uart_send_string("%\r\n");
    }
    rx_idx = 0; 
}

// ====================================================================
// 4. TRÌNH PHỤC VỤ NGẮT & HÀM MAIN
// ====================================================================
void USART1_IRQHandler(void) {
    if (USART1_SR & (1U << 5)) { 
        char c = USART1_DR;
        if (c == '!') {
            process_cmd(); // Xử lý khi gặp dấu chấm than
        } else if (c != '\r' && c != '\n') { 
            if (rx_idx < 30) rx_buf[rx_idx++] = c; // Đẩy vào bộ đệm
        }
    }
}

int main(void) {
    // 1. Cấp xung nhịp cho GPIOA, USART1, TIM2
    RCC_APB2ENR |= (1U << 2) | (1U << 14) | (1U << 0); 
    RCC_APB1ENR |= (1U << 0);

    // 2. Cấu hình PA0 (PWM TIM2_CH1) 
    GPIOA_CRL &= ~(0xFU << 0);
    GPIOA_CRL |=  (0xBU << 0);

    // 3. Cấu hình UART: PA9 (TX) Output Alt Push-Pull, PA10 (RX) Input Floating
    GPIOA_CRH &= ~(0xFFU << 4);
    GPIOA_CRH |=  (0x4BU << 4); 

    // 4. Cấu hình TIM2 (1kHz)
    TIM2_PSC = 7;
    TIM2_ARR = 999;
    TIM2_CCMR1 |= (6U << 4) | (1U << 3); 
    TIM2_CCER  |= (1U << 0); 
    TIM2_CR1   |= (1U << 7) | (1U << 0); 

    // 5. Cấu hình UART (Tốc độ 9600 bps dựa trên xung nhịp mặc định 8MHz)
    USART1_BRR = 833; 
    USART1_CR1 = (1U << 13) | (1U << 5) | (1U << 3) | (1U << 2); // Bật ngắt RXNE

    // 6. Cho phép ngắt USART1 trên hệ thống NVIC
    NVIC_ISER1 |= (1U << 5); // Bit 5 tương ứng với ngắt số 37 (USART1)

    // Khởi tạo trạng thái ban đầu
    update_led();

    while (1) {
        // Vòng lặp rỗng. Tất cả công việc sẽ do ngắt UART xử lý!
    }
    return 0;
}