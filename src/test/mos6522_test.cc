//------------------------------------------------------------------------------
//  mos6522_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/core/system_bus.h"
#include "yakc/chips/mos6522.h"

using namespace YAKC;

class mos6522_test_bus : public system_bus {
public:
    /// PIO input callback
    virtual uint8_t pio_in(int pio_id, int port_id) {
        return in_val[port_id];
    };
    /// PIO output callback
    virtual void pio_out(int pio_id, int port_id, uint8_t val) {
        out_val[port_id] = val;
    };

    uint8_t in_val[mos6522::NUM_PORTS] = { };
    uint8_t out_val[mos6522::NUM_PORTS] = { };
};

//------------------------------------------------------------------------------
TEST(mos6522_test) {
    mos6522_test_bus bus;
    mos6522 via;
    via.init(0);

    // write data direction register
    via.write(&bus, mos6522::DDRA, 0x0F);
    CHECK(via.ddr_a == 0x0F);
    CHECK(via.read(&bus, mos6522::DDRA) == 0x0F);
    via.write(&bus, mos6522::DDRB, 0xF0);
    CHECK(via.ddr_b == 0xF0);
    CHECK(via.read(&bus, mos6522::DDRB) == 0xF0);

    // output to ports
    via.write(&bus, mos6522::PA, 0xAA);
    CHECK(via.out_a == 0xAA);
    CHECK(bus.out_val[mos6522::PORT_A] == 0xFA);
    via.write(&bus, mos6522::PB, 0xBB);
    CHECK(via.out_b == 0xBB);
    CHECK(bus.out_val[mos6522::PORT_B] == 0xBF);

    // input from ports
    bus.in_val[mos6522::PORT_A] = 0x99;
    bus.in_val[mos6522::PORT_B] = 0xEE;
    CHECK(via.read(&bus, mos6522::PA) == 0x9A);
    CHECK(via.read(&bus, mos6522::PB) == 0xBE);
}

