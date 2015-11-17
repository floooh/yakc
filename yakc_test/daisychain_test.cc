//------------------------------------------------------------------------------
//  daisychain_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc_core/z80.h"
#include "yakc_core/z80int.h"

using namespace yakc;

ubyte ram[0x4000];

TEST(daisychain) {

    z80 cpu;
    memset(ram, 0, sizeof(ram));
    cpu.mem.map(0, 0x0000, sizeof(ram), ram, true);
    cpu.state.IFF1 = cpu.state.IFF2 = true;
    cpu.state.IM = 0x02;

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
    cpu.step();
    cpu.step();
    dev0.request_interrupt(0xE8);
}