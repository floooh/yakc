//------------------------------------------------------------------------------
//  daisychain_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/chips/z80.h"
#include "yakc/chips/z80int.h"
#include "yakc/core/system_bus.h"

using namespace YAKC;

static ubyte ram[0x4000];

class daisyChainTestBus : public system_bus {
public:
    z80 cpu;
    virtual void irq(bool b) {
        cpu.irq(b);
    }
};

static void step(daisyChainTestBus& bus) {
    bus.cpu.step(&bus);
    bus.cpu.handle_irq(&bus);
}

TEST(daisychain) {

    daisyChainTestBus bus;
    memset(ram, 0, sizeof(ram));
    bus.cpu.mem.map(0, 0x0000, sizeof(ram), ram, true);
    bus.cpu.IM = 0x02;
    bus.cpu.I = 0x01;

    ubyte prog[] = {
        0xFB,               // EI
        0x31, 0x00, 0x01,   // LD SP,0x0100
        0x00, 0x00, 0x00,   // 3x NOP
        0xC9,               // RET
        0xFB,               // EI
        0x00,               // NOP
        0xED, 0x4D,         // RETI
    };
    bus.cpu.mem.write(0x0000, prog, sizeof(prog));
    bus.cpu.mem.w16(0x1E8, 0x0008);

    // interrupt controller devices
    z80int dev0;
    z80int dev1;
    z80int dev2;

    // build the daisy chain
    bus.cpu.connect_irq_device(&dev0);
    dev0.connect_irq_device(&dev1);
    dev1.connect_irq_device(&dev2);

    // step the cpu a few times, and request an interrupt on the highest priority device

    // EI
    step(bus);
    CHECK(bus.cpu.int_enable);
    CHECK(!bus.cpu.IFF1);
    CHECK(!bus.cpu.IFF2);

    // LD SP,0x0100, and delayed interrupt-enable from EI
    step(bus);
    CHECK(bus.cpu.SP == 0x0100);
    CHECK(!bus.cpu.int_enable);
    CHECK(bus.cpu.IFF1);
    CHECK(bus.cpu.IFF2);

    // request interrupt, execute nop, acknowledge interrupt
    dev0.request_interrupt(&bus, 0xE8);
    CHECK(dev0.int_requested);
    CHECK(dev0.int_request_data == 0xE8);
    CHECK(!dev0.int_enabled);
    CHECK(!dev1.int_enabled);
    CHECK(!dev2.int_enabled);
    CHECK(bus.cpu.int_active);
    step(bus);
    CHECK(!bus.cpu.int_active);
    CHECK(!dev0.int_requested);
    CHECK(dev0.int_pending);
    CHECK(!dev0.int_enabled);
    CHECK(!dev1.int_enabled);
    CHECK(!dev2.int_enabled);
    CHECK(bus.cpu.PC == 0x0008);
    CHECK(!bus.cpu.IFF1);
    CHECK(!bus.cpu.IFF2);

    // interrupt handler, first execute an EI
    bus.cpu.step(&bus);
    CHECK(bus.cpu.int_enable);
    CHECK(!bus.cpu.IFF1);
    CHECK(!bus.cpu.IFF2);

    // a NOP following the EI, interrupts should be enabled again afterwards
    bus.cpu.step(&bus);
    CHECK(!bus.cpu.int_enable);
    CHECK(bus.cpu.IFF1);
    CHECK(bus.cpu.IFF2);

    // this is the RETI
    bus.cpu.step(&bus);
    CHECK(!dev0.int_pending);
    CHECK(dev0.int_enabled);
    CHECK(dev1.int_enabled);
    CHECK(dev2.int_enabled);
    CHECK(bus.cpu.IFF1);
    CHECK(bus.cpu.IFF2);
    CHECK(bus.cpu.PC == 0x0005);
}
