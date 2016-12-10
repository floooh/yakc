#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z80bus
    @brief chip-interconnect callback interface
*/
#include "yakc/core.h"

namespace YAKC {

class z80bus {
public:
    /// Z80 CPU IN callback
    virtual ubyte cpu_in(uword port);
    /// Z80 CPU OUT callback
    virtual void cpu_out(uword port, ubyte val);

    /// Z80 PIO input callback
    virtual ubyte pio_in(int pio_id, int port_id);
    /// Z80 PIO output callback
    virtual void pio_out(int pio_id, int port_id, ubyte val);
    /// Z80 PIO ready callback
    virtual void pio_rdy(int pio_id, int port_id, bool active);

    /// Z80 CTC write callback
    virtual void ctc_write(int ctc_id, int chn_id);
    /// Z80 CTC ZCTO callback
    virtual void ctc_zcto(int ctc_id, int chn_id);
};

} // namespace YAKC
