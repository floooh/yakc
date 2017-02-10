#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::mos6522.h
    @brief MOS Technologies 6522 VIA emulation
*/
#include "yakc/core/core.h"

namespace YAKC {

class system_bus;

class mos6522 {
public:
    /// register identifiers
    enum {
        IORB = 0,   // Output/Input Register A
        IORA,       // Output/Input Register B
        DDRB,       // Data Direction Register B
        DDRA,       // Data Direction Register A
        T1CL,       // T1-Low-Order-Latches/Counter
        T1CH,       // T1-High-Order-Counter
        T1LL,       // T1-Low-Order-Latches
        T1LH,       // T1-High-Order-Latches
        T2CL,       // T2-Low-Order-Latches
        T2CH,       // T2-High-Order-Latches
        SR,         // Shift Register
        ACR,        // Aux Control Register
        PCR,        // Peripheral Control Register
        IFR,        // Interrupt Flag Register
        IER,        // Interrupt Enable Register
        IORA_NOH,   // same as Reg 1 except "no Handshake"
    };

    /// initialize 6522 instance
    void init(int id);
    /// reset the 6522
    void reset();
    /// step the 6522 one tick
    void step();

    /// write to VIA
    void write(system_bus* bus, int addr, uint8_t val);
    /// read from VIA
    uint8_t read(system_bus* bus, int addr);

    int id = 0;
};

} // namespace YAKC
