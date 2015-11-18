#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z80int
    @brief building block to implement an interrupt daisy-chain
    
    See here: 
    http://www.z80.info/zip/z80-interrupts.pdf
    http://www.z80.info/1653.htm
    http://www.z80.info/zip/z80-interrupts_rewritten.pdf

    Each chip that can generate interrupt requests has an z80int object 
    embedded which implements the daisy-chain protocol to prioritize
    interrupt requests.
*/
#include "yakc_core/common.h"

namespace yakc {

class z80int {
public:
    /// interrupt request callback (/INT pin on CPU), returns false if CPU interrupts disabled
    typedef void (*cb_int)(void* userdata);

    /// connect to CPU /INT pin callback, called when device requests interrupt
    void connect_cpu(cb_int int_cb, void* userdata);
    /// connect to downstream (lower-pri) device in daisy chain
    void connect_irq_device(z80int* downstream_device);

    /// reset our state
    void reset();

    /// called by device to request an interrupt
    void request_interrupt(ubyte data);
    /// called by CPU to acknowldge interrupt request, return data byte (usually interrupt vector)
    ubyte interrupt_acknowledged();
    /// called by CPU after request_interrupt to notify devices that it cannot serve the request
    void interrupt_cancelled();
    /// the CPU has executed a RETI, enable downstream devices interrupt
    void reti();
    /// enable interrupt, called by upstream device
    void enable_interrupt();
    /// disable interrupt, called by updatream device
    void disable_interrupt();

    /// interrupt enabled/disabled state (state of the IEI pin)
    bool int_enabled = true;
    /// an interrupt is currently being requested
    bool int_requested = false;
    /// the data byte for the requested interrupt
    ubyte int_request_data = 0;
    /// the interrupt has been acknowledged and is currently processed by CPU
    /// until RETI is received
    bool int_active = false;

private:
    cb_int int_cb = nullptr;
    void* int_cb_userdata = nullptr;
    z80int* downstream_device = nullptr;
};

//------------------------------------------------------------------------------
inline void
z80int::connect_cpu(cb_int cb, void* userdata) {
    this->int_cb = cb;
    this->int_cb_userdata = userdata;
}

//------------------------------------------------------------------------------
inline void
z80int::connect_irq_device(z80int* device) {
    this->downstream_device = device;
}

//------------------------------------------------------------------------------
inline void
z80int::reset() {
    this->int_enabled = true;
    this->int_requested = false;
    this->int_request_data = 0;
    this->int_active = false;
}

//------------------------------------------------------------------------------
inline void
z80int::request_interrupt(ubyte data) {

    // FIXME: hmm can we interrupt ourselves? what happens if our device
    // requests an interrupt, but we're currently also being serviced
    // by the cpu?
    // CURRENTLY, ASSUME THAT WE CANNOT INTERRUPT OURSELF

    YAKC_ASSERT(this->int_cb);
    if (this->int_enabled && !this->int_active) {
        this->int_cb(this->int_cb_userdata);
        this->int_requested = true;
        this->int_request_data = data;
        if (this->downstream_device) {
            this->downstream_device->disable_interrupt();
        }
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80int::interrupt_acknowledged() {
    YAKC_ASSERT(this->int_enabled);
    if (this->int_requested) {
        // it's our turn, return the interrupt-request data byte,
        // downstream interrupts remain disabled until RETI
        this->int_requested = false;
        this->int_active = true;
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
inline void
z80int::interrupt_cancelled() {
    // this is called by the CPU after an interrupt-request
    // if the CPU is in an interrupt-disabled state
    YAKC_ASSERT(this->int_enabled);
    YAKC_ASSERT(!this->int_active);
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

//------------------------------------------------------------------------------
inline void
z80int::reti() {
    YAKC_ASSERT(this->int_enabled);
    if (this->int_active) {
        // this was our interrupt service routine that has finished,
        // enable interrupt on downstream device
        this->int_active = false;
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
inline void
z80int::enable_interrupt() {
    this->int_enabled = true;
    // interrupt-enable only propagates downstream if we're
    // currently not under service by the CPU
    if (!this->int_active && this->downstream_device) {
        this->downstream_device->enable_interrupt();
    }
}

//------------------------------------------------------------------------------
inline void
z80int::disable_interrupt() {
    this->int_enabled = false;
    // disable-interrupt always propagates to downstream-devices
    if (this->downstream_device) {
        this->downstream_device->disable_interrupt();
    }
}

} // namespace yakc
