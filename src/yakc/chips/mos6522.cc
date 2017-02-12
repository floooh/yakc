//------------------------------------------------------------------------------
//  mos6522.cc
//
//  reference: https://github.com/mamedev/mame/blob/master/src/devices/machine/6522via.cpp
//------------------------------------------------------------------------------
#include "mos6522.h"
#include "yakc/core/system_bus.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
mos6522::init(int id_) {
    this->id = id_;
    this->reset();
}

//------------------------------------------------------------------------------
void
mos6522::reset() {
    out_b = out_a = 0;
    in_b  = in_a  = 0;
    ddr_b = ddr_a = 0;
    acr = pcr = 0;
    t1_pb7 = 0;
    t1ll = t2lh = 0;
    t2ll = t2lh = 0;
    t1 = 0;
    t2 = 0;
}

//------------------------------------------------------------------------------
void
mos6522::step(system_bus* bus) {
    t1--;
    if (t1 == 0) {
        // FIXME: implement 3 cycle delay
        if (acr_t1_cont_int()) {
            // continuous interval, reset counter
            t1_pb7 = !t1_pb7;
            t1 = (t1lh<<8) | t1ll;
        }
        else {
            // one-shot, don't reset counter
            t1_pb7 = 1;
            t1_active = false;
        }
        if (acr_t1_pb7()) {
            bus_out_b(bus);
        }
        // FIXME: interrupt
    }
    t2--;
    if (t2 == 0) {
        // FIXME: implement 3 cycle delay
        t2_active = false;
        // FIXME: interrupt
    }
}

//------------------------------------------------------------------------------
void
mos6522::bus_out_b(system_bus* bus) {
    // mask output bits, set input bits to 1
    uint8_t val = (out_b & ddr_b) | ~ddr_b;

    // timer 1 state into bit 7?
    if (acr_t1_pb7()) {
        val = (val & 0x7F) | (t1_pb7<<7);
    }
    bus->pio_out(id, PORT_B, val);
}

//------------------------------------------------------------------------------
uint8_t
mos6522::bus_in_b(system_bus* bus) {
    uint8_t in_val = bus->pio_in(id, PORT_B);
    in_val = (out_b & ddr_b) | (in_val & ~ddr_b);
    if (acr_t1_pb7()) {
        in_val = (in_val & 0x7F) | (t1_pb7<<7);
    }
    return in_val;
}

//------------------------------------------------------------------------------
void
mos6522::bus_out_a(system_bus* bus) {
    // mask output bits, set input bits to 1
    uint8_t val = (out_a & ddr_a) | ~ddr_a;
    bus->pio_out(id, PORT_A, val);
}

//------------------------------------------------------------------------------
uint8_t
mos6522::bus_in_a(system_bus* bus) {
    uint8_t in_val = bus->pio_in(id, PORT_A);
    in_val = (out_a & ddr_a) | (in_val & ~ddr_a);
    return in_val;
}

//------------------------------------------------------------------------------
void
mos6522::write(system_bus* bus, int addr, uint8_t val) {
    switch (addr & 0x0F) {
        case PB:
            out_b = val;
            // notify bus if any data-direction-bit is set to output
            if (ddr_b != 0) {
                bus_out_b(bus);
            }
            // FIXME: clear interrupt
            // FIXME: CB2 data ready handshake
            break;

        case PA:
            out_a = val;
            if (ddr_a != 0) {
                bus_out_a(bus);
            }
            // FIXME: clear interrupt
            // FIXME: CA2 ready handshake, pulse
            break;

        case PA_NOH:
            // no handshake
            out_a = val;
            if (ddr_a != 0) {
                bus_out_a(bus);
            }
            break;

        case DDRB:
            if (val != ddr_b) {
                ddr_b = val;
                bus_out_b(bus);
            }
            break;

        case DDRA:
            if (val != ddr_a) {
                ddr_a = val;
                bus_out_a(bus);
            }
            break;

        case T1CL:
        case T1LL:
            t1ll = val;
            break;

        case T1LH:
            // FIXME: clear interrupt
            t1lh = val;
            break;

        case T1CH:
            // FIXME: clear interrupt
            t1lh = val;
            t1 = ((t1lh<<8) | t1ll);
            t1_pb7 = 0;
            if (acr_t1_pb7()) {
                bus_out_b(bus);
            }
            t1_active = true;
            break;

        case T2CL:
            t2ll = val;
            break;

        case T2CH:
            // FIXME: clear interrupt
            t2lh = val;
            t2 = (t2lh<<8) | t2ll;
            if (acr_t2_count_pb6()) {
                // count down on PB6 pulses
                t2 = (t2lh<<8) | t2ll;
                t2_active = true;
            }
            else {
                // regular countdown
                t2 = ((t2lh<<8) | t2ll);
                t2_active = true;
            }
            break;

        case SR:
            // FIXME
            break;

        case PCR:
            pcr = val;
            // FIXME: CA2, CB2
            break;

        case ACR:
            acr = val;
            bus_out_b(bus);
            // FIXME: shift timer
            if (acr_t1_cont_int()) {
                t1_active = true;
            }
            break;

        case IER:
            // FIXME
            break;

        case IFR:
            // FIXME
            break;
    }
}

//------------------------------------------------------------------------------
uint8_t
mos6522::read(system_bus* bus, int addr) {
    uint8_t val = 0;
    switch (addr & 0x0F) {
        case PB:
            if (acr_latch_b()) {
                val = in_b;
            }
            else {
                val = bus_in_b(bus);
            }
            // FIXME: clear interrupt
            break;

        case PA:
            if (acr_latch_a()) {
                val = in_a;
            }
            else {
                val = bus_in_a(bus);
            }
            // FIXME: clear interrupt
            // FIXME: handshake, pulse
            break;

        case PA_NOH:
            // port A input, no handshake
            if (acr_latch_a()) {
                val = in_a;
            }
            else {
                val = bus_in_a(bus);
            }
            break;

        case DDRB:
            val = ddr_b;
            break;

        case DDRA:
            val = ddr_a;
            break;

        case T1CL:
            // FIXME: clear interrupt
            val = t1 & 0x00FF;
            break;

        case T1CH:
            val = t1>>8;
            break;

        case T1LL:
            val = t1ll;
            break;

        case T1LH:
            val = t1lh;
            break;

        case T2CL:
            // FIXME: clear interrupt
            val = t2 & 0x00FF;
            break;

        case T2CH:
            val = t2>>8;
            break;

        case SR:
            // FIXME
            break;

        case PCR:
            val = pcr;
            break;

        case ACR:
            val = acr;
            break;

        case IER:
            // FIXME
            break;

        case IFR:
            // FIXME
            break;
    }
    return val;
}

} // namespace YAKC
