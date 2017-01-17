//------------------------------------------------------------------------------
//  mos6502_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/devices/mos6502.h"
#include "yakc/core/system_bus.h"

using namespace YAKC;

static ubyte ram0[0x10000];
typedef mos6502 f;

static mos6502 init_cpu() {
    mos6502 cpu;
    memset(ram0, 0, sizeof(ram0));
    cpu.mem.map(0, 0x0000, sizeof(ram0), ram0, true);
    // write start address
    cpu.mem.w16(0xFFFC, 0x0200);
    cpu.init();
    cpu.reset();
    return cpu;
}

// test expected flags
static bool tf(const mos6502& cpu, uint8_t mask) {
    // X is always set
    mask |= f::XF;
    return (cpu.P & mask) == mask;
}

// CPU init
TEST(init) {
    auto cpu = init_cpu();
    CHECK(0 == cpu.A);
    CHECK(0 == cpu.X);
    CHECK(0 == cpu.Y);
    CHECK(0xFD == cpu.S);
    CHECK(0x0200 == cpu.PC);
    CHECK(tf(cpu, 0));
}

// CPU reset
TEST(reset) {
    auto cpu = init_cpu();
    cpu.mem.w16(0xFFFC, 0x1234);
    cpu.reset();
    CHECK(0xFD == cpu.S);
    CHECK(0x1234 == cpu.PC);
    CHECK(tf(cpu, f::IF));
}

TEST(LDA) {
    system_bus bus;
    auto cpu = init_cpu();

    uint8_t prog[] = {
        // immediate
        0xA9, 0x00,         // LDA #$00
        0xA9, 0x01,         // LDA #$01
        0xA9, 0x00,         // LDA #$00
        0xA9, 0x80,         // LDA #$80

        // zero page
        0xA5, 0x02,         // LDA $02
        0xA5, 0x03,         // LDA $03
        0xA5, 0x80,         // LDA $80
        0xA5, 0xFF,         // LDA $FF

        // absolute
        0xAD, 0x00, 0x10,   // LDA $1000
        0xAD, 0xFF, 0xFF,   // LDA $FFFF
        0xAD, 0x21, 0x00,   // LDA $0021

        // zero page,X
        0xA2, 0x0F,         // LDX #$0F
        0xB5, 0x10,         // LDA $10,X    => 0x1F
        0xB5, 0xF8,         // LDA $FE,X    => 0x07
        0xB5, 0x78,         // LDA $78,X    => 0x87

        // absolute,X
        0xBD, 0xF1, 0x0F,   // LDA $0x0FF1,X    => 0x1000
        0xBD, 0xF0, 0xFF,   // LDA $0xFFF0,X    => 0xFFFF
        0xBD, 0x12, 0x00,   // LDA $0x0012,X    => 0x0021

        // absolute,Y
        0xA0, 0xF0,         // LDY #$F0
        0xB9, 0x10, 0x0F,   // LDA $0x0F10,Y    => 0x1000
        0xB9, 0x0F, 0xFF,   // LDA $0xFF0F,Y    => 0xFFFF
        0xB9, 0x31, 0xFF,   // LDA $0xFF31,Y    => 0x0021

        // indirect,X
        0xA1, 0xF0,         // LDA ($F0,X)  => 0xFF, second byte in 0x00 => 0x1234
        0xA1, 0x70,         // LDA ($70,X)  => 0x70 => 0x4321

        // indirect,Y
        0xB1, 0xFF,         // LDA ($FF),Y  => 0x1234+0xF0 => 0x1324
        0xB1, 0x7F,         // LDA ($7F),Y  => 0x4321+0xF0 => 0x4411
    };
    cpu.mem.w8(0x0002, 0x01); cpu.mem.w8(0x0003, 0x00); cpu.mem.w8(0x0080, 0x80); cpu.mem.w8(0x00FF, 0x03);
    cpu.mem.w8(0x1000, 0x12); cpu.mem.w8(0xFFFF, 0x34); cpu.mem.w8(0x0021, 0x56);
    cpu.mem.w8(0x001F, 0xAA); cpu.mem.w8(0x0007, 0x33); cpu.mem.w8(0x0087, 0x22);

    cpu.mem.write(0x200, prog, sizeof(prog));

    // immediate
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x80); CHECK(tf(cpu, f::NF));

    // zero page
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.A == 0x01); CHECK(tf(cpu, 0));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.A == 0x80); CHECK(tf(cpu, f::NF));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.A == 0x03); CHECK(tf(cpu, 0));

    // absolute
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x34); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x56); CHECK(tf(cpu, 0));

    // zero page,X
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x0F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0xAA); CHECK(tf(cpu, f::NF));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x33); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x22); CHECK(tf(cpu, 0));

    // absolute,X
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.A == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x34); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x56); CHECK(tf(cpu, 0));

    // absolute,Y
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0xF0); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.A == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x34); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.A == 0x56); CHECK(tf(cpu, 0));

    // indirect,X
    cpu.mem.w8(0x00FF, 0x34); cpu.mem.w8(0x00, 0x12); cpu.mem.w16(0x7f, 0x4321);
    cpu.mem.w8(0x1234, 0x89); cpu.mem.w8(0x4321, 0x8A);    
    CHECK(6 == cpu.step_op(&bus)); CHECK(cpu.A == 0x89); CHECK(tf(cpu, f::NF));
    CHECK(6 == cpu.step_op(&bus)); CHECK(cpu.A == 0x8A); CHECK(tf(cpu, f::NF));

    // indirect,Y (both 6 cycles because page boundary crossed)
    cpu.mem.w8(0x1324, 0x98); cpu.mem.w8(0x4411, 0xA8);
    CHECK(6 == cpu.step_op(&bus)); CHECK(cpu.A == 0x98); CHECK(tf(cpu, f::NF));
    CHECK(6 == cpu.step_op(&bus)); CHECK(cpu.A == 0xA8); CHECK(tf(cpu, f::NF));
}

