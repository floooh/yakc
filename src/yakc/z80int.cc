//------------------------------------------------------------------------------
//  z80int.cc
//------------------------------------------------------------------------------
#include "z80int.h"
#include "z80bus.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
z80int::connect_irq_device(z80int* device) {
    this->downstream_device = device;
}

//------------------------------------------------------------------------------
void
z80int::reset() {
    this->int_enabled = true;
    this->int_requested = false;
    this->int_request_data = 0;
    this->int_pending = false;
}

//------------------------------------------------------------------------------
bool
z80int::request_interrupt(z80bus* bus, ubyte data) {

    // FIXME: hmm can we interrupt ourselves? what happens if our device
    // requests an interrupt, but we're currently also being serviced
    // by the cpu?
    // CURRENTLY, ASSUME THAT WE CANNOT INTERRUPT OURSELF

    if (this->int_enabled) {
        this->int_enabled = false;
        if (bus) {
            bus->irq();
        }
        this->int_requested = true;
        this->int_request_data = data;
        if (this->downstream_device) {
            this->downstream_device->disable_interrupt();
        }
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
ubyte
z80int::interrupt_acknowledged() {
    if (this->int_requested) {
        // it's our turn, return the interrupt-request data byte,
        // downstream interrupts remain disabled until RETI
        this->int_requested = false;
        this->int_pending = true;
        return this->int_request_data;
    }
    else {
        // we didn't request the interrupt, pass the ack downstream
        if (this->downstream_device) {
            return this->downstream_device->interrupt_acknowledged();
        }
        else {
            // hmm this shouldn't happen...
            YAKC_ASSERT(false);
            return 0;
        }
    }
}

//------------------------------------------------------------------------------
/*
CURRENTLY NOT IMPLEMENTED, SEE z80::handle_irq()
void
z80int::interrupt_cancelled() {
    // this is called by the CPU after an interrupt-request
    // if the CPU is in an interrupt-disabled state
    this->int_enabled = true;
    if (this->int_requested) {
        this->int_requested = false;
    }
    else {
        if (this->downstream_device) {
            this->downstream_device->interrupt_cancelled();
        }
        else {
            // this shouldn't happen
            YAKC_ASSERT(false);
        }
    }
}
*/

//------------------------------------------------------------------------------
void
z80int::reti() {
    this->int_enabled = true;
    if (this->int_pending) {
        // this was our interrupt service routine that has finished,
        // enable interrupt on downstream device
        this->int_pending = false;
        if (this->downstream_device) {
            this->downstream_device->enable_interrupt();
        }
    }
    else {
        // we're not services, pass the reti on downstream
        if (this->downstream_device) {
            this->downstream_device->reti();
        }
    }
}

//------------------------------------------------------------------------------
void
z80int::enable_interrupt() {
    this->int_enabled = true;
    // interrupt-enable only propagates downstream if we're
    // currently not under service by the CPU
    if (!this->int_pending && this->downstream_device) {
        this->downstream_device->enable_interrupt();
    }
}

//------------------------------------------------------------------------------
void
z80int::disable_interrupt() {
    this->int_enabled = false;
    // disable-interrupt always propagates to downstream-devices
    if (this->downstream_device) {
        this->downstream_device->disable_interrupt();
    }
}

} // namespace YAKC
