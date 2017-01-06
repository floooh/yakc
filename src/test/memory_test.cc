//------------------------------------------------------------------------------
//  mem_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/core/memory.h"
#include <string.h>

using namespace YAKC;

TEST(memory) {

    memory mem;
    CHECK(mem.r8(0x0000) == 0xFF);
    CHECK(mem.r8(0xFFFF) == 0xFF);
    CHECK(mem.r8(0x4567) == 0xFF);
    CHECK(mem.r8(0x0000) == 0xFF);
    CHECK(mem.r8(0XFFFF) == 0xFF);

    // 2 ram banks and one rom bank
    const int size = 0x4000;
    ubyte ram0[size];
    ubyte ram1[size];
    ubyte rom[size];
    memset(ram0, 1, sizeof(ram0));
    memset(ram1, 2, sizeof(ram1));
    memset(rom, 3, sizeof(rom));

    mem.map(0, 0x0000, size, ram0, true);
    mem.map(0, 0x4000, size, ram1, true);
    mem.map(0, 0xC000, size, rom, false);

    CHECK(mem.r8(0x0000) == 1);
    CHECK(mem.r8(0x0001) == 1);
    CHECK(mem.r8(0x1000) == 1);
    CHECK(mem.r8(0x3FFF) == 1);
    CHECK(mem.r8(0x4000) == 2);
    CHECK(mem.r8(0x4100) == 2);
    CHECK(mem.r8(0x7FFF) == 2);
    CHECK(mem.r8(0x8000) == 0xFF); // 0x8000 .. 0xBFFF unmapped!
    CHECK(mem.r8(0xBFFF) == 0xFF);
    CHECK(mem.r8(0xC000) == 3); // rom
    CHECK(mem.r8(0xC002) == 3);
    CHECK(mem.r8(0xFFFF) == 3);
    CHECK(mem.r16(0x0000) == 0x0101);
    CHECK(mem.r16(0x0001) == 0x0101);
    CHECK(mem.r16(0x3FFF) == 0x0201);
    CHECK(mem.r16(0x4000) == 0x0202);
    CHECK(mem.r16(0x4101) == 0x0202);
    CHECK(mem.r16(0x7FFF) == 0xFF02);
    CHECK(mem.r16(0x8000) == 0xFFFF);
    CHECK(mem.r16(0x8203) == 0xFFFF);
    CHECK(mem.r16(0xBFFE) == 0xFFFF);
    CHECK(mem.r16(0xBFFF) == 0x03FF);
    CHECK(mem.r16(0xC000) == 0x0303);
    CHECK(mem.r16(0xDEF0) == 0x0303);
    CHECK(mem.r16(0xFFFF) == 0x0103);

    mem.w8(0x0000, 5);
    CHECK(mem.r8(0x0000) == 5);
    CHECK(mem.r16(0x0000) == 0x0105);
    CHECK(mem.r16(0xFFFF) == 0x0503);
    mem.w8(0x0001, 6);
    CHECK(mem.r8(0x0001) == 6);
    CHECK(mem.r16(0x0000) == 0x0605);
    mem.w16(0x0002, 0x0708);
    CHECK(mem.r16(0x0002) == 0x0708);
    CHECK(mem.r8(0x0002) == 8);
    CHECK(mem.r8(0x0003) == 7);
    mem.w16(0x3FFF, 0xE10A);
    CHECK(mem.r16(0x3FFF) == 0xE10A);
    CHECK(mem.r8(0x3FFF) == 0x0A);
    CHECK(mem.r8(0x4000) == 0xE1);

    // try writing to (partially) unmapped memory
    mem.w16(0x7FFF, 0xAABB);
    CHECK(mem.r16(0x7FFF) == 0xFFBB);
    CHECK(mem.r8(0x7FFF) == 0xBB);
    CHECK(mem.r8(0x8000) == 0xFF);
    mem.w16(0x8100, 0x1234);
    CHECK(mem.r16(0x8100) == 0xFFFF);
    mem.w8(0x8050, 7);
    CHECK(mem.r8(0x8050) == 0xFF);

    // try writing to ROM
    mem.w8(0xC000, 0x21);
    CHECK(mem.r8(0xC000) == 3);
    mem.w16(0xBFFF, 0x3456);
    CHECK(mem.r16(0xBFFF) == 0x03FF);
    CHECK(mem.r8(0xBFFF) == 0xFF);
    CHECK(mem.r8(0xC000) == 0x03);
    mem.w8(0xFFFF, 34);
    CHECK(mem.r8(0xFFFF) == 3);

    // write a range of bytes
    ubyte bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    mem.write(0, bytes, sizeof(bytes));
    CHECK(mem.r8(0x0000) == 1);
    CHECK(mem.r8(0x0001) == 2);
    CHECK(mem.r8(0x0002) == 3);
    CHECK(mem.r8(0x0003) == 4);
    CHECK(mem.r8(0x0004) == 5);
    CHECK(mem.r8(0x0005) == 6);
    CHECK(mem.r8(0x0006) == 7);
    CHECK(mem.r8(0x0007) == 8);

}

