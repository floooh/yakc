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
    /// interrupt request callback (/INT pin on CPU)
    typedef void cb_int(void* userdata, ubyte data);

    /// connect to low-pri device in daisy chain
    void connect_downstream_device(z80int* low_pri_device);
    /// connect to CPU /INT pin callback, called when device requests interrupt
    void connect_cpu_int(cb_int int_cb, void* userdata);

    /// called by device to request an interrupt
    void request_interrupt(ubyte data);
    /// called by CPU to acknowldge interrupt request, return data byte (usually interrupt vector)
    /// pass this on to downstream device if no interrupt was requested
    ubyte interrupt_acknowledged();
    /// enable/disable interrupt, called by upstream device
    void enable_interrupt(bool enabled);
    /// the CPU has executed a RETI, pass on to downstream devices
    void reti();

    /// interrupt enabled/disabled state (state of the IEI pin)
    bool int_enabled = true;
    /// an interrupt is currently being requested
    bool int_requested = false;
    /// the data byte for the requested interrupt
    ubyte int_request_data = 0;
    /// the interrupt has been ackowledged by CPU
    bool int_acknowledged = false;
};

} // namespace yakc
