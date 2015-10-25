//------------------------------------------------------------------------------
//  cpu_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/z80.h"

using namespace yakc;

// LD r,s
// LD r,n
TEST(LD_r_sn) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x3E, 0x12,     // LD A,0x12
        0x47,           // LD B,A
        0x4F,           // LD C,A
        0x57,           // LD D,A
        0x5F,           // LD E,A
        0x67,           // LD H,A
        0x6F,           // LD L,A
        0x7F,           // LD A,A

        0x06, 0x13,     // LD B,0x13
        0x40,           // LD B,B
        0x48,           // LD C,B
        0x50,           // LD D,B
        0x58,           // LD E,B
        0x60,           // LD H,B
        0x68,           // LD L,B
        0x78,           // LD A,B

        0x0E, 0x14,     // LD C,0x14
        0x41,           // LD B,C
        0x49,           // LD C,C
        0x51,           // LD D,C
        0x59,           // LD E,C
        0x61,           // LD H,C
        0x69,           // LD L,C
        0x79,           // LD A,C

        0x16, 0x15,     // LD D,0x15
        0x42,           // LD B,D
        0x4A,           // LD C,D
        0x52,           // LD D,D
        0x5A,           // LD E,D
        0x62,           // LD H,D
        0x6A,           // LD L,D
        0x7A,           // LD A,D

        0x1E, 0x16,     // LD E,0x16
        0x43,           // LD B,E
        0x4B,           // LD C,E
        0x53,           // LD D,E
        0x5B,           // LD E,E
        0x63,           // LD H,E
        0x6B,           // LD L,E
        0x7B,           // LD A,E

        0x26, 0x17,     // LD H,0x17
        0x44,           // LD B,H
        0x4C,           // LD C,H
        0x54,           // LD D,H
        0x5C,           // LD E,H
        0x64,           // LD H,H
        0x6C,           // LD L,H
        0x7C,           // LD A,H

        0x2E, 0x18,     // LD L,0x18
        0x45,           // LD B,L
        0x4D,           // LD C,L
        0x55,           // LD D,L
        0x5D,           // LD E,L
        0x65,           // LD H,L
        0x6D,           // LD L,L
        0x7D,           // LD A,L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));
    cpu.step(); CHECK(0x12 == cpu.state.A); CHECK(7 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.state.B); CHECK(11 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.state.C); CHECK(15 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.state.D); CHECK(19 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.state.E); CHECK(23 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.state.H); CHECK(27 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.state.L); CHECK(31 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.state.A); CHECK(35 == cpu.state.T);

    cpu.step(); CHECK(0x13 == cpu.state.B); CHECK(42 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.state.B); CHECK(46 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.state.C); CHECK(50 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.state.D); CHECK(54 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.state.E); CHECK(58 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.state.H); CHECK(62 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.state.L); CHECK(66 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.state.A); CHECK(70 == cpu.state.T);

    cpu.step(); CHECK(0x14 == cpu.state.C); CHECK(77 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.state.B); CHECK(81 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.state.C); CHECK(85 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.state.D); CHECK(89 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.state.E); CHECK(93 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.state.H); CHECK(97 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.state.L); CHECK(101 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.state.A); CHECK(105 == cpu.state.T);

    cpu.step(); CHECK(0x15 == cpu.state.D); CHECK(112 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.state.B); CHECK(116 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.state.C); CHECK(120 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.state.D); CHECK(124 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.state.E); CHECK(128 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.state.H); CHECK(132 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.state.L); CHECK(136 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.state.A); CHECK(140 == cpu.state.T);

    cpu.step(); CHECK(0x16 == cpu.state.E); CHECK(147 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.state.B); CHECK(151 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.state.C); CHECK(155 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.state.D); CHECK(159 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.state.E); CHECK(163 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.state.H); CHECK(167 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.state.L); CHECK(171 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.state.A); CHECK(175 == cpu.state.T);

    cpu.step(); CHECK(0x17 == cpu.state.H); CHECK(182 == cpu.state.T);
    cpu.step(); CHECK(0x17 == cpu.state.B); CHECK(186 == cpu.state.T);
    cpu.step(); CHECK(0x17 == cpu.state.C); CHECK(190 == cpu.state.T);
    cpu.step(); CHECK(0x17 == cpu.state.D); CHECK(194 == cpu.state.T);
    cpu.step(); CHECK(0x17 == cpu.state.E); CHECK(198 == cpu.state.T);
    cpu.step(); CHECK(0x17 == cpu.state.H); CHECK(202 == cpu.state.T);
    cpu.step(); CHECK(0x17 == cpu.state.L); CHECK(206 == cpu.state.T);
    cpu.step(); CHECK(0x17 == cpu.state.A); CHECK(210 == cpu.state.T);

    cpu.step(); CHECK(0x18 == cpu.state.L); CHECK(217 == cpu.state.T);
    cpu.step(); CHECK(0x18 == cpu.state.B); CHECK(221 == cpu.state.T);
    cpu.step(); CHECK(0x18 == cpu.state.C); CHECK(225 == cpu.state.T);
    cpu.step(); CHECK(0x18 == cpu.state.D); CHECK(229 == cpu.state.T);
    cpu.step(); CHECK(0x18 == cpu.state.E); CHECK(233 == cpu.state.T);
    cpu.step(); CHECK(0x18 == cpu.state.H); CHECK(237 == cpu.state.T);
    cpu.step(); CHECK(0x18 == cpu.state.L); CHECK(241 == cpu.state.T);
    cpu.step(); CHECK(0x18 == cpu.state.A); CHECK(245 == cpu.state.T);
}

