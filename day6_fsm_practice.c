#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t control_reg; // Bit 0: LED, Bit 1: Motor L, Bit 2: Motor R
    uint8_t speed_left;
    uint8_t speed_right;
} SmartCar;

// 1. Định nghĩa các trạng thái của hệ thống
typedef enum {
    STATE_IDLE = 0,
    STATE_LINE_FOLLOW,
    STATE_OBSTACLE_AVOID,
    STATE_EMERGENCY_STOP,
    STATE_MAX_STATES // Đánh dấu số lượng trạng thái (bằng 4)
} SystemState;

// 2. Các hàm xử lý hành vi cụ thể từng trạng thái
void handle_idle(SmartCar *car) {
    car->speed_left = 0;
    car->speed_right = 0;
    car->control_reg &= ~(0b11 << 1); // Tắt 2 động cơ
    printf("[FSM LOGIC] Dang o trang thai CHO (IDLE)... Dong co TAT.\n");
}

void handle_line_follow(SmartCar *car) {
    car->speed_left = 160;
    car->speed_right = 160;
    car->control_reg |= (0b11 << 1);  // Bật 2 động cơ
    printf("[FSM LOGIC] Dang DO DUONG (LINE FOLLOW)... Toc do: 160.\n");
}

void handle_obstacle_avoid(SmartCar *car) {
    car->speed_left = 80;
    car->speed_right = 180; // Bẻ lái quay xe
    car->control_reg |= (0b11 << 1);
    printf("[FSM LOGIC] NE VAT CAN (OBSTACLE AVOID)... Dang be lai!\n");
}

void handle_emergency_stop(SmartCar *car) {
    car->speed_left = 0;
    car->speed_right = 0;
    car->control_reg = 0; // Tắt toàn bộ thiết bị và LED
    printf("[FSM LOGIC] !!! DUNG KHAN CAP (EMERGENCY STOP) !!!\n");
}

// TODO 1: 
// A. Khai báo typedef cho con trỏ hàm tên là StateFunc nhận tham số (SmartCar *)
// B. Khai báo mảng con trỏ hàm state_table chứa 4 hàm xử lý ở trên theo đúng thứ tự enum

typedef void (*StateFunc)(SmartCar *car);

StateFunc state_table[STATE_MAX_STATES] = {
    // Điền 4 tên hàm tương ứng vào đây
    [STATE_IDLE] = handle_idle,
    [STATE_LINE_FOLLOW] = handle_line_follow,
    [STATE_OBSTACLE_AVOID] = handle_obstacle_avoid,
    [STATE_EMERGENCY_STOP] = handle_emergency_stop
};

// TODO 2: Viết hàm thực thi trạng thái FSM hiện tại thông qua mảng con trỏ hàm
void run_fsm(SmartCar *car, SystemState current_state) {
    // Kiểm tra chỉ số state hợp lệ (nhỏ hơn STATE_MAX_STATES)
    // Thực thi hàm tương ứng trong mảng state_table
    if (current_state<STATE_MAX_STATES){
        state_table[current_state](car);
    }
}

// TODO 3: Giải mã chuỗi lệnh UART để trả về kiểu enum SystemState
SystemState parse_uart_to_state(const char *cmd) {
    // Dùng strcmp để so sánh cmd với:
    // "IDLE"      -> Trả về STATE_IDLE
    // "RUN_LINE"  -> Trả về STATE_LINE_FOLLOW
    // "AVOID"     -> Trả về STATE_OBSTACLE_AVOID
    // "EMERGENCY" -> Trả về STATE_EMERGENCY_STOP
    // Mặc định trả về STATE_IDLE nếu chuỗi không hợp lệ
    if (strcmp(cmd, "IDLE") == 0) {
        return STATE_IDLE;
    }
    if (strcmp(cmd, "RUN_LINE") == 0) {
        return STATE_LINE_FOLLOW;
    }
    if (strcmp(cmd, "AVOID") == 0) {
        return STATE_OBSTACLE_AVOID;
    }
    if (strcmp(cmd, "EMERGENCY") == 0) {
        return STATE_EMERGENCY_STOP;
    }
    return STATE_IDLE;
}

int main(void) {
    SmartCar my_car = {0, 0, 0};
    SystemState current_state = STATE_IDLE;

    printf("========== TEST NGUYÊN LÝ FSM & CON TRỎ HÀM ==========\n\n");

    // Giả lập chuỗi lệnh UART gửi tới liên tục
    const char *commands[] = {"RUN_LINE", "AVOID", "EMERGENCY", "IDLE"};

    for (uint8_t i = 0; i < 4; i++) {
        printf("---> Nhan lenh UART: \"%s\"\n", commands[i]);

        // 1. Chuyển chuỗi UART thành Enum Trạng thái
        current_state = parse_uart_to_state(commands[i]);

        // 2. Thực thi FSM dựa trên mảng con trỏ hàm
        run_fsm(&my_car, current_state);

        printf("Trang thai xe -> Speed L: %3u | Speed R: %3u | Reg: 0x%02X\n\n",
               my_car.speed_left, my_car.speed_right, my_car.control_reg);
    }

    return 0;
}
