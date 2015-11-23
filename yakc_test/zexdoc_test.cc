//------------------------------------------------------------------------------
//  zexdoc_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc_core/z80.h"
#include "yakc_test/zex.h"

/*
This runs Frank Cringle's zexdoc test through the Z80 emulator. These are
CP/M .COM executables, and we provide the minimal CP/M environment to
make these work.
*/

using namespace yakc;

static ubyte ram[0x10000];

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
        putc(cpu.state.E, stdout);
    }
    else if (9 == cpu.state.C) {
        // output a string
        ubyte c;
        uword addr = cpu.state.DE;
        while ((c = cpu.mem.r8(addr++)) != '$') {
            putc(c, stdout);
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

    // initialize the z80 cpu
    z80 cpu;
    memset(ram, 0, sizeof(ram));
    cpu.mem.map(0, 0x0000, sizeof(ram), ram, true);
    cpu.init(in_func, out_func, nullptr);
    cpu.state.SP = 0xF000;  // no idea where the stack is located in CP/M
    cpu.state.PC = 0x0100;  // execution starts at 0x0100

    // load the test program dump at 0x100
    cpu.mem.write(0x0100, dump_zexdoc, sizeof(dump_zexdoc));

    bool running = true;
    int t = 0;
    while (running) {
        t += cpu.step();
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
            printf("IT'S A TRAP!\n");
            running = false;
        }
    }
}