// LD r,(HL)
TEST(LD_r_iHL) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x21, 0x00, 0x10,   // LD HL,0x1000
        0x3E, 0x33,         // LD A,0x33
        0x77,               // LD (HL),A
        0x3E, 0x22,         // LD A,0x22
        0x46,               // LD B,(HL)
        0x4E,               // LD C,(HL)
        0x56,               // LD D,(HL)
        0x5E,               // LD E,(HL)
        0x66,               // LD H,(HL)
        0x26, 0x10,         // LD H,0x10
        0x6E,               // LD L,(HL)
        0x2E, 0x00,         // LD L,0x00
        0x7E,               // LD A,(HL)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1000 == cpu.state.HL);      CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.state.A);         CHECK(17 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.mem.r8(0x1000));  CHECK(24 == cpu.state.T);
    cpu.step(); CHECK(0x22 == cpu.state.A);         CHECK(31 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.state.B);         CHECK(38 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.state.C);         CHECK(45 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.state.D);         CHECK(52 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.state.E);         CHECK(59 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.state.H);         CHECK(66 == cpu.state.T);
    cpu.step(); CHECK(0x10 == cpu.state.H);         CHECK(73 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.state.L);         CHECK(80 == cpu.state.T);
    cpu.step(); CHECK(0x00 == cpu.state.L);         CHECK(87 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.state.A);         CHECK(94 == cpu.state.T);
}

// LD r,([IX|IY] + d)
TEST(LD_r_IXY_d) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte data[] = {
        1, 2, 3, 4, 5, 6, 7, 8
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0xDD, 0x21, 0x03, 0x10,     // LD IX,0x1003
        0xDD, 0x7E, 0x00,           // LD A,(IX+0)
        0xDD, 0x46, 0x01,           // LD B,(IX+1)
        0xDD, 0x4E, 0x02,           // LD C,(IX+2)
        0xDD, 0x56, 0xFF,           // LD D,(IX-1)
        0xDD, 0x5E, 0xFE,           // LD E,(IX-2)
        0xDD, 0x66, 0x03,           // LD H,(IX+3)
        0xDD, 0x6E, 0xFD,           // LD L,(IX-3)

        0xFD, 0x21, 0x04, 0x10,     // LD IY,0x1003
        0xFD, 0x7E, 0x00,           // LD A,(IY+0)
        0xFD, 0x46, 0x01,           // LD B,(IY+1)
        0xFD, 0x4E, 0x02,           // LD C,(IY+2)
        0xFD, 0x56, 0xFF,           // LD D,(IY-1)
        0xFD, 0x5E, 0xFE,           // LD E,(IY-2)
        0xFD, 0x66, 0x03,           // LD H,(IY+3)
        0xFD, 0x6E, 0xFD,           // LD L,(IY-3)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1003 == cpu.state.IX);  CHECK(14 == cpu.state.T);
    cpu.step(); CHECK(4 == cpu.state.A);        CHECK(33 == cpu.state.T);
    cpu.step(); CHECK(5 == cpu.state.B);        CHECK(52 == cpu.state.T);
    cpu.step(); CHECK(6 == cpu.state.C);        CHECK(71 == cpu.state.T);
    cpu.step(); CHECK(3 == cpu.state.D);        CHECK(90 == cpu.state.T);
    cpu.step(); CHECK(2 == cpu.state.E);        CHECK(109 == cpu.state.T);
    cpu.step(); CHECK(7 == cpu.state.H);        CHECK(128 == cpu.state.T);
    cpu.step(); CHECK(1 == cpu.state.L);        CHECK(147 == cpu.state.T);
    cpu.step(); CHECK(0x1004 == cpu.state.IY);  CHECK(161 == cpu.state.T);
    cpu.step(); CHECK(5 == cpu.state.A);        CHECK(180 == cpu.state.T);
    cpu.step(); CHECK(6 == cpu.state.B);        CHECK(199 == cpu.state.T);
    cpu.step(); CHECK(7 == cpu.state.C);        CHECK(218 == cpu.state.T);
    cpu.step(); CHECK(4 == cpu.state.D);        CHECK(237 == cpu.state.T);
    cpu.step(); CHECK(3 == cpu.state.E);        CHECK(256 == cpu.state.T);
    cpu.step(); CHECK(8 == cpu.state.H);        CHECK(275 == cpu.state.T);
    cpu.step(); CHECK(2 == cpu.state.L);        CHECK(294 == cpu.state.T);
}

// LD (HL),r
TEST(LD_iHL_r) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x21, 0x00, 0x10,   // LD HL,0x1000
        0x3E, 0x12,         // LD A,0x12
        0x77,               // LD (HL),A
        0x06, 0x13,         // LD B,0x13
        0x70,               // LD (HL),B
        0x0E, 0x14,         // LD C,0x14
        0x71,               // LD (HL),C
        0x16, 0x15,         // LD D,0x15
        0x72,               // LD (HL),D
        0x1E, 0x16,         // LD E,0x16
        0x73,               // LD (HL),E
        0x74,               // LD (HL),H
        0x75,               // LD (HL),L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1000 == cpu.state.HL);      CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.state.A);         CHECK(17 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.mem.r8(0x1000));  CHECK(24 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.state.B);         CHECK(31 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.mem.r8(0x1000));  CHECK(38 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.state.C);         CHECK(45 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.mem.r8(0x1000));  CHECK(52 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.state.D);         CHECK(59 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.mem.r8(0x1000));  CHECK(66 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.state.E);         CHECK(73 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.mem.r8(0x1000));  CHECK(80 == cpu.state.T);
    cpu.step(); CHECK(0x10 == cpu.mem.r8(0x1000));  CHECK(87 == cpu.state.T);
    cpu.step(); CHECK(0x00 == cpu.mem.r8(0x1000));  CHECK(94 == cpu.state.T);
}

