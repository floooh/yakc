//------------------------------------------------------------------------------
//  z80pio.cc
//------------------------------------------------------------------------------
#include "z80pio.h"
#include "z80bus.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
z80pio::init(int id_, z80bus* bus_) {
    YAKC_ASSERT(bus_);
    this->id = id_;
    this->bus = bus_;
    this->int_ctrl = z80int();
    for (int i = 0; i < num_ports; i++) {
        this->port[i] = port_t();
    }
}

//------------------------------------------------------------------------------
void
z80pio::reset() {
    for (int i = 0; i < num_ports; i++) {
        auto& p = this->port[i];
        p.output = 0;
        p.mode = mode_input;
        p.int_mask = 0xFF;
        p.int_control &= ~0x80; // clear interrupt enabled bit
        p.io_select = 0;
        p.expect = expect_any;
        p.rdy = false;
        p.stb = false;
        p.bctrl_match = false;
    }
}

//------------------------------------------------------------------------------
void
z80pio::set_rdy(int port_id, bool active) {
    auto& p = this->port[port_id];
    if (p.rdy != active) {
        p.rdy = active;
        this->bus->pio_rdy(this->id, port_id, active);
    }
}

//------------------------------------------------------------------------------
void
z80pio::write_control(int port_id, ubyte val) {
    YAKC_ASSERT((port_id>=0)&&(port_id<num_ports));

    auto& p = this->port[port_id];
    if (expect_io_select == p.expect) {
        p.io_select = val;
        p.expect = expect_any;
    }
    else if (expect_int_mask == p.expect) {
        p.int_mask = val;
        p.expect = expect_any;
    }
    else {
        if ((val & 1) == 0) {
            // set interrupt vector
            p.int_vector = val;
        }
        else if ((val & 0xF) == 0xF) {
            // set mode, NOTE: bidirectional on port B is not allowed
            const ubyte m = val>>6;
            if ((port_id == B) && (m == mode_bidirectional)) {
                YAKC_ASSERT(false);
            }
            else {
                p.mode = m;
                if (m == mode_bitcontrol) {
                    p.expect = expect_io_select;
                    p.bctrl_match = false;
                }
            }
        }
        else if ((val & 0xF) == 7) {
            // set interrupt control word
            p.int_control = (val & 0xF0);
            if (val & intctrl_mask_follows) {
                p.expect = expect_int_mask;
                p.bctrl_match = false;
            }
        }
        else if ((val & 0xF) == 3) {
            // only modify interrupt enable bit
            p.int_control = (val & intctrl_enable_int) | (p.int_control & ~intctrl_enable_int);
        }
        else {
            YAKC_ASSERT(false);
        }
    }
}

//------------------------------------------------------------------------------
ubyte
z80pio::read_control() {
    // I haven't found any information what is really returned when
    // reading the control port so I'm going with MAME, which returns
    // a combination of interrupt control word bits from both interrupt channels
    return (this->port[A].int_control & 0xC0) | (this->port[B].int_control>>4);
}

//------------------------------------------------------------------------------
void
z80pio::write_data(int port_id, ubyte data) {
    YAKC_ASSERT((port_id >= 0) && (port_id < num_ports));
    auto& p = this->port[port_id];
    switch (p.mode) {
        case mode_output:
            this->set_rdy(port_id, false);
            p.output = data;
            this->bus->pio_out(this->id, port_id, data);
            this->set_rdy(port_id, true);
            break;
        case mode_input:
            p.output = data;
            break;
        case mode_bidirectional:
            this->set_rdy(port_id, false);
            p.output = data;
            if (!p.stb) {
                this->bus->pio_out(this->id, port_id, data);
            }
            this->set_rdy(port_id, true);
            break;
        case mode_bitcontrol:
            p.output = data;
            this->bus->pio_out(this->id, port_id, p.io_select | (p.output & ~p.io_select));
            break;
        default:
            YAKC_ASSERT(false);
    }
}

//------------------------------------------------------------------------------
ubyte
z80pio::read_data(int port_id) {
    YAKC_ASSERT((port_id >= 0) && (port_id < num_ports));
    ubyte data = 0;
    auto& p = this->port[port_id];
    switch (p.mode) {
        case mode_output:
            data = p.output;
            break;
        case mode_input:
            if (!p.stb) {
                p.input = this->bus->pio_in(this->id, port_id);
            }
            data = p.input;
            this->set_rdy(port_id, false);
            this->set_rdy(port_id, true);
            break;
        case mode_bidirectional:
            data = p.input;
            this->set_rdy(port_id, false);
            this->set_rdy(port_id, true);
            break;
        case mode_bitcontrol:
            p.input = this->bus->pio_in(this->id, port_id);
            data = (p.input & p.io_select) | (p.output & ~p.io_select);
            break;
        default:
            YAKC_ASSERT(false);
            break;
    }
    return data;
}

//------------------------------------------------------------------------------
void
z80pio::write(int port_id, ubyte data) {
    YAKC_ASSERT((port_id >= 0) && (port_id < num_ports));
    auto& p = this->port[port_id];
    if (mode_bitcontrol == p.mode) {
        p.input = data;
        ubyte val = (p.input & p.io_select) | (p.output & ~p.io_select);
        ubyte mask = ~p.int_mask;
        bool match = false;
        val &= mask;

        const ubyte ictrl = p.int_control & 0x60;
        if ((ictrl == 0) && (val != mask)) match = true;
        else if ((ictrl == 0x20) && (val != 0)) match = true;
        else if ((ictrl == 0x40) && (val == 0)) match = true;
        else if ((ictrl == 0x60) && (val == mask)) match = true;
        if (!p.bctrl_match && match && (p.int_control & 0x80)) {
            this->int_ctrl.request_interrupt(this->bus, p.int_vector);
        }
        p.bctrl_match = match;
    }
}

} // namespace YAKC
