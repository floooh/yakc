#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z80pio
    @brief incomplete Z80 PIO emulation (only what needed for KC85)
    
    TODO:
    - STROBE/READY handshake callbacks
    - trigger interrupt

    Resources & References:
        - http://www.z80.info/zip/z80piomn.pdf
        - http://www.z80.info/zip/um0081.pdf
        - MAME z80 pio emulation (https://github.com/mamedev/mame/blob/master/src/devices/machine/z80pio.cpp)
*/
#include "yakc/chips/z80int.h"

namespace YAKC {

class system_bus;

class z80pio {
public:
    /// port identifiers
    enum {
        A = 0,
        B = 1,
        num_ports = 2,
    };

    /// port modes
    enum : uint8_t {
        mode_output = 0,
        mode_input,
        mode_bidirectional,
        mode_bitcontrol,
    };

    /// interrupt control bits
    enum : uint8_t {
        intctrl_enable_int   = (1<<7),
        intctrl_and_or       = (1<<6),
        intctrl_high_low     = (1<<5),
        intctrl_mask_follows = (1<<4)
    };

    /// port expect mode (what is the next control word)
    enum : uint8_t {
        expect_any = 0,
        expect_io_select = 1,
        expect_int_mask = 2,
    };

    /// port state
    struct port_t {
        uint8_t output = 0;       // output register
        uint8_t input = 0;        // input register
        uint8_t io_select = 0;    // i/o select bits (for bit-control mode)
        uint8_t mode = 0;         // 0=output, 1=input, 2=bidirection, 3=bit-control
        uint8_t int_mask = 0xFF;
        uint8_t int_vector = 0;
        uint8_t int_control = 0;
        uint8_t expect = 0;           // next expected control byte
        bool rdy = false;           // ready line active
        bool stb = false;           // strobe line active
        bool bctrl_match = false;   // bitcontrol logic equation result
    };
    port_t port[num_ports];

    /// interrupt controller
    z80int int_ctrl;

    /// initialize the pio
    void init(int id);
    /// reset the pio
    void reset();

    /// write control register
    void write_control(int port_id, uint8_t val);
    /// read control register (same result for both ports)
    uint8_t read_control();
    /// write data register
    void write_data(system_bus* bus, int port_id, uint8_t data);
    /// read data register
    uint8_t read_data(system_bus* bus, int port_id);
    /// strobe signal on PIO-A from peripheral
    void astb(bool active);
    /// strobe signal on PIO-B from peripheral
    void bstb(bool active);
    /// write data from peripheral into PIO
    void write(system_bus* bus, int port_id, uint8_t val);

private:
    /// set a port's ready line
    void set_rdy(system_bus* bus, int port_id, bool active);

    int id = 0;
};

} // namespace YAKC
