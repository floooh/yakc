#pragma once
//------------------------------------------------------------------------------
/**
    The 'naive' switch-case Z80 decoder parts.
*/
#include "core/common.h"
#include "core/z80.h"

namespace yakc {

//------------------------------------------------------------------------------
inline void
z80::undoc_autocopy(ubyte reg, ubyte val) {
    // this is for the undocumented DD CB and FB CB instruction which autocopy
    // the result into an 8-bit register (except for the F register)
    switch (reg) {
        case 0: B = val; break;
        case 1: C = val; break;
        case 2: D = val; break;
        case 3: E = val; break;
        case 4: H = val; break;
        case 5: L = val; break;
        case 7: A = val; break;
    }
}

//------------------------------------------------------------------------------
inline int
z80::dd_fd_cb(ubyte lead) {
    int d = mem.rs8(PC++);
    uword addr;
    if (lead == 0xDD) {
        addr = IX + d;
    }
    else {
        addr = IY + d;
    }
    ubyte val;
    ubyte op = mem.r8(PC++);
    switch (op) {
        // RLC ([IX|IY]+d) -> r (except F)
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
            val = rlc8(mem.r8(addr), true);
            undoc_autocopy(op, val);
            mem.w8(addr, val);
            return 23;
        // RRC ([IX|IY]+d)
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0B:
        case 0x0C:
        case 0x0D:
        case 0x0E:
        case 0x0F:
            val = rrc8(mem.r8(addr), true);
            undoc_autocopy(op-0x08, val);
            mem.w8(addr, val);
            return 23;
        // RL ([IX|IY]+d)
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
            val = rl8(mem.r8(addr), true);
            undoc_autocopy(op-0x10, val);
            mem.w8(addr, val);
            return 23;
        // RR ([IX|IY]+d)
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
            val = rr8(mem.r8(addr), true);
            undoc_autocopy(op-0x18, val);
            mem.w8(addr, val);
            return 23;
        // SLA ([IX|IY]+d)
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
            val = sla8(mem.r8(addr));
            undoc_autocopy(op-0x20, val);
            mem.w8(addr, val);
            return 23;
        // SRA ([IX|IY]+d)
        case 0x28:
        case 0x29:
        case 0x2A:
        case 0x2B:
        case 0x2C:
        case 0x2D:
        case 0x2E:
        case 0x2F:
            val = sra8(mem.r8(addr));
            undoc_autocopy(op-0x28, val);
            mem.w8(addr, val);
            return 23;
        // SLL ([IX|IY]+d)
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
            val = sll8(mem.r8(addr));
            undoc_autocopy(op-0x30, val);
            mem.w8(addr, val);
            return 23;
        // SRL ([IX|IY]+d)
        case 0x38:
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
            val = srl8(mem.r8(addr));
            undoc_autocopy(op-0x38, val);
            mem.w8(addr, val);
            return 23;
        // BIT b,([IX|IY]+d)
        case 0x46 | (0<<3):
        case 0x46 | (1<<3):
        case 0x46 | (2<<3):
        case 0x46 | (3<<3):
        case 0x46 | (4<<3):
        case 0x46 | (5<<3):
        case 0x46 | (6<<3):
        case 0x46 | (7<<3):
            bit(mem.r8(addr), 1<<((op>>3)&7));
            return 20;
        // RES b,([IX|IY]+d)
        case 0x86 | (0<<3):
        case 0x86 | (1<<3):
        case 0x86 | (2<<3):
        case 0x86 | (3<<3):
        case 0x86 | (4<<3):
        case 0x86 | (5<<3):
        case 0x86 | (6<<3):
        case 0x86 | (7<<3):
            mem.w8(addr, mem.r8(addr) & ~(1<<((op>>3)&7)));
            return 23;
        // SET b,([IX|IY]+d)
        case 0xC6 | (0<<3):
        case 0xC6 | (1<<3):
        case 0xC6 | (2<<3):
        case 0xC6 | (3<<3):
        case 0xC6 | (4<<3):
        case 0xC6 | (5<<3):
        case 0xC6 | (6<<3):
        case 0xC6 | (7<<3):
            mem.w8(addr, mem.r8(addr) | (1<<((op>>3)&7)));
            return 23;

        // unknown opcode
        default:
            return invalid_opcode(4);
    }
}

} // namespace yakc

#include "core/opcodes.h"