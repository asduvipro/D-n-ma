#include <stdio.h>
#include <stdint.h>

int main(void) {
    uint8_t vehicle_control = 0; // Khởi tạo thanh ghi bằng 0
    uint16_t line_sensors[5] = {150, 200, 890, 220, 180};

    // BƯỚC 1: Dùng phép toán bitwise bật Bit 0 (LED), Bit 1 (Động cơ trái), Bit 2 (Động cơ phải)
    // Gợi ý: vehicle_control |= ...
    vehicle_control |= (0b111);
    vehicle_control &= ~(1<<3);

    // BƯỚC 2: Duyệt mảng line_sensors, in giá trị từng cảm biến và đếm số cảm biến trên vạch đen (> 500)
    uint8_t count;
    for (uint8_t i=0; i<5; i++) {
        if (line_sensors[i] > 500){
            count += 1;}
            printf("Giá trị của cảm biến %u là %u\n",(unsigned int)i,(unsigned int)line_sensors[i]);
    }
    printf("Số giá tri cảm biến phát hiện vạch đen là %u\n",(unsigned int)count);

    // BƯỚC 3: Dùng toán tử bitwise kiểm tra xem Bit 1 và Bit 2 có đang bật không, nếu có thì in "Trang thai: Xe dang di thang"
    if (vehicle_control & (0b11<<1)){
        printf("Trang thai: xe dang di thang\n");
    }

    // BƯỚC 4: Dùng phép toán bitwise tắt Bit 1 và Bit 2 (giữ nguyên Bit 0)
    // In giá trị số nguyên của vehicle_control ra màn hình
    vehicle_control &= ~(0b11<<1);
    printf("Nhi phan vehicle_control: ");
    for (int8_t i=7;i>=0;i--){
        printf("%d",(vehicle_control >> i)&1);
    }

    return 0;
}
