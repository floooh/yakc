#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z80int
    @brief building block to implement an interrupt daisy-chain
    
    See here: 
    http://www.z80.info/zip/z80-interrupts.pdf
    http://www.z80.info/1653.htm
    http://www.z80.info/zip/z80-interrupts_rewritten.pdf

    Each chip that can generate interrupt requests has an z80int object 
    embedded which implements the daisy-chain protocol to prioritize
    interrupt requests.
*/
#include "yakc/core.h"

namespace YAKC {

class z80bus;
class z80int {
public:
    /// connect to downstream (lower-pri) device in daisy chain
    void connect_irq_device(z80int* downstream_device);

    /// reset our state
    void reset();

    /// called by device to request an interrupt
    bool request_interrupt(z80bus* bus, ubyte data);
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
    z80int* downstream_device = nullptr;
};

} // namespace YAKC
