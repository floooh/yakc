//------------------------------------------------------------------------------
//  cpu_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/z80.h"

using namespace yakc;

// test the LD r,s opcodes
TEST(LD_r_s) {
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