// LD (IX|IY + d),r
TEST(LD_iIXY_d_r) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0xDD, 0x21, 0x03, 0x10,     // LD IX,0x1003
        0x3E, 0x12,                 // LD A,0x12
        0xDD, 0x77, 0x00,           // LD (IX+0),A
        0x06, 0x13,                 // LD B,0x13
        0xDD, 0x70, 0x01,           // LD (IX+1),B
        0x0E, 0x14,                 // LD C,0x14
        0xDD, 0x71, 0x02,           // LD (IX+2),C
        0x16, 0x15,                 // LD D,0x15
        0xDD, 0x72, 0xFF,           // LD (IX-1),D
        0x1E, 0x16,                 // LD E,0x16
        0xDD, 0x73, 0xFE,           // LD (IX-2),E
        0x26, 0x17,                 // LD H,0x17
        0xDD, 0x74, 0x03,           // LD (IX+3),H
        0x2E, 0x18,                 // LD L,0x18
        0xDD, 0x75, 0xFD,           // LD (IX-3),L
        0xFD, 0x21, 0x03, 0x10,     // LD IY,0x1003
        0x3E, 0x12,                 // LD A,0x12
        0xFD, 0x77, 0x00,           // LD (IY+0),A
        0x06, 0x13,                 // LD B,0x13
        0xFD, 0x70, 0x01,           // LD (IY+1),B
        0x0E, 0x14,                 // LD C,0x14
        0xFD, 0x71, 0x02,           // LD (IY+2),C
        0x16, 0x15,                 // LD D,0x15
        0xFD, 0x72, 0xFF,           // LD (IY-1),D
        0x1E, 0x16,                 // LD E,0x16
        0xFD, 0x73, 0xFE,           // LD (IY-2),E
        0x26, 0x17,                 // LD H,0x17
        0xFD, 0x74, 0x03,           // LD (IY+3),H
        0x2E, 0x18,                 // LD L,0x18
        0xFD, 0x75, 0xFD,           // LD (IY-3),L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1003 == cpu.state.IX);      CHECK(14 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.state.A);         CHECK(21 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.mem.r8(0x1003));  CHECK(40 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.state.B);         CHECK(47 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.mem.r8(0x1004));  CHECK(66 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.state.C);         CHECK(73 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.mem.r8(0x1005));  CHECK(92 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.state.D);         CHECK(99 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.mem.r8(0x1002));  CHECK(118 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.state.E);         CHECK(125 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.mem.r8(0x1001));  CHECK(144 == cpu.state.T);
    cpu.step(); CHECK(0x17 == cpu.state.H);         CHECK(151 == cpu.state.T);
    cpu.step(); CHECK(0x17 == cpu.mem.r8(0x1006));  CHECK(170 == cpu.state.T);
    cpu.step(); CHECK(0x18 == cpu.state.L);         CHECK(177 == cpu.state.T);
    cpu.step(); CHECK(0x18 == cpu.mem.r8(0x1000));  CHECK(196 == cpu.state.T);
    cpu.state.T = 0;
    cpu.step(); CHECK(0x1003 == cpu.state.IY);      CHECK(14 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.state.A);         CHECK(21 == cpu.state.T);
    cpu.step(); CHECK(0x12 == cpu.mem.r8(0x1003));  CHECK(40 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.state.B);         CHECK(47 == cpu.state.T);
    cpu.step(); CHECK(0x13 == cpu.mem.r8(0x1004));  CHECK(66 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.state.C);         CHECK(73 == cpu.state.T);
    cpu.step(); CHECK(0x14 == cpu.mem.r8(0x1005));  CHECK(92 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.state.D);         CHECK(99 == cpu.state.T);
    cpu.step(); CHECK(0x15 == cpu.mem.r8(0x1002));  CHECK(118 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.state.E);         CHECK(125 == cpu.state.T);
    cpu.step(); CHECK(0x16 == cpu.mem.r8(0x1001));  CHECK(144 == cpu.state.T);
    cpu.step(); CHECK(0x17 == cpu.state.H);         CHECK(151 == cpu.state.T);
    cpu.step(); CHECK(0x17 == cpu.mem.r8(0x1006));  CHECK(170 == cpu.state.T);
    cpu.step(); CHECK(0x18 == cpu.state.L);         CHECK(177 == cpu.state.T);
    cpu.step(); CHECK(0x18 == cpu.mem.r8(0x1000));  CHECK(196 == cpu.state.T);
}

// LD (HL),n
TEST(LD_iHL_n) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x21, 0x00, 0x20,   // LD HL,0x2000
        0x36, 0x33,         // LD (HL),0x33
        0x21, 0x00, 0x10,   // LD HL,0x1000
        0x36, 0x65,         // LD (HL),0x65
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x2000 == cpu.state.HL);      CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.mem.r8(0x2000));  CHECK(20 == cpu.state.T);
    cpu.step(); CHECK(0x1000 == cpu.state.HL);      CHECK(30 == cpu.state.T);
    cpu.step(); CHECK(0x65 == cpu.mem.r8(0x1000));  CHECK(40 == cpu.state.T);
}

