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
    cpu.init();
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
    CHECK(0 == cpu.S);
    CHECK(0 == cpu.PC);
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
    cpu.PC = 0x0200;

    // immediate
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.A == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.A == 0x80); CHECK(tf(cpu, f::NF));

    // zero page
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.A == 0x01); CHECK(tf(cpu, 0));
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.A == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.A == 0x80); CHECK(tf(cpu, f::NF));
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.A == 0x03); CHECK(tf(cpu, 0));

    // absolute
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.A == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.A == 0x34); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.A == 0x56); CHECK(tf(cpu, 0));

    // zero page,X
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.X == 0x0F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.A == 0xAA); CHECK(tf(cpu, f::NF));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.A == 0x33); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.A == 0x22); CHECK(tf(cpu, 0));

    // absolute,X
    CHECK(5 == cpu.step(&bus)); CHECK(cpu.A == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.A == 0x34); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.A == 0x56); CHECK(tf(cpu, 0));

    // absolute,Y
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.Y == 0xF0); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step(&bus)); CHECK(cpu.A == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.A == 0x34); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step(&bus)); CHECK(cpu.A == 0x56); CHECK(tf(cpu, 0));

    // indirect,Y
    cpu.mem.w8(0x00FF, 0x34); cpu.mem.w8(0x00, 0x12); cpu.mem.w16(0x7f, 0x4321);
    cpu.mem.w8(0x1234, 0x89); cpu.mem.w8(0x4321, 0x8A);    
    CHECK(6 == cpu.step(&bus)); CHECK(cpu.A == 0x89); CHECK(tf(cpu, f::NF));
    CHECK(6 == cpu.step(&bus)); CHECK(cpu.A == 0x8A); CHECK(tf(cpu, f::NF));

    // indirect,Y (both 6 cycles because page boundary crossed)
    cpu.mem.w8(0x1324, 0x98); cpu.mem.w8(0x4411, 0xA8);
    CHECK(6 == cpu.step(&bus)); CHECK(cpu.A == 0x98); CHECK(tf(cpu, f::NF));
    CHECK(6 == cpu.step(&bus)); CHECK(cpu.A == 0xA8); CHECK(tf(cpu, f::NF));
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
    cpu.PC = 0x0200;

    // immediate
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.X == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.X == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.X == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.X == 0x80); CHECK(tf(cpu, f::NF));

    // zero page
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.X == 0x01); CHECK(tf(cpu, 0));
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.X == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.X == 0x80); CHECK(tf(cpu, f::NF));
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.X == 0x03); CHECK(tf(cpu, 0));

    // absolute
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.X == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.X == 0x34); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.X == 0x56); CHECK(tf(cpu, 0));

    // zero page,Y
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.Y == 0x0F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.X == 0xAA); CHECK(tf(cpu, f::NF));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.X == 0x33); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.X == 0x22); CHECK(tf(cpu, 0));

    // absolute,X
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.Y == 0xF0); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step(&bus)); CHECK(cpu.X == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.X == 0x34); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step(&bus)); CHECK(cpu.X == 0x56); CHECK(tf(cpu, 0));
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
        0xB4, 0x10,         // LDY $10,Y    => 0x1F
        0xB4, 0xF8,         // LDY $FE,Y    => 0x07
        0xB4, 0x78,         // LDY $78,Y    => 0x87

        // absolute,X
        0xA2, 0xF0,         // LDX #$F0
        0xBC, 0x10, 0x0F,   // LDY $0F10,Y    => 0x1000
        0xBC, 0x0F, 0xFF,   // LDY $FF0F,Y    => 0xFFFF
        0xBC, 0x31, 0xFF,   // LDY $FF31,Y    => 0x0021
    };
    cpu.mem.w8(0x0002, 0x01); cpu.mem.w8(0x0003, 0x00); cpu.mem.w8(0x0080, 0x80); cpu.mem.w8(0x00FF, 0x03);
    cpu.mem.w8(0x1000, 0x12); cpu.mem.w8(0xFFFF, 0x34); cpu.mem.w8(0x0021, 0x56);
    cpu.mem.w8(0x001F, 0xAA); cpu.mem.w8(0x0007, 0x33); cpu.mem.w8(0x0087, 0x22);

    cpu.mem.write(0x200, prog, sizeof(prog));
    cpu.PC = 0x0200;

    // immediate
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.Y == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.Y == 0x01); CHECK(tf(cpu, 0));
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.Y == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.Y == 0x80); CHECK(tf(cpu, f::NF));

    // zero page
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.Y == 0x01); CHECK(tf(cpu, 0));
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.Y == 0x00); CHECK(tf(cpu, f::ZF));
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.Y == 0x80); CHECK(tf(cpu, f::NF));
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.Y == 0x03); CHECK(tf(cpu, 0));

    // absolute
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.Y == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.Y == 0x34); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.Y == 0x56); CHECK(tf(cpu, 0));

    // zero page,Y
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.X == 0x0F); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.Y == 0xAA); CHECK(tf(cpu, f::NF));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.Y == 0x33); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.Y == 0x22); CHECK(tf(cpu, 0));

    // absolute,X
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.X == 0xF0); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step(&bus)); CHECK(cpu.Y == 0x12); CHECK(tf(cpu, 0));
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.Y == 0x34); CHECK(tf(cpu, 0));
    CHECK(5 == cpu.step(&bus)); CHECK(cpu.Y == 0x56); CHECK(tf(cpu, 0));
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
    cpu.PC = 0x200;

    CHECK(2 == cpu.step(&bus)); CHECK(cpu.A == 0x23);
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.X == 0x10);
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.Y == 0xC0);
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x0010) == 0x23);
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x1234) == 0x23);
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x0020) == 0x23);
    CHECK(5 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x2010) == 0x23);
    CHECK(5 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x20C0) == 0x23);
    cpu.mem.w16(0x0020, 0x4321);
    CHECK(6 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x4321) == 0x23);
    CHECK(6 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x43E1) == 0x23);
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
    cpu.PC = 0x200;

    CHECK(2 == cpu.step(&bus)); CHECK(cpu.X == 0x23);
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.Y == 0x10);
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x0010) == 0x23);
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x1234) == 0x23);
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x0020) == 0x23);
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
    cpu.PC = 0x200;

    CHECK(2 == cpu.step(&bus)); CHECK(cpu.Y == 0x23);
    CHECK(2 == cpu.step(&bus)); CHECK(cpu.X == 0x10);
    CHECK(3 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x0010) == 0x23);
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x1234) == 0x23);
    CHECK(4 == cpu.step(&bus)); CHECK(cpu.mem.r8(0x0020) == 0x23);
}
