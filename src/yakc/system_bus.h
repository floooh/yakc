#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::system_bus
    @brief chip-interconnect callback interface
*/
#include "yakc/core.h"

namespace YAKC {

class system_bus {
public:
    /// CPU IN callback
    virtual ubyte cpu_in(uword port);
    /// CPU OUT callback
    virtual void cpu_out(uword port, ubyte val);

    /// PIO input callback
    virtual ubyte pio_in(int pio_id, int port_id);
    /// PIO output callback
    virtual void pio_out(int pio_id, int port_id, ubyte val);
    /// PIO ready callback
    virtual void pio_rdy(int pio_id, int port_id, bool active);

    /// CTC write callback
    virtual void ctc_write(int ctc_id, int chn_id);
    /// CTC ZCTO callback
    virtual void ctc_zcto(int ctc_id, int chn_id);

    /// request a CPU interrupt
    virtual void irq();
    /// interrupt acknowled CPU
    virtual void iack();
    /// clock timer triggered
    virtual void timer(int timer_id);

    /// optional, called when vblank happens
    virtual void vblank();
};

} // namespace YAKC
