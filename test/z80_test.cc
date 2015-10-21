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
    CHECK(0 == cpu.state.pc);
    CHECK(0 == cpu.state.im);
    CHECK(!cpu.state.iff1);
    CHECK(!cpu.state.iff2);
    CHECK(0 == cpu.state.i);
    CHECK(0 == cpu.state.r);

    /// run a few nops and check PC and T state counter
    cpu.exec();
    CHECK(1 == cpu.state.pc);
    CHECK(4 == cpu.state.t);
    cpu.exec();
    CHECK(2 == cpu.state.pc);
    CHECK(8 == cpu.state.t);

    /// LD BC,0x1234
    const ubyte f = cpu.state.f;
    ram0[2] = 0x01; ram0[3] = 0x34; ram0[4] = 0x12;
    cpu.exec();
    CHECK(5 == cpu.state.pc);
    CHECK(18 == cpu.state.t);
    CHECK(0x1234 == cpu.state.bc);
    CHECK(0x12 == cpu.state.b);
    CHECK(0x34 == cpu.state.c);
    CHECK(f == cpu.state.f);        // does not modify flags
}

