#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpudbg
    @brief debug helper functions
*/
#include "yakc/core/core.h"
#include "yakc/devices/z80.h"

namespace YAKC {

class cpudbg {
public:
    /// register enum for set_reg/get_reg methods
    enum class z80reg {
        A=0, F, B, C, D, E, H, L,
        AF, BC, DE, HL, WZ,
        AF_, BC_, DE_, HL_, WZ_,
        I, R, IX, IY, SP, PC,
        IM,
        num
    };

    /// size of PC history ringbuffer (must be 2^N!)
    static const int pc_history_size = 8;
    /// current pc history position
    int pc_history_pos;
    /// a history ringbuffer of previous PC addresses
    uword pc_history[pc_history_size];
    /// execution paused (e.g. because in debugger)
    bool paused;

    /// constructor
    cpudbg();

    /// test whether breakpoint is enabled and hit
    bool check_break(const z80& cpu) const;
    /// store current pc in history ringbuffer
    void store_pc_history(const z80& cpu);
    /// get pc from history ringbuffer (0 is oldest entry)
    uword get_pc_history(int index) const;

    /// enable a breakpoint
    void enable_breakpoint(int index, uword addr);
    /// disable a breakpoint
    void disable_breakpoint(int index);
    /// if breakpoint at addr exists, toggle state, else enable breakpoint
    void toggle_breakpoint(int index, uword addr);
    /// test if an address is a breakpoint
    bool is_breakpoint(uword addr) const;
    /// return true if breakpoint is enabled
    bool breakpoint_enabled(int index) const;
    /// get breakpoint address
    uword breakpoint_addr(int index) const;

    /// step until PC changed (or an invalid opcode is hit)
    void step_pc_modified(system_bus* bus, z80& cpu);

    /// set an 8-bit register value by enum (slow)
    static void set8(z80& cpu, z80reg r, ubyte v);
    /// get an 8-bit register value by enum (slow)
    static ubyte get8(const z80& cpu, z80reg r);
    /// set a 16-bit register value by enum (slow)
    static void set16(z80& cpu, z80reg r, uword v);
    /// get a 16-bit register value by enum (slow)
    static uword get16(const z80& cpu, z80reg r);
    /// get a string-name for a register
    static const char* reg_name(z80reg r);

private:
    static const int max_breakpoints = 2;
    struct breakpoint {
        breakpoint() : enabled(false), address(0) {};
        bool enabled;
        uword address;
    } breakpoints[max_breakpoints];
};

} // namespace YAKC
