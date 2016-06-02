//------------------------------------------------------------------------------
//  z80.cc
//------------------------------------------------------------------------------
#include "z80.h"

namespace YAKC {

//------------------------------------------------------------------------------
z80::z80() :
BC(0), DE(0), HL(0), AF(0), IX(0), IY(0), WZ(0), SP(0), 
BC_(0), DE_(0), HL_(0), AF_(0), WZ_(0),
PC(0), I(0), R(0), IM(0),
HALT(false), IFF1(false), IFF2(false), INV(false),
in_func(nullptr),
out_func(nullptr),
inout_userdata(nullptr),
irq_device(nullptr),
irq_received(false),
enable_interrupt(false),
break_on_invalid_opcode(false) {
    this->init_tables();
}

//------------------------------------------------------------------------------
void
z80::init(cb_in func_in, cb_out func_out, void* userdata) {
    this->reset();
    this->in_func = func_in;
    this->out_func = func_out;
    this->inout_userdata = userdata;
}

//------------------------------------------------------------------------------
void
z80::init_tables() {
    for (int val = 0; val < 256; val++) {
        int p = 0;
        if (val & (1<<0)) p++;
        if (val & (1<<1)) p++;
        if (val & (1<<2)) p++;
        if (val & (1<<3)) p++;
        if (val & (1<<4)) p++;
        if (val & (1<<5)) p++;
        if (val & (1<<6)) p++;
        if (val & (1<<7)) p++;
        ubyte f = val?(val&SF):ZF;
        f |= (val & (YF|XF));   // undocumented flag bits 3 and 5
        f |= p & 1 ? 0 : PF;
        this->szp[val] = f;
    }
}

//------------------------------------------------------------------------------
void
z80::connect_irq_device(z80int* device) {
    YAKC_ASSERT(device);
    this->irq_device = device;
}

//------------------------------------------------------------------------------
void
z80::reset() {
    this->PC = 0;
    this->WZ = this->PC;
    this->IM = 0;
    this->HALT = false;
    this->IFF1 = false;
    this->IFF2 = false;
    this->I = 0;
    this->R = 0;
    this->irq_received = false;
    this->enable_interrupt = false;
}

//------------------------------------------------------------------------------
uint32_t
z80::invalid_opcode(uword opsize) {
    if (this->break_on_invalid_opcode) {
        INV = true;
        PC -= opsize;     // stuck on invalid opcode
    }
    return 4;
}

//------------------------------------------------------------------------------
bool
z80::test_flags(ubyte expected) const {
    // mask out undocumented flags
    ubyte undoc = ~(YF|XF);
    return (F & undoc) == expected;
}

//------------------------------------------------------------------------------
void
z80::halt() {
    HALT = true;
    PC--;
}

//------------------------------------------------------------------------------
void
z80::rst(ubyte vec) {
    SP -= 2;
    mem.w16(SP, PC);
    WZ = PC = (uword) vec;
}

//------------------------------------------------------------------------------
void
z80::irq(void* userdata) {
    z80* self = (z80*) userdata;
    self->irq_received = true;
}

//------------------------------------------------------------------------------
int
z80::handle_irq() {
    int tstates = 0;
    if (this->irq_received) {
        tstates += 2;
        // we don't implement MODE0 or MODE1 (yet?)
        YAKC_ASSERT(2 == this->IM);

        // leave halt
        if (this->HALT) {
            this->HALT = false;
            this->PC++;
        }

        // NOTE: currently there's no timeout on an interrupt request if
        // interrupts are disabled for too long, immediately cancelling
        // the request doesn't work right (has problems with some KC
        // games which have high-frequency interrupts for playing sound)
        if (this->irq_device) {
            if (this->IFF1) {
                this->irq_received = false;
                // handle interrupt
                this->IFF1 = this->IFF2 = false;
                ubyte vec = this->irq_device->interrupt_acknowledged();
                uword addr = (this->I<<8)|(vec&0xFE);
                this->SP -= 2;
                this->mem.w16(this->SP, this->PC);
                this->PC = this->mem.r16(addr);
                tstates += 19;
            }
        }
        this->WZ = this->PC;
    }
    return tstates;
}

//------------------------------------------------------------------------------
void
z80::reti() {
    // this is the same as RET
    WZ = PC = mem.r16(SP);
    SP += 2;
    // ...notify daisy chain, if configured
    if (this->irq_device) {
        this->irq_device->reti();
    }
}

//------------------------------------------------------------------------------
void
z80::ei() {
    // NOTE: interrupts are actually enabled *after the next instruction*,
    // thus we only set a flag here, the IFF flags will then be changed
    // during the next instruction decode in step()
    this->enable_interrupt = true;
}

//------------------------------------------------------------------------------
void
z80::di() {
    IFF1 = false;
    IFF2 = false;
}

//------------------------------------------------------------------------------
ubyte
z80::in(uword port) {
    if (in_func) {
        return in_func(inout_userdata, port);
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
void
z80::out(uword port, ubyte val) {
    if (out_func) {
        out_func(inout_userdata, port, val);
    }
}

//------------------------------------------------------------------------------
ubyte
z80::sziff2(ubyte val, bool iff2) {
    ubyte f = YAKC_SZ(val);
    f |= (val & (YF|XF));
    if (iff2) f |= PF;
    return f;
}

//------------------------------------------------------------------------------
uword
z80::add16(uword acc, uword val) {
    WZ = acc+1;
    uint32_t res = acc + val;
    // flag computation taken from MAME
    F = (F & (SF|ZF|VF)) |
        (((acc^res^val)>>8)&HF)|
        ((res>>16) & CF) | ((res >> 8) & (YF|XF));
    return (uword)res;
}

//------------------------------------------------------------------------------
uword
z80::adc16(uword acc, uword val) {
    WZ = acc+1;
    uint32_t res = acc + val + (F & CF);
    // flag computation taken from MAME
    F = (((acc^res^val)>>8)&HF) |
        ((res>>16)&CF) |
        ((res>>8)&(SF|YF|XF)) |
        ((res & 0xFFFF) ? 0 : ZF) |
        (((val^acc^0x8000) & (val^res)&0x8000)>>13);
    return res;
}

//------------------------------------------------------------------------------
uword
z80::sbc16(uword acc, uword val) {
    WZ = acc+1;
    uint32_t res = acc - val - (F & CF);
    // flag computation taken from MAME
    F = (((acc^res^val)>>8)&HF) | NF |
        ((res>>16)&CF) |
        ((res>>8) & (SF|YF|XF)) |
        ((res & 0xFFFF) ? 0 : ZF) |
        (((val^acc) & (acc^res)&0x8000)>>13);
    return res;
}

//------------------------------------------------------------------------------
void
z80::ldi() {
    ubyte val = mem.r8(HL);
    mem.w8(DE, val);
    val += A;
    ubyte f = F & (SF|ZF|CF);
    if (val & 0x02) f |= YF;
    if (val & 0x08) f |= XF;
    HL++;
    DE++;
    BC--;
    if (BC) {
        f |= VF;
    }
    F = f;
}

//------------------------------------------------------------------------------
int
z80::ldir() {
    ldi();
    if (BC != 0) {
        PC -= 2;
        WZ = PC + 1;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
void
z80::ldd() {
    ubyte val = mem.r8(HL);
    mem.w8(DE, val);
    val += A;
    ubyte f = F & (SF|ZF|CF);
    if (val & 0x02) f |= YF;
    if (val & 0x08) f |= XF;
    HL--;
    DE--;
    BC--;
    if (BC) {
        f |= VF;
    }
    F = f;
}

//------------------------------------------------------------------------------
int
z80::lddr() {
    ldd();
    if (BC != 0) {
        PC -= 2;
        WZ = PC + 1;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
void
z80::cpi() {
    int r = int(A) - int(mem.r8(HL));
    ubyte f = NF | (F & CF) | YAKC_SZ(r);
    if ((r & 0xF) > (A & 0xF)) {
        f |= HF;
        r--;
    }
    if (r & 0x02) f |= YF;
    if (r & 0x08) f |= XF;
    WZ++;
    HL++;
    BC--;
    if (BC) {
        f |= VF;
    }
    F = f;
}

//------------------------------------------------------------------------------
int
z80::cpir() {
    cpi();
    if ((BC != 0) && !(F & ZF)) {
        PC -= 2;
        WZ = PC + 1;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
void
z80::cpd() {
    int r = int(A) - int(mem.r8(HL));
    ubyte f = NF | (F & CF) | YAKC_SZ(r);
    if ((r & 0xF) > (A & 0xF)) {
        f |= HF;
        r--;
    }
    if (r & 0x02) f |= YF;
    if (r & 0x08) f |= XF;
    WZ--;
    HL--;
    BC--;
    if (BC) {
        f |= VF;
    }
    F = f;
}

//------------------------------------------------------------------------------
int
z80::cpdr() {
    cpd();
    if ((BC != 0) && !(F & ZF)) {
        PC -= 2;
        WZ = PC + 1;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
ubyte
z80::ini_ind_flags(ubyte io_val, int c_add) {
    // NOTE: most INI flag settings are undocumented in the official
    // docs, so this is taken from MAME, there's also more
    // information here: http://www.z80.info/z80undoc3.txt
    ubyte f = B ? B & SF : ZF;
    if (io_val & SF) f |= NF;
    uint32_t t = (uint32_t)((C+c_add)&0xFF) + (uint32_t)io_val;
    if (t & 0x100) f |= HF|CF;
    f |= szp[ubyte(t & 0x07)^B] & PF;
    return f;
}

//------------------------------------------------------------------------------
void
z80::ini() {
    ubyte io_val = in(BC);
    WZ = BC + 1;
    B--;
    mem.w8(HL++, io_val);
    F = ini_ind_flags(io_val, +1);
}

//------------------------------------------------------------------------------
int
z80::inir() {
    ini();
    if (B != 0) {
        PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
void
z80::ind() {
    ubyte io_val = in(BC);
    WZ = BC - 1;
    B--;
    mem.w8(HL--, io_val);
    F = ini_ind_flags(io_val, -1);
}

//------------------------------------------------------------------------------
int
z80::indr() {
    ind();
    if (B != 0) {
        PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
ubyte
z80::outi_outd_flags(ubyte io_val) {
    // NOTE: most OUTI flag settings are undocumented in the official
    // docs, so this is taken from MAME, there's also more
    // information here: http://www.z80.info/z80undoc3.txt
    ubyte f = B ? B & SF : ZF;
    if (io_val & SF) f |= NF;
    uint32_t t = (uint32_t)L + (uint32_t)io_val;
    if (t & 0x100) f |= HF|CF;
    f |= szp[ubyte(t & 0x07)^B] & PF;
    return f;
}

//------------------------------------------------------------------------------
void
z80::outi() {
    ubyte io_val = mem.r8(HL++);
    B--;
    WZ = BC + 1;
    out(BC, io_val);
    F = outi_outd_flags(io_val);
}

//------------------------------------------------------------------------------
int
z80::otir() {
    outi();
    if (B != 0) {
        PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
void
z80::outd() {
    ubyte io_val = mem.r8(HL--);
    B--;
    WZ = BC - 1;
    out(BC, io_val);
    F = outi_outd_flags(io_val);
}

//------------------------------------------------------------------------------
int
z80::otdr() {
    outd();
    if (B != 0) {
        PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
void
z80::daa() {
    // from MAME and http://www.z80.info/zip/z80-documented.pdf
    ubyte val = A;
    if (F & NF) {
        if (((A & 0xF) > 0x9) || (F & HF)) {
            val -= 0x06;
        }
        if ((A > 0x99) || (F & CF)) {
            val -= 0x60;
        }
    }
    else {
        if (((A & 0xF) > 0x9) || (F & HF)) {
            val += 0x06;
        }
        if ((A > 0x99) || (F & CF)) {
            val += 0x60;
        }
    }
    F &= CF|NF;
    F |= (A > 0x99) ? CF:0;
    F |= (A^val) & HF;
    F |= szp[val];
    A = val;
}

//------------------------------------------------------------------------------
void
z80::rld() {
    WZ = HL;
    ubyte x = mem.r8(WZ++);
    ubyte tmp = A & 0xF;              // store A low nibble
    A = (A & 0xF0) | (x>>4);    // move (HL) high nibble into A low nibble
    x = (x<<4) | tmp;   // move (HL) low to high nibble, move A low nibble to (HL) low nibble
    mem.w8(HL, x);
    F = szp[A] | (F & CF);
}

//------------------------------------------------------------------------------
void
z80::rrd() {
    WZ = HL;
    ubyte x = mem.r8(WZ++);
    ubyte tmp = A & 0xF;                  // store A low nibble
    A = (A & 0xF0) | (x & 0x0F);    // move (HL) low nibble to A low nibble
    x = (x >> 4) | (tmp << 4);  // move A low nibble to (HL) high nibble, and (HL) high nibble to (HL) low nibble
    mem.w8(HL, x);
    F = szp[A] | (F & CF);
}

} // namespace YAKC
