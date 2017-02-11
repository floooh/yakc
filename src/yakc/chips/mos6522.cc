//------------------------------------------------------------------------------
//  mos6522.cc
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
}

//------------------------------------------------------------------------------
void
mos6522::step() {
    // FIXME
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
        case IORB:
            out_b = val;
            // notify bus if any data-direction-bit is set to output
            if (ddr_b != 0) {
                bus_out_b(bus);
            }
            // FIXME: clear interrupt
            // FIXME: CB2 data ready handshake
            break;

        case IORA:
            out_a = val;
            if (ddr_a != 0) {
                bus_out_a(bus);
            }
            // FIXME: clear interrupt
            // FIXME: CA2 ready handshake, pulse
            break;

        case IORA_NOH:
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
            // FIXME
            break;

        case T1LH:
            // FIXME
            break;

        case T1CH:
            // FIXME
            break;

        case T2CL:
            // FIXME
            break;

        case T2CH:
            // FIXME
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
            // FIXME: shift timer, continuous timer reset
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
        case IORB:
            if (acr_latch_b()) {
                val = in_b;
            }
            else {
                val = bus_in_b(bus);
            }
            // FIXME: clear interrupt
            break;

        case IORA:
            if (acr_latch_a()) {
                val = in_a;
            }
            else {
                val = bus_in_a(bus);
            }
            // FIXME: clear interrupt
            // FIXME: handshake, pulse
            break;

        case IORA_NOH:
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
            // FIXME
            break;

        case T1CH:
            // FIXME
            break;

        case T1LL:
            // FIXME
            break;

        case T1LH:
            // FIMXE
            break;

        case T2CL:
            // FIXME
            break;

        case T2CH:
            // FIXME
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
