//------------------------------------------------------------------------------
//  z80pio.cc
//------------------------------------------------------------------------------
#include "z80pio.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
z80pio::init() {
    for (int i = 0; i < num_ports; i++) {
        this->port[i] = port_t();
        this->out_callback[i].func = nullptr;
        this->out_callback[i].userdata = nullptr;
        this->in_callback[i].func = nullptr;
        this->in_callback[i].userdata = nullptr;
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
        p.io_select = 0;
        p.expect = expect_any;
    }
}

//------------------------------------------------------------------------------
void
z80pio::connect_out_cb(int port_id, void* userdata, out_cb cb) {
    YAKC_ASSERT((port_id>=0)&&(port_id<num_ports));
    this->out_callback[port_id].func = cb;
    this->out_callback[port_id].userdata = userdata;
}

//------------------------------------------------------------------------------
void
z80pio::connect_in_cb(int port_id, void* userdata, in_cb cb) {
    YAKC_ASSERT((port_id>=0)&&(port_id<num_ports));
    this->in_callback[port_id].func = cb;
    this->in_callback[port_id].userdata = userdata;
}

//------------------------------------------------------------------------------
void
z80pio::control(int port_id, ubyte val) {
    YAKC_ASSERT((port_id>=0)&&(port_id<num_ports));

    // FIXME: this lacks the finer details of interrupt handling,
    // for instance, interrupts are disabled while the PIO
    // expects a followup control word

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
                }
            }
        }
        else if ((val & 0xF) == 7) {
            // set interrupt control word
            p.int_control = val & 0xF0;
            if (val & intctrl_mask_follows) {
                p.expect = expect_int_mask;
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
void
z80pio::write_data(int port_id, ubyte data) {
    YAKC_ASSERT((port_id >= 0) && (port_id < num_ports));
    auto& p = this->port[port_id];
    const auto& cb = this->out_callback[port_id];
    switch (p.mode) {
        case mode_output:
            p.output = data;
            if (cb.func) {
                cb.func(cb.userdata, data);
            }
            break;
        case mode_input:
            p.output = data;
            break;
        case mode_bidirectional:
            // FIXME (write data to peripheral, how)
            p.output = data;
            break;
        case mode_bitcontrol:
            p.output = data;
            if (cb.func) {
                cb.func(cb.userdata, p.io_select | (p.output & ~p.io_select));
            }
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
    const auto& cb = this->in_callback[port_id];
    switch (p.mode) {
        case mode_output:
            data = p.output;
            break;
        case mode_input:
            if (cb.func) {
                p.input = cb.func(cb.userdata);
            }
            data = p.input;
            break;
        case mode_bidirectional:
            // FIXME: p.input written from peripheral device?
            data = p.input;
            break;
        case mode_bitcontrol:
            if (cb.func) {
                p.input = cb.func(cb.userdata);
            }
            data = (p.input & p.io_select) | (p.output & ~p.io_select);
            break;
        default:
            YAKC_ASSERT(false);
            break;
    }
    return data;
}

} // namespace YAKC
