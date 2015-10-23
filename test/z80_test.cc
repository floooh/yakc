//------------------------------------------------------------------------------
//  cpu_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/z80.h"

using namespace yakc;

TEST(cpu) {

    // setup CPU with a 16 kByte RAM bank at 0x0000
    ubyte ram0[memory::bank::size] = { 0 };
    memory mem;
    mem.map(0, ram0, sizeof(ram0), memory::type::ram);

    z80 cpu;
    cpu.init(&mem);

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