// LD ([IX|IY] + d),n
TEST(LD_iIXY_d_n) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0xDD, 0x21, 0x00, 0x20,     // LD IX,0x2000
        0xDD, 0x36, 0x02, 0x33,     // LD (IX+2),0x33
        0xDD, 0x36, 0xFE, 0x11,     // LD (IX-2),0x11
        0xFD, 0x21, 0x00, 0x10,     // LD IY,0x1000
        0xFD, 0x36, 0x01, 0x22,     // LD (IY+1),0x22
        0xFD, 0x36, 0xFF, 0x44,     // LD (IY-1),0x44
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x2000 == cpu.state.IX);      CHECK(14 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.mem.r8(0x2002));  CHECK(33 == cpu.state.T);
    cpu.step(); CHECK(0x11 == cpu.mem.r8(0x1FFE));  CHECK(52 == cpu.state.T);
    cpu.step(); CHECK(0x1000 == cpu.state.IY);      CHECK(66 == cpu.state.T);
    cpu.step(); CHECK(0x22 == cpu.mem.r8(0x1001));  CHECK(85 == cpu.state.T);
    cpu.step(); CHECK(0x44 == cpu.mem.r8(0x0FFF));  CHECK(104 == cpu.state.T);
}

// LD A,(BC)
// LD A,(DE)
// LD A,(nn)
TEST(LD_A_iBCDEnn) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte data[] = {
        0x11, 0x22, 0x33
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x01, 0x00, 0x10,   // LD BC,0x1000
        0x11, 0x01, 0x10,   // LD DE,0x1001
        0x0A,               // LD A,(BC)
        0x1A,               // LD A,(DE)
        0x3A, 0x02, 0x10,   // LD A,(0x1002)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1000 == cpu.state.BC);  CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x1001 == cpu.state.DE);  CHECK(20 == cpu.state.T);
    cpu.step(); CHECK(0x11 == cpu.state.A);     CHECK(27 == cpu.state.T);
    cpu.step(); CHECK(0x22 == cpu.state.A);     CHECK(34 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.state.A);     CHECK(47 == cpu.state.T);
}

// LD (BC),A
// LD (DE),A
// LD (nn),A
TEST(LD_iBCDEnn_A) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x01, 0x00, 0x10,   // LD BC,0x1000
        0x11, 0x01, 0x10,   // LD DE,0x1001
        0x3E, 0x77,         // LD A,0x77
        0x02,               // LD (BC),A
        0x12,               // LD (DE),A
        0x32, 0x02, 0x10,   // LD (0x1002),A
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1000 == cpu.state.BC);      CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x1001 == cpu.state.DE);      CHECK(20 == cpu.state.T);
    cpu.step(); CHECK(0x77 == cpu.state.A);         CHECK(27 == cpu.state.T);
    cpu.step(); CHECK(0x77 == cpu.mem.r8(0x1000));  CHECK(34 == cpu.state.T);
    cpu.step(); CHECK(0x77 == cpu.mem.r8(0x1001));  CHECK(41 == cpu.state.T);
    cpu.step(); CHECK(0x77 == cpu.mem.r8(0x1002));  CHECK(54 == cpu.state.T);
}

// LD I,A
// LD R,A
TEST(LD_IR_A) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x3E, 0x45,     // LD A,0x45
        0xED, 0x47,     // LD I,A
        0xED, 0x4F,     // LD R,A
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x45 == cpu.state.A);     CHECK(7 == cpu.state.T);
    cpu.step(); CHECK(0x45 == cpu.state.I);     CHECK(16 == cpu.state.T);
    cpu.step(); CHECK(0x45 == cpu.state.R);     CHECK(25 == cpu.state.T);
}

// LD dd,nn
// LD IX,nn
// LD IY,nn
TEST(LD_ddIXY_nn) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x01, 0x34, 0x12,       // LD BC,0x1234
        0x11, 0x78, 0x56,       // LD DE,0x5678
        0x21, 0xBC, 0x9A,       // LD HL,0x9ABC
        0x31, 0x68, 0x13,       // LD SP,0x1368
        0xDD, 0x21, 0x21, 0x43, // LD IX,0x4321
        0xFD, 0x21, 0x65, 0x87, // LD IY,0x8765
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1234 == cpu.state.BC); CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x5678 == cpu.state.DE); CHECK(20 == cpu.state.T);
    cpu.step(); CHECK(0x9ABC == cpu.state.HL); CHECK(30 == cpu.state.T);
    cpu.step(); CHECK(0x1368 == cpu.state.SP); CHECK(40 == cpu.state.T);
    cpu.step(); CHECK(0x4321 == cpu.state.IX); CHECK(54 == cpu.state.T);
    cpu.step(); CHECK(0x8765 == cpu.state.IY); CHECK(68 == cpu.state.T);
}

// LD HL,(nn)
// LD dd,(nn)
// LD IX,(nn)
// LD IY,(nn)
TEST(LD_HLddIXY_inn) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte data[] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x2A, 0x00, 0x10,           // LD HL,(0x1000)
        0xED, 0x4B, 0x01, 0x10,     // LD BC,(0x1001)
        0xED, 0x5B, 0x02, 0x10,     // LD DE,(0x1002)
        0xED, 0x6B, 0x03, 0x10,     // LD HL,(0x1003) undocumented 'long' version
        0xED, 0x7B, 0x04, 0x10,     // LD SP,(0x1004)
        0xDD, 0x2A, 0x05, 0x10,     // LD IX,(0x1004)
        0xFD, 0x2A, 0x06, 0x10,     // LD IY,(0x1005)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x0201 == cpu.state.HL); CHECK(16 == cpu.state.T);
    cpu.step(); CHECK(0x0302 == cpu.state.BC); CHECK(36 == cpu.state.T);
    cpu.step(); CHECK(0x0403 == cpu.state.DE); CHECK(56 == cpu.state.T);
    cpu.step(); CHECK(0x0504 == cpu.state.HL); CHECK(76 == cpu.state.T);
    cpu.step(); CHECK(0x0605 == cpu.state.SP); CHECK(96 == cpu.state.T);
    cpu.step(); CHECK(0x0706 == cpu.state.IX); CHECK(116 == cpu.state.T);
    cpu.step(); CHECK(0x0807 == cpu.state.IY); CHECK(136 == cpu.state.T);
}

