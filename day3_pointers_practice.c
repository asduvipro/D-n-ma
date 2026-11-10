#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // Để dùng hàm abs() lấy giá trị tuyệt đối nếu cần

// TODO 1: Cài đặt hàm swap_sensor_data ở đây
void swap_sensor_data(uint16_t *sensor_a, uint16_t *sensor_b) {
    // ... Viết logic swap dùng biến trung gian temp tại đây ...
    uint16_t temp = *sensor_a;
    *sensor_a = *sensor_b;
    *sensor_b = temp;
}

// TODO 2: Cài đặt hàm calculate_steering ở đây
void calculate_steering(uint8_t *left_speed, uint8_t *right_speed, int8_t error) {
    uint8_t base_speed = 150;
    int16_t temp_left = base_speed;  // Dùng int16_t để tính toán tránh tràn số trước khi ép về uint8_t
    int16_t temp_right = base_speed;

    // ... Tính toán temp_left và temp_right dựa theo biến error ... 
    // ... Làm tương tự với temp_right ...
    if (error == 0){
        temp_left = 150;
        temp_right = 150;
    }
    else if (error<0){
        temp_left -= abs(error)*10;
        temp_right += abs(error)*10;
    }
    else if (error>0){
        temp_left += abs(error)*10;
        temp_right -= abs(error)*10;
    }
    // ... Chống tràn: Nếu temp_left > 255 -> temp_left = 255; nếu < 0 -> temp_left = 0 ...
   if (temp_left>255){
    temp_left = 255;
    }
   else if (temp_left<0){
    temp_left = 0;
    }
   if (temp_right>255){
    temp_right = 255;
    }
   else if (temp_right<0){
    temp_right = 0;
    }    
    // ... Gán giá trị sau khi xử lý vào con trỏ: *left_speed = (uint8_t)temp_left ...
    *left_speed = (uint8_t)temp_left;
    *right_speed = (uint8_t)temp_right;
}

int main(void) {
    // Kiêm tra Bài tập 1
    uint16_t sen1 = 1200, sen2 = 4500;
    printf("--- BAI TAP 1: SWAP ---\n");
    printf("Truoc khi swap: Sen1 = %u, Sen2 = %u\n", sen1, sen2);

    // TODO: Gọi hàm swap_sensor_data truyền địa chỉ sen1, sen2
    // ...
    swap_sensor_data(&sen1,&sen2);
    printf("Sau khi swap   : Sen1 = %u, Sen2 = %u\n\n", sen1, sen2);

    // Kiểm tra Bài tập 2
    uint8_t motor_l = 0, motor_r = 0;
    printf("--- BAI TAP 2: DIEU KHIEN XE ---\n");

    // Test case 1: Lệch trái nặng (error = -5)
    calculate_steering(&motor_l, &motor_r, -5);
    printf("Test error = -5 (Re phai) -> Left: %u | Right: %u\n", motor_l, motor_r);

    // Test case 2: Xe đi thẳng (error = 0)
    calculate_steering(&motor_l, &motor_r, 0);
    printf("Test error =  0 (Di thang)-> Left: %u | Right: %u\n", motor_l, motor_r);

    return 0;
}
