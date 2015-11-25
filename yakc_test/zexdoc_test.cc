//------------------------------------------------------------------------------
//  zexdoc_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc_core/z80.h"
#include "yakc_test/zex.h"
#include "Time/Clock.h"
#include <string.h>
#include <inttypes.h>

/*
This runs Frank Cringle's zexdoc test through the Z80 emulator. These are
CP/M .COM executables, and we provide the minimal CP/M environment to
make these work.
*/

using namespace yakc;
using namespace Oryol;

static ubyte ram[0x10000];

static const int output_size = 1<<16;
static int out_pos = 0;
static unsigned char output[output_size];

static void put_char(char c) {
    if (out_pos < output_size) {
        output[out_pos++] = c;
    }
    putc(c, stdout);
    fflush(stdout);
}

static ubyte in_func(void* userdata, uword port) {
    return 0;
}

static void out_func(void* userdata, uword port, ubyte val) {
    // empty
}

static bool cpm_bdos(z80& cpu) {

    bool retval = true;
    if (2 == cpu.state.C) {
        // output a character
        put_char(cpu.state.E);
    }
    else if (9 == cpu.state.C) {
        // output a string
        ubyte c;
        uword addr = cpu.state.DE;
        while ((c = cpu.mem.r8(addr++)) != '$') {
            put_char(c);
        }
    }
    else {
        printf("Unknown CP/M call %d!\n", cpu.state.C);
        retval = false;
    }
    // emulate a RET
    cpu.state.PC = cpu.mem.r16(cpu.state.SP);
    cpu.state.SP += 2;

    return retval;
}

TEST(zexdoc) {

    memset(output, 0, sizeof(output));

    // initialize the z80 cpu
    z80 cpu;
    memset(ram, 0, sizeof(ram));
    cpu.mem.map(0, 0x0000, sizeof(ram), ram, true);
    cpu.init(in_func, out_func, nullptr);
    cpu.state.SP = 0xF000;  // no idea where the stack is located in CP/M
    cpu.state.PC = 0x0100;  // execution starts at 0x0100

    // load the test program dump at 0x100
    cpu.mem.write(0x0100, dump_zexdoc, sizeof(dump_zexdoc));

    auto startTime = Clock::Now();
    bool running = true;
    std::uint64_t t = 0;
    std::uint64_t num = 0;
    while (running) {
        t += cpu.step();
        num++;
        // check for bdos call and trap
        if (cpu.state.INV) {
            printf("INVALID OPCODE HIT (%02X %02X %02X %02X)\n",
                cpu.mem.r8(cpu.state.PC),
                cpu.mem.r8(cpu.state.PC+1),
                cpu.mem.r8(cpu.state.PC+2),
                cpu.mem.r8(cpu.state.PC+3));
            bool invalid_opcode = false;
            CHECK(invalid_opcode);
            running = false;
        }
        else if (cpu.state.PC == 5) {
            if (!cpm_bdos(cpu)) {
                bool invalid_bdos_call = false;
                CHECK(invalid_bdos_call);
                running = false;
            }
        }
        else if (cpu.state.PC == 0) {
            running = false;
        }
    }
    float64 dur = Clock::Since(startTime).AsSeconds();
    printf("\n%llu cycles, %llu ops in %.3fsecs (%.2f MHz / %.2f MIPS)\n", t, num, dur, (t/dur)/1000000.0,(num/dur)/1000000.0);

    // did an error occur?
    output[output_size-1] = 0;
    if (strstr((const char*)output, "ERROR")!=nullptr) {
        bool zexdoc_failed = false;
        CHECK(zexdoc_failed);
    }
    else {
        printf("\n\nALL ZEXDOC TESTS PASSED!\n");
    }
}