TEST(LDX) {
    system_bus bus;
    auto cpu = init_cpu();

    uint8_t prog[] = {
        // immediate
        0xA2, 0x00,         // LDX #$00
        0xA2, 0x01,         // LDX #$01
        0xA2, 0x00,         // LDX #$00
        0xA2, 0x80,         // LDX #$80

        // zero page
        0xA6, 0x02,         // LDX $02
        0xA6, 0x03,         // LDX $03
        0xA6, 0x80,         // LDX $80
        0xA6, 0xFF,         // LDX $FF

        // absolute
        0xAE, 0x00, 0x10,   // LDX $1000
        0xAE, 0xFF, 0xFF,   // LDX $FFFF
        0xAE, 0x21, 0x00,   // LDX $0021

        // zero page,Y
        0xA0, 0x0F,         // LDY #$0F
        0xB6, 0x10,         // LDX $10,Y    => 0x1F
        0xB6, 0xF8,         // LDX $FE,Y    => 0x07
        0xB6, 0x78,         // LDX $78,Y    => 0x87

        // absolute,Y
        0xA0, 0xF0,         // LDY #$F0
        0xBE, 0x10, 0x0F,   // LDX $0F10,Y    => 0x1000
        0xBE, 0x0F, 0xFF,   // LDX $FF0F,Y    => 0xFFFF
        0xBE, 0x31, 0xFF,   // LDX $FF31,Y    => 0x0021
    };
    cpu.mem.w8(0x0002, 0x01); cpu.mem.w8(0x0003, 0x00); cpu.mem.w8(0x0080, 0x80); cpu.mem.w8(0x00FF, 0x03);
    cpu.mem.w8(0x1000, 0x12); cpu.mem.w8(0xFFFF, 0x34); cpu.mem.w8(0x0021, 0x56);
    cpu.mem.w8(0x001F, 0xAA); cpu.mem.w8(0x0007, 0x33); cpu.mem.w8(0x0087, 0x22);

    cpu.mem.write(0x200, prog, sizeof(prog));

    // immediate
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x80); CHECK(tf(cpu, f::NF));

    // zero page
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.X == 0x01); CHECK(tf(cpu, 0));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.X == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.X == 0x80); CHECK(tf(cpu, f::NF));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.X == 0x03); CHECK(tf(cpu, 0));

    // absolute
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.X == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.X == 0x34); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.X == 0x56); CHECK(tf(cpu, 0));

    // zero page,Y
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x0F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.X == 0xAA); CHECK(tf(cpu, f::NF));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.X == 0x33); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.X == 0x22); CHECK(tf(cpu, 0));

    // absolute,X
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0xF0); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.X == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.X == 0x34); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.X == 0x56); CHECK(tf(cpu, 0));
}