// LD (nn),HL
// LD (nn),dd
// LD (nn),IX
// LD (nn),IY
TEST(LD_inn_HLDDIXY) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x21, 0x01, 0x02,           // LD HL,0x0201
        0x22, 0x00, 0x10,           // LD (0x1000),HL
        0x01, 0x34, 0x12,           // LD BC,0x1234
        0xED, 0x43, 0x02, 0x10,     // LD (0x1002),BC
        0x11, 0x78, 0x56,           // LD DE,0x5678
        0xED, 0x53, 0x04, 0x10,     // LD (0x1004),DE
        0x21, 0xBC, 0x9A,           // LD HL,0x9ABC
        0xED, 0x63, 0x06, 0x10,     // LD (0x1006),HL undocumented 'long' version
        0x31, 0x68, 0x13,           // LD SP,0x1368
        0xED, 0x73, 0x08, 0x10,     // LD (0x1008),SP
        0xDD, 0x21, 0x21, 0x43,     // LD IX,0x4321
        0xDD, 0x22, 0x0A, 0x10,     // LD (0x100A),IX
        0xFD, 0x21, 0x65, 0x87,     // LD IY,0x8765
        0xFD, 0x22, 0x0C, 0x10,     // LD (0x100C),IY
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x0201 == cpu.state.HL);          CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x0201 == cpu.mem.r16(0x1000));   CHECK(26 == cpu.state.T);
    cpu.step(); CHECK(0x1234 == cpu.state.BC);          CHECK(36 == cpu.state.T);
    cpu.step(); CHECK(0x1234 == cpu.mem.r16(0x1002));   CHECK(56 == cpu.state.T);
    cpu.step(); CHECK(0x5678 == cpu.state.DE);          CHECK(66 == cpu.state.T);
    cpu.step(); CHECK(0x5678 == cpu.mem.r16(0x1004));   CHECK(86 == cpu.state.T);
    cpu.step(); CHECK(0x9ABC == cpu.state.HL);          CHECK(96 == cpu.state.T);
    cpu.step(); CHECK(0x9ABC == cpu.mem.r16(0x1006));   CHECK(116 == cpu.state.T);
    cpu.step(); CHECK(0x1368 == cpu.state.SP);          CHECK(126 == cpu.state.T);
    cpu.step(); CHECK(0x1368 == cpu.mem.r16(0x1008));   CHECK(146 == cpu.state.T);
    cpu.step(); CHECK(0x4321 == cpu.state.IX);          CHECK(160 == cpu.state.T);
    cpu.step(); CHECK(0x4321 == cpu.mem.r16(0x100A));   CHECK(180 == cpu.state.T);
    cpu.step(); CHECK(0x8765 == cpu.state.IY);          CHECK(194 == cpu.state.T);
    cpu.step(); CHECK(0x8765 == cpu.mem.r16(0x100C));   CHECK(214 == cpu.state.T);
}

// LD SP,HL
// LD SP,IX
// LD SP,IY
TEST(LD_SP_HLIXY) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x21, 0x34, 0x12,           // LD HL,0x1234
        0xDD, 0x21, 0x78, 0x56,     // LD IX,0x5678
        0xFD, 0x21, 0xBC, 0x9A,     // LD IY,0x9ABC
        0xF9,                       // LD SP,HL
        0xDD, 0xF9,                 // LD SP,IX
        0xFD, 0xF9,                 // LD SP,IY
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1234 == cpu.state.HL);  CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x5678 == cpu.state.IX);  CHECK(24 == cpu.state.T);
    cpu.step(); CHECK(0x9ABC == cpu.state.IY);  CHECK(38 == cpu.state.T);
    cpu.step(); CHECK(0x1234 == cpu.state.SP);  CHECK(44 == cpu.state.T);
    cpu.step(); CHECK(0x5678 == cpu.state.SP);  CHECK(54 == cpu.state.T);
    cpu.step(); CHECK(0x9ABC == cpu.state.SP);  CHECK(64 == cpu.state.T);
}

