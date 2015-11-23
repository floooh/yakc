//------------------------------------------------------------------------------
//  daisychain_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc_core/z80.h"
#include "yakc_core/z80int.h"

using namespace yakc;

static ubyte ram[0x4000];

static void step(z80& cpu) {
    cpu.step();
    cpu.handle_irq();
}

TEST(daisychain) {

    z80 cpu;
    memset(ram, 0, sizeof(ram));
    cpu.mem.map(0, 0x0000, sizeof(ram), ram, true);
    cpu.state.IM = 0x02;
    cpu.state.I = 0x01;

    ubyte prog[] = {
        0xFB,               // EI
        0x31, 0x00, 0x01,   // LD SP,0x0100
        0x00, 0x00, 0x00,   // 3x NOP
        0xC9,               // RET
        0xFB,               // EI
        0x00,               // NOP
        0xED, 0x4D,         // RETI
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));
    cpu.mem.w16(0x1E8, 0x0008);

    // interrupt controller devices
    z80int dev0;
    z80int dev1;
    z80int dev2;

    // connect interrupt controllers to INT pin of CPU
    dev0.connect_cpu(z80::irq, &cpu);
    dev1.connect_cpu(z80::irq, &cpu);
    dev2.connect_cpu(z80::irq, &cpu);

    // build the daisy chain
    cpu.connect_irq_device(&dev0);
    dev0.connect_irq_device(&dev1);
    dev1.connect_irq_device(&dev2);

    // step the cpu a few times, and request an interrupt on the highest priority device

    // EI
    step(cpu);
    CHECK(cpu.enable_interrupt);
    CHECK(!cpu.state.IFF1);
    CHECK(!cpu.state.IFF2);

    // LD SP,0x0100, and delayed interrupt-enable from EI
    step(cpu);
    CHECK(cpu.state.SP == 0x0100);
    CHECK(!cpu.enable_interrupt);
    CHECK(cpu.state.IFF1);
    CHECK(cpu.state.IFF2);

    // request interrupt, execute nop, acknowledge interrupt
    dev0.request_interrupt(0xE8);
    CHECK(dev0.int_requested);
    CHECK(dev0.int_request_data == 0xE8);
    CHECK(!dev0.int_enabled);
    CHECK(!dev1.int_enabled);
    CHECK(!dev2.int_enabled);
    CHECK(cpu.irq_received);
    step(cpu);
    CHECK(!cpu.irq_received);
    CHECK(!dev0.int_requested);
    CHECK(dev0.int_pending);
    CHECK(!dev0.int_enabled);
    CHECK(!dev1.int_enabled);
    CHECK(!dev2.int_enabled);
    CHECK(cpu.state.PC == 0x0008);
    CHECK(!cpu.state.IFF1);
    CHECK(!cpu.state.IFF2);

    // interrupt handler, first execute an EI
    cpu.step();
    CHECK(cpu.enable_interrupt);
    CHECK(!cpu.state.IFF1);
    CHECK(!cpu.state.IFF2);

    // a NOP following the EI, interrupts should be enabled again afterwards
    cpu.step();
    CHECK(!cpu.enable_interrupt);
    CHECK(cpu.state.IFF1);
    CHECK(cpu.state.IFF2);

    // this is the RETI
    cpu.step();
    CHECK(!dev0.int_pending);
    CHECK(dev0.int_enabled);
    CHECK(dev1.int_enabled);
    CHECK(dev2.int_enabled);
    CHECK(cpu.state.IFF1);
    CHECK(cpu.state.IFF2);
    CHECK(cpu.state.PC == 0x0005);
}
