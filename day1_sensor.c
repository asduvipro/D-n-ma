#include <stdio.h>
#include <stdint.h>
int main(void) {
    // BƯỚC 1: Khai báo và gán giá trị cụ thể cho biến
    uint32_t runtime_ms = 12500;
    uint16_t adc_pin = 768;
    uint8_t pwm_speed = 200;

    // BƯỚC 2: Ép kiểu và tính toán
    float voltage = (float)adc_pin * 5.0f / 1023.0f;
    float speed_percent = (float)pwm_speed * 100.0f / 255.0f;

    // BƯỚC 3: In kết quả chuẩn định dạng
    printf("--- HE THONG XE TU HANH ---\n");
    printf("Thoi gian hoat dong : %lu ms\n", (unsigned long)runtime_ms);
    printf("Gia tri ADC pin     : %u\n", adc_pin);
    printf("Dien ap pin         : %.2f V\n", voltage);
    printf("Cong suat dong co   : %.2f %%\n", speed_percent);

    return 0;
}