TEST(LDY) {
    system_bus bus;
    auto cpu = init_cpu();

    uint8_t prog[] = {
        // immediate
        0xA0, 0x00,         // LDY #$00
        0xA0, 0x01,         // LDY #$01
        0xA0, 0x00,         // LDY #$00
        0xA0, 0x80,         // LDY #$80

        // zero page
        0xA4, 0x02,         // LDY $02
        0xA4, 0x03,         // LDY $03
        0xA4, 0x80,         // LDY $80
        0xA4, 0xFF,         // LDY $FF

        // absolute
        0xAC, 0x00, 0x10,   // LDY $1000
        0xAC, 0xFF, 0xFF,   // LDY $FFFF
        0xAC, 0x21, 0x00,   // LDY $0021

        // zero page,X
        0xA2, 0x0F,         // LDX #$0F
        0xB4, 0x10,         // LDY $10,X    => 0x1F
        0xB4, 0xF8,         // LDY $FE,X    => 0x07
        0xB4, 0x78,         // LDY $78,X    => 0x87

        // absolute,X
        0xA2, 0xF0,         // LDX #$F0
        0xBC, 0x10, 0x0F,   // LDY $0F10,X    => 0x1000
        0xBC, 0x0F, 0xFF,   // LDY $FF0F,X    => 0xFFFF
        0xBC, 0x31, 0xFF,   // LDY $FF31,X    => 0x0021
    };
    cpu.mem.w8(0x0002, 0x01); cpu.mem.w8(0x0003, 0x00); cpu.mem.w8(0x0080, 0x80); cpu.mem.w8(0x00FF, 0x03);
    cpu.mem.w8(0x1000, 0x12); cpu.mem.w8(0xFFFF, 0x34); cpu.mem.w8(0x0021, 0x56);
    cpu.mem.w8(0x001F, 0xAA); cpu.mem.w8(0x0007, 0x33); cpu.mem.w8(0x0087, 0x22);

    cpu.mem.write(0x200, prog, sizeof(prog));

    // immediate
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x80); CHECK(tf(cpu, f::NF));

    // zero page
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x01); CHECK(tf(cpu, 0));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x80); CHECK(tf(cpu, f::NF));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x03); CHECK(tf(cpu, 0));

    // absolute
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x34); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x56); CHECK(tf(cpu, 0));

    // zero page,Y
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x0F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.Y == 0xAA); CHECK(tf(cpu, f::NF));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x33); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x22); CHECK(tf(cpu, 0));

    // absolute,X
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0xF0); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x34); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x56); CHECK(tf(cpu, 0));
}

TEST(STA) {
    system_bus bus;
    auto cpu = init_cpu();

    uint8_t prog[] = {
        0xA9, 0x23,             // LDA #$23
        0xA2, 0x10,             // LDX #$10
        0xA0, 0xC0,             // LDY #$C0
        0x85, 0x10,             // STA $10
        0x8D, 0x34, 0x12,       // STA $1234
        0x95, 0x10,             // STA $10,X
        0x9D, 0x00, 0x20,       // STA $2000,X
        0x99, 0x00, 0x20,       // STA $2000,Y
        0x81, 0x10,             // STA ($10,X)
        0x91, 0x20,             // STA ($20),Y
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x23);
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x10);
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0xC0);
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x0010) == 0x23);
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x1234) == 0x23);
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x0020) == 0x23);
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x2010) == 0x23);
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x20C0) == 0x23);
    cpu.mem.w16(0x0020, 0x4321);
    CHECK(6 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x4321) == 0x23);
    CHECK(6 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x43E1) == 0x23);
}

