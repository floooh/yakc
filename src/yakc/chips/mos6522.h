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

    /// port id's
    enum {
        PORT_A = 0,
        PORT_B,
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

    /// output the port B value to the bus
    void bus_out_b(system_bus* bus);
    /// output the port A value to the bus
    void bus_out_a(system_bus* bus);

    /// read port B input from bus
    uint8_t bus_in_b(system_bus* bus);
    /// read port A input from bus
    uint8_t bus_in_a(system_bus* bus);

    int id = 0;
    uint8_t out_b = 0;      // output latch port B
    uint8_t in_b  = 0;      // input latch port B
    uint8_t ddr_b = 0;      // data direction bits port B
    uint8_t in_a  = 0;      // input latch port A
    uint8_t out_a = 0;      // output latch port A
    uint8_t ddr_a = 0;      // data direction bits port A
    uint8_t acr = 0;        // auxilary control register
    uint8_t pcr = 0;        // peripheral control register
    uint8_t t1_pb7 = 0;     // timer1 toggle bit (masked into port B bit 7)

    /// auxilary control register bits
    bool acr_latch_a() const { return acr & 0x01; }
    bool acr_latch_b() const { return acr & 0x02; }
    bool acr_sr_disabled() const    { return ((acr>>2) & 7) == 0; }
    bool acr_sr_in_t2() const       { return ((acr>>2) & 7) == 1; }
    bool acr_sr_in_02() const       { return ((acr>>2) & 7) == 2; }
    bool acr_sr_in_ext() const      { return ((acr>>2) & 7) == 3; }
    bool acr_sr_out_free_t2() const { return ((acr>>2) & 7) == 4; }
    bool acr_sr_out_t2() const      { return ((acr>>2) & 7) == 5; }
    bool acr_sr_out_02() const      { return ((acr>>2) & 7) == 6; }
    bool acr_sr_out_ext() const     { return ((acr>>2) & 7) == 7; }
    bool acr_t2_int() const         { return (acr & (1<<5)) == 0; }
    bool acr_t2_countt_pb6() const  { return (acr & (1<<5)) != 0; }
    bool acr_t1_timed_int() const   { return (acr & (1<<6)) == 0; }
    bool acr_t1_cont_int() const    { return (acr & (1<<6)) != 0; }
    bool acr_t1_pb7() const         { return (acr & (1<<7)) != 0; }
};

} // namespace YAKC
