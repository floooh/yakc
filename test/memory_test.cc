//------------------------------------------------------------------------------
//  mem_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/memory.h"

using namespace yakc;

TEST(memory) {

    memory mem;
    CHECK(mem.read_ubyte(0x0000) == 0);
    CHECK(mem.read_ubyte(0xFFFF) == 0);
    CHECK(mem.read_ubyte(0x4567) == 0);
    CHECK(mem.read_uword(0x0000) == 0);
    CHECK(mem.read_uword(0XFFFF) == 0);

    // 2 ram banks and one rom bank
    ubyte ram0[memory::bank::size];
    ubyte ram1[memory::bank::size];
    ubyte rom[memory::bank::size];
    std::memset(ram0, 1, sizeof(ram0));
    std::memset(ram1, 2, sizeof(ram1));
    std::memset(rom, 3, sizeof(rom));

    mem.map(0, ram0, sizeof(ram0), memory::type::ram);
    mem.map(1, ram1, sizeof(ram0), memory::type::ram);
    mem.map(3, rom, sizeof(rom), memory::type::rom);

    CHECK(mem.read_ubyte(0x0000) == 1);
    CHECK(mem.read_ubyte(0x0001) == 1);
    CHECK(mem.read_ubyte(0x1000) == 1);
    CHECK(mem.read_ubyte(0x3FFF) == 1);
    CHECK(mem.read_ubyte(0x4000) == 2);
    CHECK(mem.read_ubyte(0x4100) == 2);
    CHECK(mem.read_ubyte(0x7FFF) == 2);
    CHECK(mem.read_ubyte(0x8000) == 0); // 0x8000 .. 0xBFFF unmapped!
    CHECK(mem.read_ubyte(0xBFFF) == 0);
    CHECK(mem.read_ubyte(0xC000) == 3); // rom
    CHECK(mem.read_ubyte(0xC002) == 3);
    CHECK(mem.read_ubyte(0xFFFF) == 3);

    CHECK(mem.read_uword(0x0000) == 0x0101);
    CHECK(mem.read_uword(0x0001) == 0x0101);
    CHECK(mem.read_uword(0x3FFF) == 0x0201);
    CHECK(mem.read_uword(0x4000) == 0x0202);
    CHECK(mem.read_uword(0x4101) == 0x0202);
    CHECK(mem.read_uword(0x7FFF) == 0x0002);
    CHECK(mem.read_uword(0x8000) == 0x0000);
    CHECK(mem.read_uword(0x8203) == 0x0000);
    CHECK(mem.read_uword(0xBFFE) == 0x0000);
    CHECK(mem.read_uword(0xBFFF) == 0x0300);

    CHECK(mem.read_uword(0xC000) == 0x0303);
    CHECK(mem.read_uword(0xDEF0) == 0x0303);
    CHECK(mem.read_uword(0xFFFF) == 0x0103);
}