// PUSH qq
// PUSH IX
// PUSH IY
// POP qq
// POP IX
// POP IY
TEST(PUSH_POP_qqIXY) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x01, 0x34, 0x12,       // LD BC,0x1234
        0x11, 0x78, 0x56,       // LD DE,0x5678
        0x21, 0xBC, 0x9A,       // LD HL,0x9ABC
        0x3E, 0xEF,             // LD A,0xEF
        0xDD, 0x21, 0x45, 0x23, // LD IX,0x2345
        0xFD, 0x21, 0x89, 0x67, // LD IY,0x6789
        0x31, 0x00, 0x01,       // LD SP,0x0100
        0xF5,                   // PUSH AF
        0xC5,                   // PUSH BC
        0xD5,                   // PUSH DE
        0xE5,                   // PUSH HL
        0xDD, 0xE5,             // PUSH IX
        0xFD, 0xE5,             // PUSH IY
        0xF1,                   // POP AF
        0xC1,                   // POP BC
        0xD1,                   // POP DE
        0xE1,                   // POP HL
        0xDD, 0xE1,             // POP IX
        0xFD, 0xE1,             // POP IY
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1234 == cpu.state.BC);  CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x5678 == cpu.state.DE);  CHECK(20 == cpu.state.T);
    cpu.step(); CHECK(0x9ABC == cpu.state.HL);  CHECK(30 == cpu.state.T);
    cpu.step(); CHECK(0xEF00 == cpu.state.AF);  CHECK(37 == cpu.state.T);
    cpu.step(); CHECK(0x2345 == cpu.state.IX);  CHECK(51 == cpu.state.T);
    cpu.step(); CHECK(0x6789 == cpu.state.IY);  CHECK(65 == cpu.state.T);
    cpu.step(); CHECK(0x0100 == cpu.state.SP);  CHECK(75 == cpu.state.T);
    cpu.step(); CHECK(0xEF00 == cpu.mem.r16(0x00FE)); CHECK(0x00FE == cpu.state.SP); CHECK(86 == cpu.state.T);
    cpu.step(); CHECK(0x1234 == cpu.mem.r16(0x00FC)); CHECK(0x00FC == cpu.state.SP); CHECK(97 == cpu.state.T);
    cpu.step(); CHECK(0x5678 == cpu.mem.r16(0x00FA)); CHECK(0x00FA == cpu.state.SP); CHECK(108 == cpu.state.T);
    cpu.step(); CHECK(0x9ABC == cpu.mem.r16(0x00F8)); CHECK(0x00F8 == cpu.state.SP); CHECK(119 == cpu.state.T);
    cpu.step(); CHECK(0x2345 == cpu.mem.r16(0x00F6)); CHECK(0x00F6 == cpu.state.SP); CHECK(134 == cpu.state.T);
    cpu.step(); CHECK(0x6789 == cpu.mem.r16(0x00F4)); CHECK(0x00F4 == cpu.state.SP); CHECK(149 == cpu.state.T);
    cpu.step(); CHECK(0x6789 == cpu.state.AF); CHECK(0x00F6 == cpu.state.SP); CHECK(159 == cpu.state.T);
    cpu.step(); CHECK(0x2345 == cpu.state.BC); CHECK(0x00F8 == cpu.state.SP); CHECK(169 == cpu.state.T);
    cpu.step(); CHECK(0x9ABC == cpu.state.DE); CHECK(0x00FA == cpu.state.SP); CHECK(179 == cpu.state.T);
    cpu.step(); CHECK(0x5678 == cpu.state.HL); CHECK(0x00FC == cpu.state.SP); CHECK(189 == cpu.state.T);
    cpu.step(); CHECK(0x1234 == cpu.state.IX); CHECK(0x00FE == cpu.state.SP); CHECK(203 == cpu.state.T);
    cpu.step(); CHECK(0xEF00 == cpu.state.IY); CHECK(0x0100 == cpu.state.SP); CHECK(217 == cpu.state.T);
}

// EX DE,HL
// EX AF,AF'
// EXX
// EX (SP),HL
// EX (SP),IX
// EX (SP),IY
TEST(EX) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x21, 0x34, 0x12,       // LD HL,0x1234
        0x11, 0x78, 0x56,       // LD DE,0x5678
        0xEB,                   // EX DE,HL
        0x3E, 0x11,             // LD A,0x11
        0x08,                   // EX AF,AF'
        0x3E, 0x22,             // LD A,0x22
        0x08,                   // EX AF,AF'
        0x01, 0xBC, 0x9A,       // LD BC,0x9ABC
        0xD9,                   // EXX
        0x21, 0x11, 0x11,       // LD HL,0x1111
        0x11, 0x22, 0x22,       // LD DE,0x2222
        0x01, 0x33, 0x33,       // LD BC,0x3333
        0xD9,                   // EXX
        0x31, 0x00, 0x01,       // LD SP,0x0100
        0xD5,                   // PUSH DE
        0xE3,                   // EX (SP),HL
        0xDD, 0x21, 0x99, 0x88, // LD IX,0x8899
        0xDD, 0xE3,             // EX (SP),IX
        0xFD, 0x21, 0x77, 0x66, // LD IY,0x6677
        0xFD, 0xE3,             // EX (SP),IY
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1234 == cpu.state.HL); CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x5678 == cpu.state.DE); CHECK(20 == cpu.state.T);
    cpu.step(); CHECK(0x1234 == cpu.state.DE); CHECK(0x5678 == cpu.state.HL); CHECK(24 == cpu.state.T);
    cpu.step(); CHECK(0x1100 == cpu.state.AF); CHECK(0x0000 == cpu.state.AF_); CHECK(31 == cpu.state.T);
    cpu.step(); CHECK(0x0000 == cpu.state.AF); CHECK(0x1100 == cpu.state.AF_); CHECK(35 == cpu.state.T);
    cpu.step(); CHECK(0x2200 == cpu.state.AF); CHECK(0x1100 == cpu.state.AF_); CHECK(42 == cpu.state.T);
    cpu.step(); CHECK(0x1100 == cpu.state.AF); CHECK(0x2200 == cpu.state.AF_); CHECK(46 == cpu.state.T);
    cpu.step(); CHECK(0x9ABC == cpu.state.BC); CHECK(56 == cpu.state.T);
    cpu.step();
    CHECK(0x0000 == cpu.state.HL); CHECK(0x5678 == cpu.state.HL_);
    CHECK(0x0000 == cpu.state.DE); CHECK(0x1234 == cpu.state.DE_);
    CHECK(0x0000 == cpu.state.BC); CHECK(0x9ABC == cpu.state.BC_);
    CHECK(60 == cpu.state.T);
    cpu.step(); CHECK(0x1111 == cpu.state.HL); CHECK(70 == cpu.state.T);
    cpu.step(); CHECK(0x2222 == cpu.state.DE); CHECK(80 == cpu.state.T);
    cpu.step(); CHECK(0x3333 == cpu.state.BC); CHECK(90 == cpu.state.T);
    cpu.step();
    CHECK(0x5678 == cpu.state.HL); CHECK(0x1111 == cpu.state.HL_);
    CHECK(0x1234 == cpu.state.DE); CHECK(0x2222 == cpu.state.DE_);
    CHECK(0x9ABC == cpu.state.BC); CHECK(0x3333 == cpu.state.BC_);
    CHECK(94 == cpu.state.T);
    cpu.step(); CHECK(0x0100 == cpu.state.SP); CHECK(104 == cpu.state.T);
    cpu.step(); CHECK(0x1234 == cpu.mem.r16(0x00FE)); CHECK(115 == cpu.state.T);
    cpu.step(); CHECK(0x1234 == cpu.state.HL); CHECK(0x5678 == cpu.mem.r16(0x00FE)); CHECK(134 == cpu.state.T);
    cpu.step(); CHECK(0x8899 == cpu.state.IX); CHECK(148 == cpu.state.T);
    cpu.step(); CHECK(0x5678 == cpu.state.IX); CHECK(0x8899 == cpu.mem.r16(0x00FE)); CHECK(171 == cpu.state.T);
    cpu.step(); CHECK(0x6677 == cpu.state.IY); CHECK(185 == cpu.state.T);
    cpu.step(); CHECK(0x8899 == cpu.state.IY); CHECK(0x6677 == cpu.mem.r16(0x00FE)); CHECK(208 == cpu.state.T);
}

