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
#include "yakc/common.h"

namespace yakc {

class z80int {
public:
    /// interrupt request callback (/INT pin on CPU), returns false if CPU interrupts disabled
    typedef void (*cb_int)(void* userdata);

    /// enable/disable logging
    void enable_logging(bool b);

    /// connect to CPU /INT pin callback, called when device requests interrupt
    void connect_cpu(cb_int int_cb, void* userdata);
    /// connect to downstream (lower-pri) device in daisy chain
    void connect_irq_device(z80int* downstream_device);

    /// reset our state
    void reset();

    /// called by device to request an interrupt
    bool request_interrupt(ubyte data);
    /// called by CPU to acknowldge interrupt request, return data byte (usually interrupt vector)
    ubyte interrupt_acknowledged();
    /// NOTE: interrupt_cancelled is currently not called by CPU, this
    /// caused problems with some games which had very high frequency interrupts,
    /// called by CPU after request_interrupt to notify devices that it cannot serve the request
    // void interrupt_cancelled();
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
    /// the interrupt has been acknowledged and is currently processed by CPU until RETI is received
    bool int_pending = false;

private:
    bool logging_enabled = false;
    cb_int int_cb = nullptr;
    void* int_cb_userdata = nullptr;
    z80int* downstream_device = nullptr;
};

//------------------------------------------------------------------------------
inline void
z80int::enable_logging(bool b) {
    this->logging_enabled = b;
}

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
    this->int_pending = false;
}

//------------------------------------------------------------------------------
inline bool
z80int::request_interrupt(ubyte data) {

    // FIXME: hmm can we interrupt ourselves? what happens if our device
    // requests an interrupt, but we're currently also being serviced
    // by the cpu?
    // CURRENTLY, ASSUME THAT WE CANNOT INTERRUPT OURSELF

    YAKC_ASSERT(this->int_cb);
    if (this->int_enabled) {
        if (this->logging_enabled) {
            YAKC_PRINTF("z80int: interrupt requested\n");
        }
        this->int_enabled = false;
        this->int_cb(this->int_cb_userdata);
        this->int_requested = true;
        this->int_request_data = data;
        if (this->downstream_device) {
            this->downstream_device->disable_interrupt();
        }
        return true;
    }
    else {
        if (this->logging_enabled) {
            YAKC_PRINTF("z80int: interrupt requested, but disabled!\n");
        }
        return false;
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80int::interrupt_acknowledged() {
    if (this->int_requested) {
        // it's our turn, return the interrupt-request data byte,
        // downstream interrupts remain disabled until RETI
        if (this->logging_enabled) {
            YAKC_PRINTF("z80int: interrupt acknowledged, returning 0x%02X\n",
                this->int_request_data);
        }
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
inline void
z80int::interrupt_cancelled() {
    // this is called by the CPU after an interrupt-request
    // if the CPU is in an interrupt-disabled state
    if (this->logging_enabled) {
        YAKC_PRINTF("z80int: interrupt denied by CPU\n");
    }
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
inline void
z80int::reti() {
    this->int_enabled = true;
    if (this->int_pending) {
        // this was our interrupt service routine that has finished,
        // enable interrupt on downstream device
        if (this->logging_enabled) {
            YAKC_PRINTF("z80int: reti received, enabling interrupt\n");
        }
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
inline void
z80int::enable_interrupt() {
    this->int_enabled = true;
    // interrupt-enable only propagates downstream if we're
    // currently not under service by the CPU
    if (!this->int_pending && this->downstream_device) {
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
