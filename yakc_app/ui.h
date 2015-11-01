#pragma once
//------------------------------------------------------------------------------
/**
    @class ui
    @brief imgui-based debugger UI
*/
#include "kc85_oryol.h"
#include "Time/TimePoint.h"

class ui {
public:
    /// constructor
    ui();
    /// setup the UI
    void setup(yakc::kc85& kc);
    /// discard the UI
    void discard();
    /// do one frame
    void onframe(yakc::kc85& kc);
private:
    /// read register value into text edit buffer
    void reg_to_buf(const yakc::kc85& kc, yakc::z80::reg r);
    /// write updated register back to cpu state
    void buf_to_reg(yakc::kc85& kc, yakc::z80::reg r) const;
    /// read all register values into their text edit buffers
    void regs_to_buf(const yakc::kc85& kc);
    /// the cpu-status window
    void cpu(yakc::kc85& kc);

    Oryol::TimePoint curTime;
    bool show_cpu;
    bool kc_paused;
    static const int buf_size = 5;
    char buf[yakc::z80::reg::num][buf_size];
};
