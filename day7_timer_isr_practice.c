#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// TODO 1: Khai báo 2 biến toàn cục dùng chung giữa ISR và main.
// - Biến 1: system_ticks (kiểu uint32_t, khởi tạo = 0) -> Đếm tổng số ms đã trôi qua.
// - Biến 2: flag_50ms (kiểu bool, khởi tạo = false)     -> Cờ báo sự kiện 50ms.
// GỢI Ý: Bắt buộc phải có từ khóa 'volatile' ở đầu khai báo!
// ============================================================================

/* TODO 1: VIẾT CODE KHAI BÁO CÁC BIẾN VOLATILE TẠI ĐÂY */
static volatile uint32_t system_ticks = 0;
static volatile bool     flag_50ms    = false;
// ============================================================================
// GIẢ LẬP TRÌNH XỬ LÝ NGẮT TIMER (Hàm này chạy tự động mỗi 1ms khi có ngắt)
// ============================================================================
void Hardware_Timer_ISR(void) {
    // TODO 2: Hoàn thiện logic bên trong hàm ngắt ISR:
    // a) Tăng biến system_ticks lên 1 đơn vị.
    // b) Kiểm tra nếu system_ticks chia hết cho 50 (system_ticks % 50 == 0):
    //    thì bật cờ flag_50ms = true.

    /* TODO 2: VIẾT LOGIC CHO ISR TẠI ĐÂY */
    system_ticks ++;
    if (system_ticks % 50 == 0){
        flag_50ms = true;
    }
}


// Cấu trúc mô phỏng xe
typedef struct {
    uint8_t speed_left;
    uint8_t speed_right;
} SmartCar;


int main(void) {
    SmartCar my_car = {180, 180};
    uint32_t last_uart_time = 0;

    printf("========== BÀI 7: THỰC HÀNH TIMER TICK & NON-BLOCKING ISR ==========\n\n");

    // Giả lập vòng lặp main() chạy liên tục 300 bước (tương đương 300ms)
    for (int step = 0; step < 300; step++) {
        // Giả lập xung nhịp phần cứng ngắt Timer xuất hiện mỗi vòng lặp (1ms)
        Hardware_Timer_ISR(); 
        // -------------------------------------------------------------------
        // TÁC VỤ A: Đọc cảm biến định kỳ mỗi 50ms dựa vào CỜ NGẮT (ISR Flag)
        // -------------------------------------------------------------------
        // TODO 3: Kiểm tra nếu flag_50ms đang là true:
        // - Xóa cờ ngay lập tức (gán flag_50ms = false).
        // - In ra màn hình thông báo: "[ISR FLAG @ %4ums] Doc cam bien IR dieu chinh goc lai!\n"
        /* TODO 3: VIẾT LOGIC XỬ LÝ CỜ NGẮT TẠI ĐÂY */
        if (flag_50ms == true){
            flag_50ms = false;
            printf("[ISR FLAG @ %4ums] Doc cam bien IR dieu chinh goc lai!\n",system_ticks);
        }
        // -------------------------------------------------------------------
        // TÁC VỤ B: Đẩy dữ liệu UART định kỳ mỗi 100ms dựa vào HIỆU TICK (Non-blocking)
        // -------------------------------------------------------------------
        // TODO 4: Kiểm tra nếu (system_ticks - last_uart_time >= 100):
        // - Cập nhật last_uart_time = system_ticks.
        // - In ra màn hình: "   ---> [MAIN UART @ %4ums] Gui telemetry: Speed L=180, R=180\n"
        /* TODO 4: VIẾT LOGIC TICK DIFFERENCE TẠI ĐÂY */
        if (system_ticks - last_uart_time >= 100){
            last_uart_time = system_ticks;
            printf ("   ---> [MAIN UART @ %4ums] Gui telemetry: Speed L=180, R=180\n",system_ticks);
        }
    }

    printf("\n=> Hoan thanh mo phong 300ms! Tong so ngat ISR da chay: %u ticks.\n", system_ticks);
    return 0;
}