TEST(STX) {
    system_bus bus;
    auto cpu = init_cpu();

    uint8_t prog[] = {
        0xA2, 0x23,             // LDX #$23
        0xA0, 0x10,             // LDY #$10

        0x86, 0x10,             // STX $10
        0x8E, 0x34, 0x12,       // STX $1234
        0x96, 0x10,             // STX $10,Y
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x23);
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x10);
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x0010) == 0x23);
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x1234) == 0x23);
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x0020) == 0x23);
}

TEST(STY) {
    system_bus bus;
    auto cpu = init_cpu();

    uint8_t prog[] = {
        0xA0, 0x23,             // LDY #$23
        0xA2, 0x10,             // LDX #$10

        0x84, 0x10,             // STX $10
        0x8C, 0x34, 0x12,       // STX $1234
        0x94, 0x10,             // STX $10,Y
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x23);
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x10);
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x0010) == 0x23);
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x1234) == 0x23);
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.mem.r8(0x0020) == 0x23);
}

TEST(TAX_TXA) {
    system_bus bus;
    auto cpu = init_cpu();

    uint8_t prog[] = {
        0xA9, 0x00,     // LDA #$00
        0xA2, 0x10,     // LDX #$10
        0xAA,           // TAX
        0xA9, 0xF0,     // LDA #$F0
        0x8A,           // TXA
        0xA9, 0xF0,     // LDA #$F0
        0xA2, 0x00,     // LDX #$00
        0xAA,           // TAX
        0xA9, 0x01,     // LDA #$01
        0x8A,           // TXA
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x10); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0xF0); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0xF0); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0xF0); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0xF0); CHECK(tf(cpu, f::NF));
}

TEST(TAY_TYA) {
    system_bus bus;
    auto cpu = init_cpu();

    uint8_t prog[] = {
        0xA9, 0x00,     // LDA #$00
        0xA0, 0x10,     // LDY #$10
        0xA8,           // TAY
        0xA9, 0xF0,     // LDA #$F0
        0x98,           // TYA
        0xA9, 0xF0,     // LDA #$F0
        0xA0, 0x00,     // LDY #$00
        0xA8,           // TAY
        0xA9, 0x01,     // LDA #$01
        0x98,           // TYA
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x10); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0xF0); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0xF0); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0xF0); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0xF0); CHECK(tf(cpu, f::NF));
}

TEST(DEX_INX_DEY_INY) {
    system_bus bus;
    auto cpu = init_cpu();

    uint8_t prog[] = {
        0xA2, 0x01,     // LDX #$01
        0xCA,           // DEX
        0xCA,           // DEX
        0xE8,           // INX
        0xE8,           // INX
        0xA0, 0x01,     // LDY #$01
        0x88,           // DEY
        0x88,           // DEY
        0xC8,           // INY
        0xC8,           // INY
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0xFF); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0xFF); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x01); CHECK(tf(cpu, 0));
}

TEST(TXS_TSX) {
    system_bus bus;
    auto cpu = init_cpu();

    uint8_t prog[] = {
        0xA2, 0xAA,     // LDX #$AA
        0xA9, 0x00,     // LDA #$00
        0x9A,           // TXS
        0xAA,           // TAX
        0xBA,           // TSX
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0xAA); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.S == 0xAA); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0xAA); CHECK(tf(cpu, f::NF));
}

TEST(NOP) {
    system_bus bus;
    auto cpu = init_cpu();
    uint8_t prog[] = {
        0xEA,       // NOP
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus));
}

