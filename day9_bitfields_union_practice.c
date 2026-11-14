#include <stdio.h>
#include <stdint.h>

// ============================================================================
// TODO 1: Định nghĩa cấu trúc Bit-fields 8-bit cho thanh ghi động cơ (MotorBits_t)
// Thứ tự từ Bit 0 (phải) lên Bit 7 (trái):
// - EN       : 1 bit (Bit 0: Bật/tắt động cơ)
// - DIR      : 1 bit (Bit 1: Chiều quay 0=Tiến, 1=Lùi)
// - BRAKE    : 1 bit (Bit 2: Phanh khẩn cấp)
// - GEAR     : 2 bit (Bit 3..4: Cấp số tốc độ 0..3)
// - RESERVED : 3 bit (Bit 5..7: Dự phòng cho đủ 8 bit)
// ============================================================================
typedef struct {
    /* VIẾT CODE CỦA BẠN TẠI ĐÂY */
    uint8_t EN : 1;
    uint8_t DIR : 1;
    uint8_t BRAKE : 1;
    uint8_t GEAR : 2;
    uint8_t RESERVED : 3;
} MotorBits_t;

// Hợp nhất 1 byte thanh ghi động cơ: Vừa nhìn cả byte (.all), vừa nhìn từng bit (.bits)
typedef union {
    uint8_t     all;
    MotorBits_t bits;
} MotorReg_t;

// ============================================================================
// TODO 2: Viết hàm cấu hình thanh ghi động cơ
// 1. Xóa sạch toàn bộ thanh ghi về 0 thông qua trường .all
// 2. Bật EN = 1, đặt DIR = 0 (tiến), BRAKE = 0 (nhả phanh), GEAR = 3 (số tối đa)
//    thông qua trường .bits
// ============================================================================
void Motor_SetFullForward(MotorReg_t *reg) {
    /* VIẾT CODE CỦA BẠN TẠI ĐÂY */
    reg->all = 0x00;
    reg->bits.EN = 1;
    reg->bits.DIR = 0;
    reg->bits.BRAKE = 0;
    reg->bits.GEAR = 3;
}

// ============================================================================
// TODO 3: Định nghĩa Union gói tin điều khiển xe 6 byte (CarPacket_t)
// Chồng 2 góc nhìn lên cùng 6 byte RAM:
// 1. Một struct có thuộc tính __attribute__((packed)) tên là 'field' gồm:
//    - uint8_t    header;       (Byte 0)
//    - MotorReg_t ctrl;         (Byte 1)
//    - int16_t    left_speed;   (Byte 2..3)
//    - int16_t    right_speed;  (Byte 4..5)
// 2. Mảng 'uint8_t raw[6];'
// ============================================================================
typedef union {
    /* VIẾT CODE CỦA BẠN TẠI ĐÂY */
    struct __attribute__((packed)){uint8_t header;
    MotorReg_t ctrl;
    int16_t left_speed;
    int16_t right_speed;}field;
    uint8_t raw[6];
} CarPacket_t;

// ============================================================================
// TODO 4: Giải nén gói tin nhận được từ mảng byte thô
// Sao chép 6 byte từ mảng 'incoming_bytes' vào 'rx_pkt->raw' bằng vòng lặp for
// ============================================================================
void Packet_Unpack(CarPacket_t *rx_pkt, const uint8_t incoming_bytes[6]) {
    /* VIẾT CODE CỦA BẠN TẠI ĐÂY */
    for (uint8_t i=0; i<6; i++){
        rx_pkt->raw[i] = incoming_bytes[i];
    }
}

/* ============================================================================
 * PHẦN KIỂM THỬ HỆ THỐNG (KHÔNG CẦN SỬA)
 * ============================================================================ */
int main(void) {
    // --- PHẦN 1: KIỂM THỬ THANH GHI ĐỘNG CƠ 8-BIT ---
    MotorReg_t motor_reg;
    Motor_SetFullForward(&motor_reg);

    printf("=== 1. KIEM THU THANH GHI DONG CO (8-BIT) ===\n");
    printf("Kich thuoc MotorReg_t: %u byte\n", (unsigned)sizeof(MotorReg_t));
    printf("Gia tri ca thanh ghi (motor_reg.all): 0x%02X (Thap phan: %u - Ky vong: 0x19 / 25)\n",
           motor_reg.all, motor_reg.all);
    printf("Trang thai tung cong tac: EN=%u, DIR=%u, BRAKE=%u, GEAR=%u\n\n",
           motor_reg.bits.EN, motor_reg.bits.DIR,
           motor_reg.bits.BRAKE, motor_reg.bits.GEAR);

    // --- PHẦN 2: KIỂM THỬ CHẶT BIẾN LỚN THÀNH TỪNG BYTE GỬI QUA UART ---
    printf("=== 2. KIEM THU GOI TIN TRUYEN THONG (6 BYTE) ===\n");
    printf("Kich thuoc CarPacket_t: %u byte (Ky vong: 6 byte)\n",
           (unsigned)sizeof(CarPacket_t));

    // Phía bộ điều khiển: Gán giá trị tự nhiên bằng tên biến
    CarPacket_t tx_pkt = {0};
    tx_pkt.field.header      = 0xAA;
    tx_pkt.field.ctrl        = motor_reg; // Nhét nguyên thanh ghi động cơ vừa cấu hình ở trên
    tx_pkt.field.left_speed  = 250;       // Bánh trái tiến tốc độ 250
    tx_pkt.field.right_speed = -100;      // Bánh phải lùi tốc độ -100 (quay gắt)

    printf("6 byte tho (tx_pkt.raw) chay qua ong UART: ");
    for (int i = 0; i < 6; i++) {
        printf("0x%02X ", tx_pkt.raw[i]);
    }
    printf("\n");

    // Phía xe tự hành: Hứng 6 byte thô và giải nén ngược lại thành biến có nghĩa
    CarPacket_t rx_pkt = {0};
    Packet_Unpack(&rx_pkt, tx_pkt.raw);

    printf("===> [XE TU HANH GIAI MA DUOC]:\n");
    printf("     Header = 0x%02X | Motor EN = %u | Gear = %u\n",
           rx_pkt.field.header, rx_pkt.field.ctrl.bits.EN, rx_pkt.field.ctrl.bits.GEAR);
    printf("     Toc do banh trai = %d | Toc do banh phai = %d\n",
           rx_pkt.field.left_speed, rx_pkt.field.right_speed);

    return 0;
}
