//------------------------------------------------------------------------------
//  nestest_test.cc
//
//  Run the nestest.nes 6502 tester.
//
//  http://www.qmtpro.com/~nes/misc/nestest.log
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/devices/mos6502.h"
#include "yakc/core/system_bus.h"
#include "test/dump.h"
#include "test/nestestlog.h"

using namespace YAKC;

static uint8_t mem_cb(bool write, uint16_t addr, uint8_t inval) {
    // FIXME
    return 0x00;
}

static ubyte ram[0x0800];
static ubyte sram[0x2000];

// parses the nestest dump and loads it into the emulators memory
struct nes_header {
    uint8_t magic_N, magic_E, magic_S, magic_1A;
    uint8_t num_rom = 0;
    uint8_t num_chr = 0;
    uint8_t flags6 = 0;
    uint8_t flags7 = 0;
    uint8_t num_ram = 0;
    uint8_t flags9 = 0;
    uint8_t flags10 = 0;
    uint8_t zero[5];
};
static_assert(sizeof(nes_header) == 16, "sizeof(nes_header)");

TEST(nestest) {
    system_bus bus;
    mos6502 cpu;

    // need to implement a minimal NES emulation here
    memset(ram, 0, sizeof(ram));
    memset(sram, 0, sizeof(sram));

    // RAM 0..7FF are repeated until 1FFF
    cpu.mem.map(0, 0x0000, sizeof(ram), ram, true);
    cpu.mem.map(0, 0x0800, sizeof(ram), ram, true);
    cpu.mem.map(0, 0x1000, sizeof(ram), ram, true);
    cpu.mem.map(0, 0x1800, sizeof(ram), ram, true);

    // memory-mapped-IO (2000..401F), but we 1KB page size...
    cpu.mem.map_io(0, 0x2000, 0x4400, mem_cb);

    // SRAM 0x6000..0x8000
    cpu.mem.map(0, 0x6000, sizeof(sram), sram, true);

    // map nestest rom (one 16KB bank repeated at 0x8000 and 0xC000)
    // nestest file format has a 16 byte header
    cpu.mem.map(0, 0x8000, 0x4000, &(dump_nestest[16]), false);
    cpu.mem.map(0, 0xC000, 0x4000, &(dump_nestest[16]), false);

    // initialize the CPU
    cpu.init(&bus);
    cpu.reset();
    cpu.bcd_enabled = false;
    cpu.PC = 0xC000;

    // run the test
    int i = 0;
    while (cpu.PC != 0xC66E) {
        // compare the CPU state with the nestestlog data
        const auto& state = state_table[i++];
        if ((cpu.PC != state.PC) ||
            (cpu.A  != state.A) ||
            (cpu.X  != state.X) ||
            (cpu.Y  != state.Y) ||
            (cpu.P  != state.P) ||
            (cpu.S  != state.S))
        {
            printf("### NESTEST failed at op %d (PC=0x%04X)!\n", i, cpu.PC);
            CHECK(cpu.PC == state.PC);
            CHECK(cpu.A == state.A);
            CHECK(cpu.X == state.X);
            CHECK(cpu.Y == state.Y);
            CHECK(cpu.P == state.P);
            CHECK(cpu.S == state.S);
            return;
        }
        // and run the next instruction
        cpu.step();
    }
}