TEST(ORA) {
    system_bus bus;
    auto cpu = init_cpu();
    uint8_t prog[] = {
        0xA9, 0x00,         // LDA #$00
        0xA2, 0x01,         // LDX #$01
        0xA0, 0x02,         // LDY #$02
        0x09, 0x00,         // ORA #$00
        0x05, 0x10,         // ORA $10
        0x15, 0x10,         // ORA $10,X
        0x0d, 0x00, 0x10,   // ORA $1000
        0x1d, 0x00, 0x10,   // ORA $1000,X
        0x19, 0x00, 0x10,   // ORA $1000,Y
        0x01, 0x22,         // ORA ($22,X)
        0x11, 0x20,         // ORA ($20),Y
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));
    cpu.mem.w16(0x0020, 0x1002);
    cpu.mem.w16(0x0023, 0x1003);
    cpu.mem.w8(0x0010, (1<<0));
    cpu.mem.w8(0x0011, (1<<1));
    cpu.mem.w8(0x1000, (1<<2));
    cpu.mem.w8(0x1001, (1<<3));
    cpu.mem.w8(0x1002, (1<<4));
    cpu.mem.w8(0x1003, (1<<5));
    cpu.mem.w8(0x1004, (1<<6));

    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x02); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.A == 0x01); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x03); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x07); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x0F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x1F); CHECK(tf(cpu, 0));
    CHECK(6 == cpu.step_op(&bus)); CHECK(cpu.A == 0x3F); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.A == 0x7F); CHECK(tf(cpu, 0));
}

TEST(AND) {
    system_bus bus;
    auto cpu = init_cpu();
    uint8_t prog[] = {
        0xA9, 0xFF,         // LDA #$FF
        0xA2, 0x01,         // LDX #$01
        0xA0, 0x02,         // LDY #$02
        0x29, 0xFF,         // AND #$FF
        0x25, 0x10,         // AND $10
        0x35, 0x10,         // AND $10,X
        0x2d, 0x00, 0x10,   // AND $1000
        0x3d, 0x00, 0x10,   // AND $1000,X
        0x39, 0x00, 0x10,   // AND $1000,Y
        0x21, 0x22,         // AND ($22,X)
        0x31, 0x20,         // AND ($20),Y
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));
    cpu.mem.w16(0x0020, 0x1002);
    cpu.mem.w16(0x0023, 0x1003);
    cpu.mem.w8(0x0010, 0x7F);
    cpu.mem.w8(0x0011, 0x3F);
    cpu.mem.w8(0x1000, 0x1F);
    cpu.mem.w8(0x1001, 0x0F);
    cpu.mem.w8(0x1002, 0x07);
    cpu.mem.w8(0x1003, 0x03);
    cpu.mem.w8(0x1004, 0x01);

    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0xFF); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x02); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0xFF); CHECK(tf(cpu, f::NF));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.A == 0x7F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x3F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x1F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x0F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x07); CHECK(tf(cpu, 0));
    CHECK(6 == cpu.step_op(&bus)); CHECK(cpu.A == 0x03); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.A == 0x01); CHECK(tf(cpu, 0));
}

TEST(EOR) {
    system_bus bus;
    auto cpu = init_cpu();
    uint8_t prog[] = {
        0xA9, 0xFF,         // LDA #$FF
        0xA2, 0x01,         // LDX #$01
        0xA0, 0x02,         // LDY #$02
        0x49, 0xFF,         // EOR #$FF
        0x45, 0x10,         // EOR $10
        0x55, 0x10,         // EOR $10,X
        0x4d, 0x00, 0x10,   // EOR $1000
        0x5d, 0x00, 0x10,   // EOR $1000,X
        0x59, 0x00, 0x10,   // EOR $1000,Y
        0x41, 0x22,         // EOR ($22,X)
        0x51, 0x20,         // EOR ($20),Y
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));
    cpu.mem.w16(0x0020, 0x1002);
    cpu.mem.w16(0x0023, 0x1003);
    cpu.mem.w8(0x0010, 0x7F);
    cpu.mem.w8(0x0011, 0x3F);
    cpu.mem.w8(0x1000, 0x1F);
    cpu.mem.w8(0x1001, 0x0F);
    cpu.mem.w8(0x1002, 0x07);
    cpu.mem.w8(0x1003, 0x03);
    cpu.mem.w8(0x1004, 0x01);

    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0xFF); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.X == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.Y == 0x02); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.A == 0x7F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x40); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x5F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x50); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x57); CHECK(tf(cpu, 0));
    CHECK(6 == cpu.step_op(&bus)); CHECK(cpu.A == 0x54); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step_op(&bus)); CHECK(cpu.A == 0x55); CHECK(tf(cpu, 0));
}

