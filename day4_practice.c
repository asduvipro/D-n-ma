#include <stdio.h>
#include <stdint.h>

// ==================== BÀI TẬP 1 ====================
typedef struct {
    uint8_t status_reg; // Bit 0: LED nguon, Bit 1: Buzzer, Bit 2: Pin yeu
    uint16_t battery_mv;
} SystemStatus;

// TODO 1.1: Viet ham cap nhat pin va canh bao
void update_battery(SystemStatus *sys, uint16_t mv) {
    // Viết code của bạn ở đây:
    // 1. Gán mv vào sys->battery_mv
    // 2. Kiểm tra nếu mv < 3300: Bật Bit 2 và Bit 1
    // 3. Ngược lại (mv >= 3300): Tắt Bit 2 và Bit 1
    sys->battery_mv = mv;
    if (mv<3300){
        sys->status_reg |= (1<<2) | (1<<1);
    }
    else{
        sys->status_reg &= ~(1<<2) | ~(1<<1);
    }
}

// TODO 1.2: Viet ham dao trang thai LED nguon (Bit 0)
void toggle_power_led(SystemStatus *sys) {
    // Viết code đảo Bit 0 ở đây dùng toán tử ^= và ->
    sys->status_reg ^= (1<<0);
}


// ==================== BÀI TẬP 2 ====================
typedef struct {
    uint8_t control_reg; // Bit 0: Headlight, Bit 1: Motor L, Bit 2: Motor R
    uint8_t speed_left;
    uint8_t speed_right;
    uint16_t sensors[3]; // 3 cam bien line: Trai, Giua, Phai
} SmartCar;

// TODO 2.1: Viet ham bat/tat den pha (Bit 0)
void set_headlight(SmartCar *car, uint8_t state) {
    // Viết code ở đây (dùng |= để bật khi state=1, dùng &= ~ để tắt khi state=0)
    if (state=1){
        car->control_reg |= (1<<0);
    }
    else if (state=0){
        car->control_reg &= ~(1<<0);
    }
}

// TODO 2.2: Viet ham dieu khien huong di va dong co
void process_steering(SmartCar *car, int8_t error) {
    // Viết code điều khiển tốc độ và bật/tắt Bit 1, Bit 2 ở đây
    if (error==0){
        car->speed_left = 150;
        car->speed_right = 150;
        car->control_reg |= (1<<1) | (1<<2);
    }
    else if (error<0){
        car->speed_left = 100;
        car->speed_right = 200;
        car->control_reg |= (1<<1) | (1<<2);
    }
    else if (0<error && error<99){
        car->speed_left = 200;
        car->speed_right = 100;
        car->control_reg |= (1<<1) | (1<<2);
    }
    else if (error==99){
        car->speed_left = 0;
        car->speed_right = 0;
        car->control_reg &= ~(1<<1) | ~(1<<2);
    }

}

// Hàm hỗ trợ in chuỗi nhị phân 8-bit
void print_binary(uint8_t val) {
    for (int8_t i = 7; i >= 0; i--) {
        printf("%d", (val >> i) & 1);
    }
}

// TODO 2.3: Viet ham in trang thai xe
void print_car_status(const SmartCar *car) {
    printf("Speed Left: %3u | Speed Right: %3u | Control Reg: ", car->speed_left, car->speed_right);
    print_binary(car->control_reg);
    printf("\n");
}


// ==================== MAIN TEST ====================
int main(void) {
    printf("========== TEST BAI TAP 1: SYSTEM DIAGNOSTIC ==========\n");
    SystemStatus sys = {0, 4000}; // Pin ban dau 4000mV (4.0V)

    toggle_power_led(&sys);
    printf("Sau khi bat LED nguon (Status Reg): ");
    print_binary(sys.status_reg);
    printf("\n");

    printf("Kiem tra pin sụt xuong 3100mV (Pin yeu):\n");
    update_battery(&sys, 3100);
    printf("Battery: %umV | Status Reg: ", sys.battery_mv);
    print_binary(sys.status_reg);
    printf("\n\n");


    printf("========== TEST BAI TAP 2: SMARTCAR CONTROLLER ==========\n");
    SmartCar my_car = {0, 0, 0, {200, 800, 150}};

    // 1. Bật đèn pha
    set_headlight(&my_car, 1);
    printf("1. Bat den pha       -> ");
    print_car_status(&my_car);

    // 2. Xe đi thẳng
    process_steering(&my_car, 0);
    printf("2. Xe di thang       -> ");
    print_car_status(&my_car);

    // 3. Xe lệch trái (rẽ phải)
    process_steering(&my_car, -2);
    printf("3. Lech trai (Re P)  -> ");
    print_car_status(&my_car);

    // 4. Dừng khẩn cấp
    process_steering(&my_car, 99);
    printf("4. Dung khan cap     -> ");
    print_car_status(&my_car);

    return 0;
}
