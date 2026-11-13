#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define RING_BUFFER_SIZE  16
#define RING_BUFFER_MASK  (RING_BUFFER_SIZE - 1)

typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE]; // Mảng lưu dữ liệu trong SRAM
    volatile uint8_t head;            // Chỉ số ngăn sẽ GHI tiếp theo (0..15)
    volatile uint8_t tail;            // Chỉ số ngăn sẽ ĐỌC tiếp theo (0..15)
} RingBuffer_t;

static RingBuffer_t uart_rx_rb;

// Giả lập thanh ghi nhận dữ liệu 1 byte duy nhất của phần cứng UART
static volatile uint8_t SIMULATED_UART_RDR = 0;

void RingBuffer_Init(RingBuffer_t *rb) {
    rb->head = 0;
    rb->tail = 0;
    memset(rb->buffer, 0, RING_BUFFER_SIZE);
}

// ============================================================================
// TODO 1: Kiểm tra bộ đệm RỖNG (Empty)
// Trả về true nếu chỉ số head bằng chỉ số tail, ngược lại trả về false.
// ============================================================================
bool RingBuffer_IsEmpty(const RingBuffer_t *rb) {
    /* VIẾT CODE CỦA BẠN TẠI ĐÂY */
    if (rb->head == rb->tail) {
        return true;
    } else {
        return false;
    }

}

// ============================================================================
// TODO 2: Kiểm tra bộ đệm ĐẦY (Full)
// Tính bước tiếp theo của head bằng Bitwise Mask: (rb->head + 1) & RING_BUFFER_MASK
// Trả về true nếu bước tiếp theo đó trùng với tail, ngược lại trả về false.
// ============================================================================
bool RingBuffer_IsFull(const RingBuffer_t *rb) {
    /* VIẾT CODE CỦA BẠN TẠI ĐÂY */
    if (((rb->head + 1) & RING_BUFFER_MASK) == rb->tail) {
        return true;}
    else {
        return false;
    }
}

// ============================================================================
// TODO 3: Đẩy 1 byte vào Ring Buffer (Producer - Gọi trong ngắt UART ISR)
// 1. Gọi RingBuffer_IsFull(rb). Nếu đầy -> trả về false (báo tràn đệm).
// 2. Ghi data vào mảng tại vị trí head: rb->buffer[rb->head] = data;
// 3. Cập nhật head quay vòng: rb->head = (rb->head + 1) & RING_BUFFER_MASK;
// 4. Trả về true.
// ============================================================================
bool RingBuffer_Put(RingBuffer_t *rb, uint8_t data) {
    /* VIẾT CODE CỦA BẠN TẠI ĐÂY */
    if (RingBuffer_IsFull(rb)){
        return false;
    } 
    else {
        rb->buffer[rb->head] = data;
        rb->head = (rb->head + 1) & RING_BUFFER_MASK;
        return true;
    }
}

// ============================================================================
// TODO 4: Lấy 1 byte ra khỏi Ring Buffer (Consumer - Gọi trong Main Loop)
// 1. Gọi RingBuffer_IsEmpty(rb). Nếu rỗng -> trả về false (báo hết dữ liệu).
// 2. Lấy dữ liệu tại vị trí tail gán vào biến đích: *p_data = rb->buffer[rb->tail];
// 3. Cập nhật tail quay vòng: rb->tail = (rb->tail + 1) & RING_BUFFER_MASK;
// 4. Trả về true.
// ============================================================================
bool RingBuffer_Get(RingBuffer_t *rb, uint8_t *p_data) {
    /* VIẾT CODE CỦA BẠN TẠI ĐÂY */
    if (RingBuffer_IsEmpty(rb)){
        return false;
    }
    else{
        *p_data = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) & RING_BUFFER_MASK;
        return true;
    }
}

/* ============================================================================
 * PHẦN MÔ PHỎNG PHẦN CỨNG & GIẢI MÃ GÓI TIN (KHÔNG CẦN SỬA)
 * ============================================================================ */

// Trình phục vụ ngắt UART (ISR): Bốc 1 byte từ thanh ghi RDR cất vào SRAM
void USART_RX_IRQHandler(void) {
    uint8_t received_byte = SIMULATED_UART_RDR;

    if (!RingBuffer_Put(&uart_rx_rb, received_byte)) {
        printf("  [ISR -> LOI OVERRUN] Bo dem day! Mat byte '%c'\n", received_byte);
    } else {
        printf("  [ISR] Doc RDR='%c' -> Cat vao SRAM (head=%2d, tail=%2d)\n",
               received_byte, uart_rx_rb.head, uart_rx_rb.tail);
    }
}

// Giả lập phần cứng nhận chuỗi byte nối tiếp qua thanh ghi RDR 1 byte
void Hardware_UART_Receive_Stream(const char *stream) {
    printf("[PHAN CUNG UART] Nhan chuoi \"%s\" qua thanh ghi RDR:\n", stream);
    for (int i = 0; stream[i] != '\0'; i++) {
        SIMULATED_UART_RDR = (uint8_t)stream[i];
        USART_RX_IRQHandler();
    }
}

// Vòng lặp chính rút dữ liệu từ SRAM, bẫy ngoặc gói tin '[' ']' và điều phối lệnh
void Main_Loop_Process(void) {
    static char packet[32];
    static uint8_t idx = 0;
    static bool in_packet = false;
    uint8_t byte_out = 0;

    printf("\n[MAIN LOOP] Rut tung byte tu SRAM de giai ma:\n");
    while (RingBuffer_Get(&uart_rx_rb, &byte_out)) {
        if (byte_out == '[') {
            in_packet = true;
            idx = 0;
        } else if (byte_out == ']') {
            if (in_packet) {
                packet[idx] = '\0';
                in_packet = false;
                printf("  ===> [FSM NHAN LENH]: \"%s\"\n", packet);
                if (strcmp(packet, "RUN_LINE") == 0) {
                    printf("       -> Cap nhat PWM: Xe bat dau do line!\n");
                } else if (strcmp(packet, "STOP") == 0) {
                    printf("       -> Cap nhat PWM = 0: Phanh khan cap!\n");
                }
            }
        } else if (in_packet && idx < sizeof(packet) - 1) {
            packet[idx++] = (char)byte_out;
        }
    }
}

int main(void) {
    RingBuffer_Init(&uart_rx_rb);

    // Kịch bản 1: Nhận gói tin "[RUN_LINE]" (10 byte)
    Hardware_UART_Receive_Stream("[RUN_LINE]");
    Main_Loop_Process();

    printf("\n------------------------------------------------------------\n");

    // Kịch bản 2: Nhận gói tin "[STOP]" (6 byte) -> Quan sát head chạm 15 rồi quay về 0
    Hardware_UART_Receive_Stream("[STOP]");
    Main_Loop_Process();

    printf("\n------------------------------------------------------------\n");

    // Kịch bản 3: Ngoại vi gửi dồn 18 byte (> 15 byte khả dụng) khi main() chưa kịp đọc
    Hardware_UART_Receive_Stream("[SPEED_MAX_123456]");
    Main_Loop_Process();

    return 0;
}