TEST(PHA_PLA_PHP_PLP) {
    system_bus bus;
    auto cpu = init_cpu();
    uint8_t prog[] = {
        0xA9, 0x23,     // LDA #$23
        0x48,           // PHA
        0xA9, 0x32,     // LDA #$32
        0x68,           // PLA
        0x08,           // PHP
        0xA9, 0x00,     // LDA #$00
        0x28,           // PLP
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x23); CHECK(cpu.S == 0xFD);
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.S == 0xFC); CHECK(cpu.mem.r8(0x01FD) == 0x23);
    CHECK(2 == cpu.step_op(&bus)); CHECK(cpu.A == 0x32);
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.A == 0x23); CHECK(cpu.S == 0xFD); CHECK(tf(cpu, 0));
    CHECK(3 == cpu.step_op(&bus)); CHECK(cpu.S == 0xFC); CHECK(cpu.mem.r8(0x01FD) == (f::XF|f::IF|f::BF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, f::ZF));
    CHECK(4 == cpu.step_op(&bus)); CHECK(cpu.S == 0xFD); CHECK(tf(cpu, 0));
}

TEST(CLC_SEC_CLI_SEI_CLV_CLD_SED) {
    system_bus bus;
    auto cpu = init_cpu();
    uint8_t prog[] = {
        0xB8,       // CLV
        0x78,       // SEI
        0x58,       // CLI
        0x38,       // SEC
        0x18,       // CLC
        0xF8,       // SED
        0xD8,       // CLD
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));
    cpu.P |= f::VF;

    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, f::IF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, f::CF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, f::DF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, 0));
}

TEST(INC_DEC) {
    system_bus bus;
    auto cpu = init_cpu();
    uint8_t prog[] = {
        0xA2, 0x10,         // LDX #$10
        0xE6, 0x33,         // INC $33
        0xF6, 0x33,         // INC $33,X
        0xEE, 0x00, 0x10,   // INC $1000
        0xFE, 0x00, 0x10,   // INC $1000,X
        0xC6, 0x33,         // DEC $33
        0xD6, 0x33,         // DEC $33,X
        0xCE, 0x00, 0x10,   // DEC $1000
        0xDE, 0x00, 0x10,   // DEC $1000,X
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus)); CHECK(0x10 == cpu.X);
    CHECK(5 == cpu.step_op(&bus)); CHECK(0x01 == cpu.mem.r8(0x0033)); CHECK(tf(cpu, 0));
    CHECK(6 == cpu.step_op(&bus)); CHECK(0x01 == cpu.mem.r8(0x0043)); CHECK(tf(cpu, 0));
    CHECK(6 == cpu.step_op(&bus)); CHECK(0x01 == cpu.mem.r8(0x1000)); CHECK(tf(cpu, 0));
    CHECK(7 == cpu.step_op(&bus)); CHECK(0x01 == cpu.mem.r8(0x1010)); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step_op(&bus)); CHECK(0x00 == cpu.mem.r8(0x0033)); CHECK(tf(cpu, f::ZF));
    CHECK(6 == cpu.step_op(&bus)); CHECK(0x00 == cpu.mem.r8(0x0043)); CHECK(tf(cpu, f::ZF));
    CHECK(6 == cpu.step_op(&bus)); CHECK(0x00 == cpu.mem.r8(0x1000)); CHECK(tf(cpu, f::ZF));
    CHECK(7 == cpu.step_op(&bus)); CHECK(0x00 == cpu.mem.r8(0x1010)); CHECK(tf(cpu, f::ZF));
}

