#pragma once
//------------------------------------------------------------------------------
/**
    @class i8255
    @brief emulation of the Intel 8255 PIO 
    
    NOTE: currently this only contains functionality needed for
    Amstrad CPC emulation (only MODE_0 is implemented)
    
    Data sheet: 
        Intel: http://www.csee.umbc.edu/~cpatel2/links/310/data_sheets/8255.pdf
        Mitsubishi: http://www.cpcwiki.eu/imgs/d/df/PPI_M5L8255AP-5.pdf
    MAME impl:
        https://github.com/mamedev/mame/blob/master/src/devices/machine/i8255.cpp
*/
#include "yakc/core.h"

namespace YAKC {

class system_bus;

class i8255 {
public:
    /// port/control identifier (A0, A1 address bits)
    enum {
        PORT_A = 0,
        PORT_B = 1,
        PORT_C = 2,
        CONTROL = 3,
    };

    /// input/output modes
    enum {
        MODE_OUTPUT = 0,
        MODE_INPUT,
    };

    static const int num_ports = 3;
    ubyte output[num_ports] = { };      // output latch
    ubyte control = 0;                  // control word

    /// initialize a i8255 instance
    void init(int id);
    /// reset the i8255
    void reset();
    /// write to the PIO, addr is A,B,C,CTRL (0..3)
    void write(system_bus* bus, int addr, ubyte val);
    /// read from the PIO, addr is A,B,C (CTRL can't be read)
    ubyte read(system_bus* bus, int addr);

private:
    /// change the control bits
    void set_mode(system_bus* bus, ubyte val);
    /// return input or output mode for port A
    int port_a_mode() const {
        return this->control & (1<<4) ? MODE_INPUT : MODE_OUTPUT;
    }
    /// return input or output mode for port B
    int port_b_mode() const {
        return this->control & (1<<1) ? MODE_INPUT : MODE_OUTPUT;
    }
    /// return input or output mode for lower 4 bits of port C
    int port_c_lower_mode() const {
        return this->control & (1<<0) ? MODE_INPUT : MODE_OUTPUT;
    }
    /// return input or output mode for upper 4 bits of port C
    int port_c_upper_mode() const {
        return this->control & (1<<3) ? MODE_INPUT : MODE_OUTPUT;
    }
    /// output value on port C to data bus
    void output_port_c(system_bus* bus) const;
    /// input value on port C
    ubyte input_port_c(system_bus* bus);

    int id = 0;
};


} // namespace YAKC


