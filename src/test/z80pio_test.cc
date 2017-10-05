//------------------------------------------------------------------------------
//  z80pio_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/chips/z80pio.h"
#include "yakc/core/system_bus.h"

using namespace YAKC;

//------------------------------------------------------------------------------
TEST(z80pio_control) {

    // this roughly follows the Z80 PIO user's manual
    // (http://www.z80.info/zip/z80piomn.pdf )

    system_bus bus;
    z80pio pio;
    pio.init(0);

    for (int i = 0; i < z80pio::num_ports; i++) {
        CHECK(0 == pio.port[i].output);
        CHECK(0 == pio.port[i].input);
        CHECK(0 == pio.port[i].io_select);
        CHECK(0 == pio.port[i].mode);
        CHECK(0xFF == pio.port[i].int_mask);
        CHECK(0 == pio.port[i].int_vector);
        CHECK(0 == pio.port[i].int_control);
        CHECK(z80pio::expect_any == pio.port[i].expect);
    }

    // load interrupt vector
    pio.write_control(z80pio::A, 0xE4);
    CHECK(0xE4 == pio.port[z80pio::A].int_vector);
    pio.write_control(z80pio::B, 0xE6);
    CHECK(0xE6 == pio.port[z80pio::B].int_vector);

    // select OUTPUT mode
    pio.write_control(z80pio::A, (0<<6)|0xF);
    CHECK(z80pio::mode_output == pio.port[z80pio::A].mode);
    CHECK(z80pio::expect_any == pio.port[z80pio::A].expect);

    // select INPUT mode
    pio.write_control(z80pio::A, (1<<6)|0xF);
    CHECK(z80pio::mode_input == pio.port[z80pio::A].mode);
    CHECK(z80pio::expect_any == pio.port[z80pio::A].expect);

    // select BIDIRECTIONAL mode (only allowed on port A)
    pio.write_control(z80pio::A, (2<<6)|0xF);
    CHECK(z80pio::mode_bidirectional == pio.port[z80pio::A].mode);
    CHECK(z80pio::expect_any == pio.port[z80pio::A].expect);

    // select BITCONTROL mode, must be followed by io-select mask
    pio.write_control(z80pio::A, (3<<6)|0xF);
    CHECK(z80pio::mode_bitcontrol == pio.port[z80pio::A].mode);
    CHECK(z80pio::expect_io_select == pio.port[z80pio::A].expect);
    CHECK(0 == pio.port[z80pio::A].io_select);
    pio.write_control(z80pio::A, 0xAA);
    CHECK(z80pio::mode_bitcontrol == pio.port[z80pio::A].mode);
    CHECK(z80pio::expect_any == pio.port[z80pio::A].expect);
    CHECK(0xAA == pio.port[z80pio::A].io_select);

    // set interrupt control word (no interrupt control mask)
    pio.write_control(z80pio::A, (1<<7)|(6<<4)|7);
    CHECK(z80pio::mode_bitcontrol == pio.port[z80pio::A].mode);
    CHECK(z80pio::expect_any == pio.port[z80pio::A].expect);
    CHECK(0xE0 == pio.port[z80pio::A].int_control);

    // just flip the interrupt-enabled flag
    pio.write_control(z80pio::A, (0<<7)|3);
    CHECK(z80pio::mode_bitcontrol == pio.port[z80pio::A].mode);
    CHECK(z80pio::expect_any == pio.port[z80pio::A].expect);
    CHECK(0x60 == pio.port[z80pio::A].int_control);
    pio.write_control(z80pio::A, (1<<7)|3);
    CHECK(z80pio::mode_bitcontrol == pio.port[z80pio::A].mode);
    CHECK(z80pio::expect_any == pio.port[z80pio::A].expect);
    CHECK(0xE0 == pio.port[z80pio::A].int_control);

    // set the mode 3 interrupt control mask
    pio.write_control(z80pio::A, (1<<7)|(7<<4)|7);
    CHECK(z80pio::mode_bitcontrol == pio.port[z80pio::A].mode);
    CHECK(z80pio::expect_int_mask == pio.port[z80pio::A].expect);
    CHECK(0xF0 == pio.port[z80pio::A].int_control);
    CHECK(0xFF == pio.port[z80pio::A].int_mask);
    pio.write_control(z80pio::A, 0x55);
    CHECK(z80pio::mode_bitcontrol == pio.port[z80pio::A].mode);
    CHECK(z80pio::expect_any == pio.port[z80pio::A].expect);
    CHECK(0xF0 == pio.port[z80pio::A].int_control);
    CHECK(0x55 == pio.port[z80pio::A].int_mask);

    // read back the control port, FIXME: is this undocumented?
    pio.write_control(z80pio::A, (1<<7)|(1<<4)|7);
    pio.write_control(z80pio::A, 0x55);
    pio.write_control(z80pio::B, (1<<7)|(1<<6)|(1<<5)|7);
    uint8_t val_a = pio.read_control();
    uint8_t val_b = pio.read_control();
    CHECK(val_a == val_b);
    CHECK(val_a == ((1<<7)|(1<<3)|(1<<2)|(1<<1)));

    // reset the pio
    pio.reset();
    CHECK(0xE4 == pio.port[z80pio::A].int_vector);  // int vector are *not* reset
    CHECK(0xE6 == pio.port[z80pio::B].int_vector);
    for (int i = 0; i < z80pio::num_ports; i++) {
        CHECK(0 == pio.port[i].output);
        CHECK(z80pio::mode_input == pio.port[i].mode);
        CHECK(0xFF == pio.port[i].int_mask);
        CHECK(0 == pio.port[i].io_select);
        CHECK(z80pio::expect_any == pio.port[i].expect);
    }
}

