//------------------------------------------------------------------------------
//  zexdoc_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/z80.h"
#include "yakc/system_bus.h"
#include "test/zex.h"
#include "Core/Time/Clock.h"
#include <string.h>
#include <inttypes.h>

/*
This runs Frank Cringle's zexdoc test through the Z80 emulator. These are
CP/M .COM executables, and we provide the minimal CP/M environment to
make these work.
*/

using namespace YAKC;
using namespace Oryol;

static ubyte ram[0x10000];

static const int output_size = 1<<16;
static int out_pos = 0;
static unsigned char output[output_size];

static void put_char(char c) {
    if (out_pos < output_size) {
        output[out_pos++] = c;
    }
    printf("%c", c);
}

static bool cpm_bdos(z80& cpu) {

    bool retval = true;
    if (2 == cpu.C) {
        // output a character
        put_char(cpu.E);
    }
    else if (9 == cpu.C) {
        // output a string
        ubyte c;
        uword addr = cpu.DE;
        while ((c = cpu.mem.r8(addr++)) != '$') {
            put_char(c);
        }
    }
    else {
        printf("Unknown CP/M call %d!\n", cpu.C);
        retval = false;
    }
    // emulate a RET
    cpu.PC = cpu.mem.r16(cpu.SP);
    cpu.SP += 2;

    return retval;
}

// runs the cpu through a previously configured test (zexdoc or zexall)
static void run_test(z80& cpu, system_bus& bus, const char* name) {
    auto startTime = Clock::Now();
    bool running = true;
    std::uint64_t t = 0;
    std::uint64_t num = 0;
    while (running) {
        t += cpu.step(&bus);
        num++;
        // check for bdos call and trap
        if (cpu.INV) {
            printf("INVALID OPCODE HIT (%02X %02X %02X %02X)\n",
                cpu.mem.r8(cpu.PC),
                cpu.mem.r8(cpu.PC+1),
                cpu.mem.r8(cpu.PC+2),
                cpu.mem.r8(cpu.PC+3));
            bool invalid_opcode = false;
            CHECK(invalid_opcode);
            running = false;
        }
        else if (cpu.PC == 5) {
            if (!cpm_bdos(cpu)) {
                bool invalid_bdos_call = false;
                CHECK(invalid_bdos_call);
                running = false;
            }
        }
        else if (cpu.PC == 0) {
            running = false;
        }
    }
    double dur = Clock::Since(startTime).AsSeconds();
    printf("\n%s: %llu cycles, %llu ops in %.3fsecs (%.2f MHz / %.2f MIPS)\n", name, t, num, dur, (t/dur)/1000000.0,(num/dur)/1000000.0);

    // did an error occur?
    output[output_size-1] = 0;
    if (strstr((const char*)output, "ERROR")!=nullptr) {
        bool zex_failed = false;
        CHECK(zex_failed);
    }
    else {
        printf("\n\nALL %s TESTS PASSED!\n", name);
    }
}

//------------------------------------------------------------------------------
TEST(zexdoc) {

    memset(output, 0, sizeof(output));
    system_bus bus;
    z80 cpu;
    memset(ram, 0, sizeof(ram));
    cpu.mem.map(0, 0x0000, sizeof(ram), ram, true);
    cpu.init();
    cpu.SP = 0xF000;  // no idea where the stack is located in CP/M
    cpu.PC = 0x0100;  // execution starts at 0x0100
    cpu.mem.write(0x0100, dump_zexdoc, sizeof(dump_zexdoc));
    run_test(cpu, bus, "ZEXDOC");
}

//------------------------------------------------------------------------------
TEST(zexall) {

    memset(output, 0, sizeof(output));
    system_bus bus;
    z80 cpu;
    memset(ram, 0, sizeof(ram));
    cpu.mem.map(0, 0x0000, sizeof(ram), ram, true);
    cpu.init();
    cpu.SP = 0xF000;  // no idea where the stack is located in CP/M
    cpu.PC = 0x0100;  // execution starts at 0x0100
    cpu.mem.write(0x0100, dump_zexall, sizeof(dump_zexall));
    run_test(cpu, bus, "ZEXALL");
}