TEST(ADC_SBC) {
    system_bus bus;
    auto cpu = init_cpu();
    uint8_t prog[] = {
        0xA9, 0x01,         // LDA #$01
        0x85, 0x10,         // STA $10
        0x8D, 0x00, 0x10,   // STA $1000
        0xA9, 0xFC,         // LDA #$FC
        0xA2, 0x08,         // LDX #$08
        0xA0, 0x04,         // LDY #$04
        0x18,               // CLC
        0x69, 0x01,         // ADC #$01
        0x65, 0x10,         // ADC $10
        0x75, 0x08,         // ADC $8,X
        0x6D, 0x00, 0x10,   // ADC $1000
        0x7D, 0xF8, 0x0F,   // ADC $0FF8,X
        0x79, 0xFC, 0x0F,   // ADC $0FFC,Y
        // FIXME: ADC (zp,X) and ADC (zp),X
        0xF9, 0xFC, 0x0F,   // SBC $0FFC,Y
        0xFD, 0xF8, 0x0F,   // SBC $0FF8,X
        0xED, 0x00, 0x10,   // SBC $1000
        0xF5, 0x08,         // SBC $8,X
        0xE5, 0x10,         // SBC $10
        0xE9, 0x01,         // SBC #$10
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus)); CHECK(0x01 == cpu.A);
    CHECK(3 == cpu.step_op(&bus)); CHECK(0x01 == cpu.mem.r8(0x0010));
    CHECK(4 == cpu.step_op(&bus)); CHECK(0x01 == cpu.mem.r8(0x1000));
    CHECK(2 == cpu.step_op(&bus)); CHECK(0xFC == cpu.A);
    CHECK(2 == cpu.step_op(&bus)); CHECK(0x08 == cpu.X);
    CHECK(2 == cpu.step_op(&bus)); CHECK(0x04 == cpu.Y);
    CHECK(2 == cpu.step_op(&bus));  // CLC
    // ADC
    CHECK(2 == cpu.step_op(&bus)); CHECK(0xFD == cpu.A); CHECK(tf(cpu, f::NF));
    CHECK(3 == cpu.step_op(&bus)); CHECK(0xFE == cpu.A); CHECK(tf(cpu, f::NF));
    CHECK(4 == cpu.step_op(&bus)); CHECK(0xFF == cpu.A); CHECK(tf(cpu, f::NF));
    CHECK(4 == cpu.step_op(&bus)); CHECK(0x00 == cpu.A); CHECK(tf(cpu, f::ZF|f::CF));
    CHECK(5 == cpu.step_op(&bus)); CHECK(0x02 == cpu.A); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step_op(&bus)); CHECK(0x03 == cpu.A); CHECK(tf(cpu, 0));
    // SBC
    CHECK(5 == cpu.step_op(&bus)); CHECK(0x01 == cpu.A); CHECK(tf(cpu, f::CF));
    CHECK(5 == cpu.step_op(&bus)); CHECK(0x00 == cpu.A); CHECK(tf(cpu, f::ZF|f::CF));
    CHECK(4 == cpu.step_op(&bus)); CHECK(0xFF == cpu.A); CHECK(tf(cpu, f::NF));
    CHECK(4 == cpu.step_op(&bus)); CHECK(0xFD == cpu.A); CHECK(tf(cpu, f::NF|f::CF));
    CHECK(3 == cpu.step_op(&bus)); CHECK(0xFC == cpu.A); CHECK(tf(cpu, f::NF|f::CF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(0xFB == cpu.A); CHECK(tf(cpu, f::NF|f::CF));
}

TEST(CMP_CPX_CPY) {
    system_bus bus;
    auto cpu = init_cpu();

    // I'm lazy and only test the immediate ops
    uint8_t prog[] = {
        0xA9, 0x01,     // LDA #$01
        0xA2, 0x02,     // LDX #$02
        0xA0, 0x03,     // LDY #$03
        0xC9, 0x00,     // CMP #$00
        0xC9, 0x01,     // CMP #$01
        0xC9, 0x02,     // CMP #$02
        0xE0, 0x01,     // CPX #$01
        0xE0, 0x02,     // CPX #$02
        0xE0, 0x03,     // CPX #$03
        0xC0, 0x02,     // CPY #$02
        0xC0, 0x02,     // CPY #$03
        0xC0, 0x03,     // CPY #$04
    };
    cpu.mem.write(0x0200, prog, sizeof(prog));

    CHECK(2 == cpu.step_op(&bus)); CHECK(0x01 == cpu.A);
    CHECK(2 == cpu.step_op(&bus)); CHECK(0x02 == cpu.X);
    CHECK(2 == cpu.step_op(&bus)); CHECK(0x03 == cpu.Y);
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, f::CF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, f::ZF|f::CF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, f::NF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, f::CF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, f::ZF|f::CF));
    CHECK(2 == cpu.step_op(&bus)); CHECK(tf(cpu, f::NF));
}


