#pragma once
//------------------------------------------------------------------------------
/**
    @class ui
    @brief imgui-based debugger UI
*/
#include "yakc_core/kc85.h"
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
    struct reg16 {
        enum r16 {
            AF = 0,
            BC,
            DE,
            HL,
            AF_,
            BC_,
            DE_,
            HL_,
            IX,
            IY,
            SP,
            PC,
            num
        };
        static const char* name(r16 r) {
            switch (r) {
                case AF: return "AF";
                case BC: return "BC";
                case DE: return "DE";
                case HL: return "HL";
                case AF_: return "AF'";
                case BC_: return "BC'";
                case DE_: return "DE'";
                case HL_: return "HL'";
                case IX: return "IX";
                case IY: return "IY";
                case SP: return "SP";
                case PC: return "PC";
                default: return "??";
            }
        };
        static const int buf_size = 5;
        char buf[buf_size] = { 0 };
    } r16[reg16::num];

    /// convert a single 4-bit nibble to a hex character (0..F)
    static char nibble_to_str(yakc::ubyte n);
    /// convert a byte to a hex string
    static void ubyte_to_str(yakc::ubyte b, char* buf, int buf_size);
    /// convert an uword to a hex string
    static void uword_to_str(yakc::uword w, char* buf, int buf_size);
    /// parse 4 hex characters into an uint16_t, return old value if failed
    static yakc::uword parse_uword(const char* str, yakc::uword old_val);
    /// read register value into text edit buffer
    void reg_to_buf(const yakc::kc85& kc, reg16::r16 r);
    /// write updated register back to cpu state
    void buf_to_reg(yakc::kc85& kc, reg16::r16 r) const;
    /// read all register values into their text edit buffers
    void regs_to_buf(const yakc::kc85& kc);
    /// the cpu-status window
    void cpu(yakc::kc85& kc);

    Oryol::TimePoint curTime;
    bool show_cpu;
    bool kc_paused;
};