// ADD A,r
// ADD A,n
TEST(ADD_A_r) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x3E, 0x0F,     // LD A,0x0F
        0x87,           // ADD A,A
        0x06, 0xE0,     // LD B,0xE0
        0x80,           // ADD A,B
        0x3E, 0x81,     // LD A,0x81
        0x0E, 0x80,     // LD C,0x80
        0x81,           // ADD A,C
        0x16, 0xFF,     // LD D,0xFF
        0x82,           // ADD A,D
        0x1E, 0x40,     // LD E,0x40
        0x83,           // ADD A,E
        0x26, 0x80,     // LD H,0x80
        0x84,           // ADD A,H
        0x2E, 0x33,     // LD L,0x33
        0x85,           // ADD A,L
        0xC6, 0x44,     // ADD A,0x44

    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x0F == cpu.state.A); CHECK(cpu.test_flags(0));       CHECK(7 == cpu.state.T);
    cpu.step(); CHECK(0x1E == cpu.state.A); CHECK(cpu.test_flags(z80::HF)); CHECK(11 == cpu.state.T);
    cpu.step(); CHECK(0xE0 == cpu.state.B);                                 CHECK(18 == cpu.state.T);
    cpu.step(); CHECK(0xFE == cpu.state.A); CHECK(cpu.test_flags(z80::SF)); CHECK(22 == cpu.state.T);
    cpu.step(); CHECK(0x81 == cpu.state.A);                                 CHECK(29 == cpu.state.T);
    cpu.step(); CHECK(0x80 == cpu.state.C);                                 CHECK(36 == cpu.state.T);
    cpu.step(); CHECK(0x01 == cpu.state.A); CHECK(cpu.test_flags(z80::VF|z80::CF)); CHECK(40 == cpu.state.T);
    cpu.step(); CHECK(0xFF == cpu.state.D);                                 CHECK(47 == cpu.state.T);
    cpu.step(); CHECK(0x00 == cpu.state.A); CHECK(cpu.test_flags(z80::ZF|z80::HF|z80::CF)); CHECK(51 == cpu.state.T);
    cpu.step(); CHECK(0x40 == cpu.state.E);                                 CHECK(58 == cpu.state.T);
    cpu.step(); CHECK(0x40 == cpu.state.A); CHECK(cpu.test_flags(0));       CHECK(62 == cpu.state.T);
    cpu.step(); CHECK(0x80 == cpu.state.H);                                 CHECK(69 == cpu.state.T);
    cpu.step(); CHECK(0xC0 == cpu.state.A); CHECK(cpu.test_flags(z80::SF)); CHECK(73 == cpu.state.T);
    cpu.step(); CHECK(0x33 == cpu.state.L);                                 CHECK(80 == cpu.state.T);
    cpu.step(); CHECK(0xF3 == cpu.state.A); CHECK(cpu.test_flags(z80::SF)); CHECK(84 == cpu.state.T);
    cpu.step(); CHECK(0x37 == cpu.state.A); CHECK(cpu.test_flags(z80::CF)); CHECK(91 == cpu.state.T);
}

// ADD A,(HL)
// ADD A,(IX+d)
// ADD A,(IY+d)
TEST(ADD_a_iHLIXY_d) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte data[] = { 0x41, 0x61, 0x81 };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10, // LD IX,0x1000
        0xFD, 0x21, 0x03, 0x10, // LD IY,0x1003
        0x3E, 0x00,             // LD A,0x00
        0x86,                   // ADD A,(HL)
        0xDD, 0x86, 0x01,       // ADD A,(IX+1)
        0xFD, 0x86, 0xFF,       // ADD A,(IY-1)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1000 == cpu.state.HL); CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x1000 == cpu.state.IX); CHECK(24 == cpu.state.T);
    cpu.step(); CHECK(0x1003 == cpu.state.IY); CHECK(38 == cpu.state.T);
    cpu.step(); CHECK(0x00 == cpu.state.A); CHECK(45 == cpu.state.T);
    cpu.step(); CHECK(0x41 == cpu.state.A); CHECK(cpu.test_flags(0)); CHECK(52 == cpu.state.T);
    cpu.step(); CHECK(0xA2 == cpu.state.A); CHECK(cpu.test_flags(z80::SF|z80::VF)); CHECK(71 == cpu.state.T);
    cpu.step(); CHECK(0x23 == cpu.state.A); CHECK(cpu.test_flags(z80::VF|z80::CF)); CHECK(90 == cpu.state.T);
}