//------------------------------------------------------------------------------
class pioTestBus : public system_bus {
public:
    uint8_t out_value_a = 0;
    uint8_t in_value_a = 0;
    uint8_t out_value_b = 0;
    uint8_t in_value_b = 0;

    virtual uint8_t pio_in(int pio_id, int port_id) override {
        if (z80pio::A == port_id) {
            return this->in_value_a;
        }
        else {
            return this->in_value_b;
        }
    }
    virtual void pio_out(int pio_id, int port_id, uint8_t val) override {
        if (z80pio::A == port_id) {
            this->out_value_a = val;
        }
        else {
            this->out_value_b = val;
        }
    }
};

TEST(z80pio_output_input) {

    // init port A to output mode and port B to input mode
    pioTestBus bus;
    z80pio pio;
    pio.init(0);

    pio.write_control(z80pio::A, (0<<6)|0xF);
    CHECK(z80pio::mode_output == pio.port[z80pio::A].mode);
    CHECK(z80pio::expect_any == pio.port[z80pio::A].expect);
    pio.write_control(z80pio::B, (1<<6)|0xF);
    CHECK(z80pio::mode_input == pio.port[z80pio::B].mode);
    CHECK(z80pio::expect_any == pio.port[z80pio::B].expect);

    // write a data value to PIO-A, this should set the internal output register
    // and call the out callback
    pio.write_data(&bus, z80pio::A, 0x54);
    CHECK(0x54 == pio.port[z80pio::A].output);
    CHECK(0 == pio.port[z80pio::A].input);
    CHECK(0x54 == bus.out_value_a);

    // reading a value in output mode should not call the input callbacks
    // but simply return the value of the output register
    bus.out_value_a = 0;
    bus.in_value_a = 0x23;
    uint8_t read_val = pio.read_data(&bus, z80pio::A);
    CHECK(0x54 == read_val);
    CHECK(0 == bus.out_value_a);
    CHECK(0x23 == bus.in_value_a);
    CHECK(0x54 == pio.port[z80pio::A].output);
    CHECK(0 == pio.port[z80pio::A].input);

    // read data value from PIO-B, in input mode, this should invoke
    // the input-callback to fetch a value
    bus.out_value_b = 0x11;
    bus.in_value_b = 0x45;
    read_val = pio.read_data(&bus, z80pio::B);
    CHECK(0x45 == read_val);
    CHECK(0x45 == pio.port[z80pio::B].input);
    CHECK(0 == pio.port[z80pio::B].output);
    CHECK(0x11 == bus.out_value_b);

    // writing data in input mode writes to the output register
    // (not the input register, but doesn't any callbacks to be called)
    bus.out_value_b = 0;
    bus.in_value_b = 0;
    pio.write_data(&bus, z80pio::B, 0x78);
    CHECK(0 == bus.out_value_b);
    CHECK(0 == bus.in_value_b);
    CHECK(0x78 == pio.port[z80pio::B].output);
    CHECK(0x45 == pio.port[z80pio::B].input);

    // a reset should clear the output register
    pio.reset();
    CHECK(0 == pio.port[z80pio::A].output);
    CHECK(0 == pio.port[z80pio::B].output);
}

