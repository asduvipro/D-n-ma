#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t control_reg; // Bit 0: Headlight, Bit 1: Motor L, Bit 2: Motor R
    uint8_t speed_left;
    uint8_t speed_right;
} SmartCar;

// TODO 1: Viết hàm giải mã chuỗi lệnh UART gửi xuống xe
void parse_uart_command(SmartCar *car, const char *cmd) {
    // 1. Dùng strcmp(cmd, "FORWARD") == 0 để xử lý lệnh tiến
    if (strcmp(cmd, "FORWARD")==0){
        car->speed_left = 180;
        car->speed_right = 180;
        car->control_reg |= (1<<1) | (1<<2);
    }
    // 2. Dùng strcmp(cmd, "BACKWARD") == 0 để xử lý lệnh lùi
    if (strcmp(cmd, "BACKWARD")==0){
        car->speed_left = 120;
        car->speed_right = 120;
        car->control_reg |= (1<<1) | (1<<2);
    }
    // 3. Dùng strcmp(cmd, "STOP") == 0 để xử lý lệnh dừng
    if (strcmp(cmd, "STOP")==0){
        car->speed_left = 0;
        car->speed_right = 0;
        car->control_reg &= ~((1<<1) | (1<<2));
    }
    // Cập nhật car->speed_left, car->speed_right và car->control_reg tương ứng
}

// TODO 2: Viết hàm quét Hàng 0 của ma trận cảm biến 3x3 tìm cột có giá trị cao nhất
uint8_t analyze_sensor_grid(uint16_t grid[3][3]) {
    uint8_t max_col = 0;
    uint16_t max_val = grid[0][0];

    // Viết vòng lặp for duyệt cột c từ 1 đến 2 ở hàng 0 (grid[0][c])
    // Nếu grid[0][c] > max_val -> Cập nhật max_val và max_col
    for (uint16_t i = 0; i<3;i++){
        if (grid[0][i] > max_val){
            max_val = grid[0][i];
            max_col = i;
            }
    }

    return max_col; // Trả về chỉ số cột (0: Trái, 1: Giữa, 2: Phải)
}

// Hàm in binary 8-bit
void print_binary(uint8_t val) {
    for (int8_t i = 7; i >= 0; i--) {
        printf("%d", (val >> i) & 1);
    }
}

int main(void) {
    SmartCar my_car = {0, 0, 0};

    printf("========== TEST BÀI 1: GIẢI MÃ CHUỖI UART ==========\n");
    parse_uart_command(&my_car, "FORWARD");
    printf("Lệnh FORWARD  -> Speed L: %3u | Speed R: %3u | Reg: ", my_car.speed_left, my_car.speed_right);
    print_binary(my_car.control_reg); printf("\n");

    parse_uart_command(&my_car, "BACKWARD");
    printf("Lệnh BACKWARD -> Speed L: %3u | Speed R: %3u | Reg: ", my_car.speed_left, my_car.speed_right);
    print_binary(my_car.control_reg); printf("\n");

    parse_uart_command(&my_car, "STOP");
    printf("Lệnh STOP     -> Speed L: %3u | Speed R: %3u | Reg: ", my_car.speed_left, my_car.speed_right);
    print_binary(my_car.control_reg); printf("\n\n");

    printf("========== TEST BÀI 2: MA TRẬN CẢM BIẾN 3x3 ==========\n");
    uint16_t sensor_grid[3][3] = {
        {150, 920, 210}, // Hàng 0: Cột 1 (Giữa) có giá trị cao nhất (920)
        {110, 780, 180},
        { 90, 600, 130}
    };

    uint8_t line_col = analyze_sensor_grid(sensor_grid);
    printf("Vạch đen đậm nhất phát hiện ở Hàng 0, Cột: %u ", line_col);
    if (line_col == 0) printf("(BÊN TRÁI)\n");
    else if (line_col == 1) printf("(Ở GIỮA)\n");
    else if (line_col == 2) printf("(BÊN PHẢI)\n");

    return 0;
}