// ADC A,r
// ADC A,n
TEST(ADC_a_r) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte prog[] = {
        0x3E, 0x00,         // LD A,0x00
        0x06, 0x41,         // LD B,0x41
        0x0E, 0x61,         // LD C,0x61
        0x16, 0x81,         // LD D,0x81
        0x1E, 0x41,         // LD E,0x41
        0x26, 0x61,         // LD H,0x61
        0x2E, 0x81,         // LD L,0x81
        0x8F,               // ADC A,A
        0x88,               // ADC A,B
        0x89,               // ADC A,C
        0x8A,               // ADC A,D
        0x8B,               // ADC A,E
        0x8C,               // ADC A,H
        0x8D,               // ADC A,L
        0xCE, 0x01,         // ADC A,1
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x00 == cpu.state.A); CHECK(7 == cpu.state.T);
    cpu.step(); CHECK(0x41 == cpu.state.B); CHECK(14 == cpu.state.T);
    cpu.step(); CHECK(0x61 == cpu.state.C); CHECK(21 == cpu.state.T);
    cpu.step(); CHECK(0x81 == cpu.state.D); CHECK(28 == cpu.state.T);
    cpu.step(); CHECK(0x41 == cpu.state.E); CHECK(35 == cpu.state.T);
    cpu.step(); CHECK(0x61 == cpu.state.H); CHECK(42 == cpu.state.T);
    cpu.step(); CHECK(0x81 == cpu.state.L); CHECK(49 == cpu.state.T);
    cpu.step(); CHECK(0x00 == cpu.state.A); CHECK(cpu.test_flags(z80::ZF)); CHECK(53 == cpu.state.T);
    cpu.step(); CHECK(0x41 == cpu.state.A); CHECK(cpu.test_flags(0)); CHECK(57 == cpu.state.T);
    cpu.step(); CHECK(0xA2 == cpu.state.A); CHECK(cpu.test_flags(z80::SF|z80::VF)); CHECK(61 == cpu.state.T);
    cpu.step(); CHECK(0x23 == cpu.state.A); CHECK(cpu.test_flags(z80::VF|z80::CF)); CHECK(65 == cpu.state.T);
    cpu.step(); CHECK(0x65 == cpu.state.A); CHECK(cpu.test_flags(0)); CHECK(69 == cpu.state.T);
    cpu.step(); CHECK(0xC6 == cpu.state.A); CHECK(cpu.test_flags(z80::SF|z80::VF)); CHECK(73 == cpu.state.T);
    cpu.step(); CHECK(0x47 == cpu.state.A); CHECK(cpu.test_flags(z80::VF|z80::CF)); CHECK(77 == cpu.state.T);
    cpu.step(); CHECK(0x49 == cpu.state.A); CHECK(cpu.test_flags(0)); CHECK(84 == cpu.state.T);
}

// ADC A,(HL)
// ADC A,(IX+d)
// ADC A,(IY+d)
TEST(ADC_a_iHLIXY_d) {
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    ubyte data[] = { 0x41, 0x61, 0x81, 0x2 };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10, // LD IX,0x1000
        0xFD, 0x21, 0x03, 0x10, // LD IY,0x1003
        0x3E, 0x00,             // LD A,0x00
        0x86,                   // ADD A,(HL)
        0xDD, 0x8E, 0x01,       // ADC A,(IX+1)
        0xFD, 0x8E, 0xFF,       // ADC A,(IY-1)
        0xDD, 0x8E, 0x03,       // ADC A,(IX+3)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.step(); CHECK(0x1000 == cpu.state.HL); CHECK(10 == cpu.state.T);
    cpu.step(); CHECK(0x1000 == cpu.state.IX); CHECK(24 == cpu.state.T);
    cpu.step(); CHECK(0x1003 == cpu.state.IY); CHECK(38 == cpu.state.T);
    cpu.step(); CHECK(0x00 == cpu.state.A); CHECK(45 == cpu.state.T);
    cpu.step(); CHECK(0x41 == cpu.state.A); CHECK(cpu.test_flags(0)); CHECK(52 == cpu.state.T);
    cpu.step(); CHECK(0xA2 == cpu.state.A); CHECK(cpu.test_flags(z80::SF|z80::VF)); CHECK(71 == cpu.state.T);
    cpu.step(); CHECK(0x23 == cpu.state.A); CHECK(cpu.test_flags(z80::VF|z80::CF)); CHECK(90 == cpu.state.T);
    cpu.step(); CHECK(0x26 == cpu.state.A); CHECK(cpu.test_flags(0)); CHECK(109 == cpu.state.T);
}

TEST(cpu) {

    // setup CPU with a 16 kByte RAM bank at 0x0000
    z80 cpu;
    ubyte ram0[memory::bank::size] = { 0 };
    cpu.mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    // check initial reset
    cpu.reset();
    CHECK(0 == cpu.state.PC);
    CHECK(0 == cpu.state.IM);
    CHECK(!cpu.state.IFF1);
    CHECK(!cpu.state.IFF2);
    CHECK(0 == cpu.state.I);
    CHECK(0 == cpu.state.R);

    /// run a few nops and check PC and T state counter
    cpu.step();
    CHECK(1 == cpu.state.PC);
    CHECK(4 == cpu.state.T);
    cpu.step();
    CHECK(2 == cpu.state.PC);
    CHECK(8 == cpu.state.T);

    /// LD BC,0x1234
    const ubyte f = cpu.state.F;
    ram0[2] = 0x01; ram0[3] = 0x34; ram0[4] = 0x12;
    cpu.step();
    CHECK(5 == cpu.state.PC);
    CHECK(18 == cpu.state.T);
    CHECK(0x1234 == cpu.state.BC);
    CHECK(0x12 == cpu.state.B);
    CHECK(0x34 == cpu.state.C);
    CHECK(f == cpu.state.F);        // does not modify flags
}

