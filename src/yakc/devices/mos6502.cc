//------------------------------------------------------------------------------
//  mos6502.cc
//------------------------------------------------------------------------------
#include "mos6502.h"
#include "yakc/core/system_bus.h"

namespace YAKC {

//------------------------------------------------------------------------------
//  address mode table [cc][bbb][aaa]
//
// addressing modes
enum {
    A____,      // no addressing mode
    A_IMM,      // #
    A_ZER,      // zp
    A_ZPX,      // zp,X
    A_ZPY,      // zp,Y
    A_ABS,      // abs
    A_ABX,      // abs,X
    A_ABY,      // abs,Y
    A_IDX,      // (zp,X)
    A_IDY,      // (zp),Y

    A_INV,      // this is an invalid instruction
};
// memory addressing modes
enum {
    M___,       // no memory access
    M_R_,        // read
    M__W,        // write in current cycle
    M_RW,       // read-modify-write
};

// opcode descriptions
mos6502::op_desc mos6502::ops[4][8][8] = {
// cc = 00
{
//---         BIT          JMP          JMP()        STY          LDY          CPY          CPX
{{A____,M___},{A____,M_R_},{A____,M_R_},{A____,M_R_},{A_INV,M___},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_}},
{{A_INV,M___},{A_ZER,M_R_},{A_INV,M___},{A_INV,M___},{A_ZER,M__W},{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M_R_}},
{{A____,M___},{A____,M___},{A____,M__W},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___}},
{{A_INV,M___},{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M__W},{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M_R_}},
{{A____,M___},{A____,M_R_},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___}},
{{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_ZPX,M__W},{A_ZPX,M_R_},{A_INV,M___},{A_INV,M___}},
{{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___}},
{{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_ABX,M_R_},{A_INV,M___},{A_INV,M___}}
},
// cc = 01
{
//ORA         AND          EOR          ADC          STA          LDA          CMP          SBC
{{A_IDX,M_R_},{A_IDX,M_R_},{A_IDX,M_R_},{A_IDX,M_R_},{A_IDX,M__W},{A_IDX,M_R_},{A_IDX,M_R_},{A_IDX,M_R_}},
{{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M__W},{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M_R_}},
{{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_INV,M__W},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_}},
{{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M__W},{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M_R_}},
{{A_IDY,M_R_},{A_IDY,M_R_},{A_IDY,M_R_},{A_IDY,M_R_},{A_IDY,M__W},{A_IDY,M_R_},{A_IDY,M_R_},{A_IDY,M_R_}},
{{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M__W},{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M_R_}},
{{A_ABY,M_R_},{A_ABY,M_R_},{A_ABY,M_R_},{A_ABY,M_R_},{A_ABY,M__W},{A_ABY,M_R_},{A_ABY,M_R_},{A_ABY,M_R_}},
{{A_ABX,M_R_},{A_ABX,M_R_},{A_ABX,M_R_},{A_ABX,M_R_},{A_ABX,M__W},{A_ABX,M_R_},{A_ABX,M_R_},{A_ABX,M_R_}},
},
// cc = 02
{
//ASL         ROL          LSR          ROR          STX          LDX          DEC          INC
{{A_INV,M_RW},{A_INV,M_RW},{A_INV,M_RW},{A_INV,M_RW},{A_INV,M__W},{A_IMM,M_R_},{A_INV,M_RW},{A_INV,M_RW}},
{{A_ZER,M_RW},{A_ZER,M_RW},{A_ZER,M_RW},{A_ZER,M_RW},{A_ZER,M__W},{A_ZER,M_R_},{A_ZER,M_RW},{A_ZER,M_RW}},
{{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___}},
{{A_ABS,M_RW},{A_ABS,M_RW},{A_ABS,M_RW},{A_ABS,M_RW},{A_ABS,M__W},{A_ABS,M_R_},{A_ABS,M_RW},{A_ABS,M_RW}},
{{A_INV,M_RW},{A_INV,M_RW},{A_INV,M_RW},{A_INV,M_RW},{A_INV,M__W},{A_INV,M_R_},{A_INV,M_RW},{A_INV,M_RW}},
{{A_ZPX,M_RW},{A_ZPX,M_RW},{A_ZPX,M_RW},{A_ZPX,M_RW},{A_ZPY,M__W},{A_ZPY,M_R_},{A_ZPX,M_RW},{A_ZPX,M_RW}},
{{A_INV,M_RW},{A_INV,M_RW},{A_INV,M_RW},{A_INV,M_RW},{A____,M___},{A____,M___},{A_INV,M_RW},{A_INV,M_RW}},
{{A_ABX,M_RW},{A_ABX,M_RW},{A_ABX,M_RW},{A_ABX,M_RW},{A_INV,M__W},{A_ABY,M_R_},{A_ABX,M_RW},{A_ABX,M_RW}},
},
// cc = 03
{
{{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___}},
{{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___}},
{{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___}},
{{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___}},
{{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___}},
{{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___}},
{{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___}},
{{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___}}
}
};

//------------------------------------------------------------------------------
mos6502::mos6502() {
    this->init();
}

//------------------------------------------------------------------------------
void
mos6502::init() {
    A = 0; X = 0; Y = 0; S = 0; P = XF; PC = 0;
    OP = 0;
    RW = true;
    Cycle = 0;
    Fetch = true;
    AddrCycle = 0;
    ExecCycle = 0;
    AddrMode = A____;
    MemAccess = M___;
}

//------------------------------------------------------------------------------
void
mos6502::reset() {
    P = (IF | XF);
    S = 0xFD;
    PC = mem.r16(0xFFFC);
    RW = true;
    Cycle = 0;
    Fetch = true;
    AddrCycle = 0;
    ExecCycle = 0;
    AddrMode = A____;
    MemAccess = M___;
}

//------------------------------------------------------------------------------
uint32_t
mos6502::step_op(system_bus* bus) {
    do {
        this->step(bus);
    }
    while (!Fetch);
    return Cycle;
}

//------------------------------------------------------------------------------
void
mos6502::step(system_bus* bus) {
    if (Fetch) {
        this->step_fetch();
    }
    if (RW) {
        DATA = mem.r8(ADDR);
    }
    if (Fetch) {
        step_decode();
    }
    if (AddrCycle > 0) {
        step_addr();
    }
    if (ExecCycle > 0) {
        step_exec();
    }
    if (!RW) {
        mem.w8(ADDR, DATA);
    }
    Cycle++;
}

//------------------------------------------------------------------------------
void
mos6502::step_fetch() {
    this->ADDR = PC++;
    RW = true;
    AddrCycle = 0;
    ExecCycle = 0;
    Cycle = 0;
}

//------------------------------------------------------------------------------
void
mos6502::step_decode() {
    //  - determine addressing mode
    //  - determine memory acces (read, write, read/write)
    OP = DATA;
    uint8_t cc  = OP & 0x03;
    uint8_t bbb = (OP >> 2) & 0x07;
    uint8_t aaa = (OP >> 5) & 0x07;
    AddrMode  = ops[cc][bbb][aaa].addr;
    MemAccess = ops[cc][bbb][aaa].mem;
    Fetch = false;
    AddrCycle = 1;
}

//------------------------------------------------------------------------------
void
mos6502::step_addr() {
    bool done = false;
    switch (AddrMode) {
        //-- no addressing, this still puts the PC on the ADDR bus
        //-- without incrementing the PC, so the same instruction
        //-- byte is read 2x
        case A____:
            ADDR = PC; done = true; break;
            break;
        //-- immediate mode
        case A_IMM:
            ADDR = PC++; done = true; break;
            break;
        //-- zero page
        case A_ZER:
            switch (AddrCycle) {
                case 1: ADDR = PC++; break;
                case 2: ADDR = DATA; done = true; break;
            }
            break;
        //-- zero page + X
        case A_ZPX:
            switch (AddrCycle) {
                case 1: ADDR = PC++; break;
                case 2: ADDR = DATA; break;
                case 3: ADDR = (ADDR + X) & 0x00FF; done = true; break;
            }
            break;
        //-- zero page + Y
        case A_ZPY:
            switch (AddrCycle) {
                case 1: ADDR = PC++; break;
                case 2: ADDR = DATA; break;
                case 3: ADDR = (ADDR + Y) & 0x00FF; done = true; break;
            }
            break;
        //--- absolute
        case A_ABS:
            switch (AddrCycle) {
                case 1: ADDR = PC++; break;
                case 2: tmp16 = DATA; ADDR = PC++; break;
                case 3: ADDR = DATA<<8 | tmp16; done = true; break;
            }
            break;
        //--- absolute + X
        case A_ABX:
            switch (AddrCycle) {
                case 1: ADDR = PC++; break;
                case 2: tmp16 = DATA + X; ADDR = PC++; break;
                case 3:
                    ADDR = (DATA<<8) | (tmp16&0xFF);
                    if (((tmp16 & 0xFF00) == 0x0000) && (MemAccess == M_R_)) {
                        // page boundary was not crossed, and not store, can exit early
                        done = true;
                    }
                    break;
                case 4:
                    // page boundary was crossed, add carry and read again
                    ADDR = (ADDR & 0xFF00) + tmp16;
                    done = true;
                    break;
            }
            break;
        //--- absolute + Y
        case A_ABY:
            switch (AddrCycle) {
                case 1: ADDR = PC++; break;
                case 2: tmp16 = DATA + Y; ADDR = PC++; break;
                case 3:
                    ADDR = (DATA<<8) | (tmp16&0xFF);
                    if (((tmp16 & 0xFF00) == 0x0000) && (MemAccess == M_R_)) {
                        // page boundary was not crossed, and not store, can exit early
                        done = true;
                    }
                    break;
                case 4:
                    // page boundary was crossed, add carry and read again
                    ADDR = (ADDR & 0xFF00) + tmp16;
                    done = true;
                    break;
            }
            break;
        //--- (zp,X)
        case A_IDX:
            switch (AddrCycle) {
                case 1: ADDR = PC++; break;
                case 2: ADDR = DATA; break;
                case 3: ADDR = (ADDR + X) & 0x00FF; break;
                case 4: tmp16 = DATA; ADDR = (ADDR + 1) & 0x00FF; break;
                case 5: ADDR = (DATA<<8) | tmp16; done = true; break;
            }
            break;
        //--- (zp),Y
        case A_IDY:
            switch (AddrCycle) {
                case 1: ADDR = PC++; break;
                case 2: ADDR = DATA; break;
                case 3: tmp16 = DATA + Y; ADDR = (ADDR + 1) & 0x00FF; break;
                case 4:
                    ADDR = (DATA<<8) | (tmp16&0xFF);
                    if (((tmp16 & 0xFF00) == 0x0000) && (MemAccess == M_R_)) {
                        // page boundary was not crossed, and not store, can exit early
                        done = true;
                    }
                    break;
                case 5:
                    // page boundary was crossed, add carry and read again
                    ADDR = (ADDR & 0xFF00) + tmp16;
                    done = true;
                    break;
            }
            break;
        //--- can't happen
        default: YAKC_ASSERT(false); break;
    }
    AddrCycle++;

    // transition to Exec state?
    if (done) {
        AddrCycle = 0;
        ExecCycle = 1;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_exec() {
    // the first exec cycle is always a read, nothing to do here
    if (ExecCycle > 1) {
        uint8_t cc  = OP & 0x03;
        uint8_t aaa = (OP >> 5) & 0x07;
        uint8_t bbb = (OP >> 2) & 0x07;
        switch (cc) {
            case 0:
                switch (aaa) {
                    case 0:
                        switch (bbb) {
                            case 2:  this->php(); break;    // 0x08: aaa=000 bbb=010 cc=00
                            case 6:  this->cl(CF); break;   // 0x18: aaa=000 bbb=110 cc=00
                        }
                        break;
                    case 1:
                        switch (bbb) {
                            case 2:  this->plp(); break;    // 0x28: aaa=001 bbb=010 cc=00
                            case 6:  this->se(CF); break;   // 0x38: aaa=001 bbb=110 cc=00
                            default: this->bit(); break;
                        }
                        break;
                    case 2:
                        switch (bbb) {
                            case 2:  this->pha(); break;    // 0x48: aaa=010 bbb=010 cc=00
                            case 6:  this->cl(IF); break;   // 0x58: aaa=010 bbb=110 cc=00
                            default: this->jmp(); break;
                        }
                        break;
                    case 3:
                        switch (bbb) {
                            case 2:  this->pla(); break;    // 0x68: aaa=011 bbb=010 cc=00
                            case 6:  this->se(IF); break;   // 0x78: aaa=011 bbb=110 cc=00
                            default: this->jmpi(); break;
                        }
                        break;
                    case 4:
                        switch (bbb) {
                            case 2:  this->dey(); break;    // 0x88: aaa=100 bbb=010 cc=00
                            case 6:  this->tya(); break;    // 0x98: aaa=100 bbb=110 cc=00
                            default: this->sty(); break;
                        }
                        break;
                    case 5:
                        switch (bbb) {
                            case 2:  this->tay(); break;    // 0xA8: aaa=101 bbb=010 cc=00
                            case 6:  this->cl(VF); break;   // 0xB8: aaa=101 bbb=110 cc=00
                            default: this->ldy(); break;
                        }
                        break;
                    case 6:
                        switch (bbb) {
                            case 2:  this->iny(); break;    // 0xC8: aaa=110 bbb=010 cc=00
                            case 6:  this->cl(DF); break;   // 0xD8: aaa=110 bbb=110 cc=00
                            default: this->cpy(); break;
                        }
                        break;
                    case 7:
                        switch (bbb) {
                            case 2:  this->inx(); break;    // 0xE8: aaa=111 bbb=010 cc=00
                            case 6:  this->se(DF); break;   // 0xF8: aaa=111 bbb=110 cc=00
                            default: this->cpx(); break;
                        }
                        break;
                }
                break;

            case 1:
                switch (aaa) {
                    case 0: this->ora(); break;
                    case 1: this->anda(); break;
                    case 2: this->eor(); break;
                    case 3: this->adc(); break;
                    case 4: this->sta(); break;
                    case 5: this->lda(); break;
                    case 6: this->cmp(); break;
                    case 7: this->sbc(); break;
                }
                break;

            case 2:
                switch (aaa) {
                    case 0:
                        switch (bbb) {
                            case 2:  this->asla(); break;
                            default: this->asl(); break;
                        }
                        break;
                    case 1:
                        switch (bbb) {
                            case 2:  this->rola(); break;
                            default: this->rol(); break;
                        }
                        break;
                    case 2:
                        switch (bbb) {
                            case 2:  this->lsra(); break;
                            default: this->lsr(); break;
                        }
                        break;
                    case 3:
                        switch (bbb) {
                            case 2:  this->rora(); break;
                            default: this->ror(); break;
                        }
                        break;
                    case 4:
                        switch (bbb) {
                            case 2:  this->txa(); break;    // 0x8A: aaa=100 bbb=010 cc=10
                            case 6:  this->txs(); break;    // 0x9A: aaa=100 bbb=110 cc=10
                            default: this->stx(); break;
                        }
                        break;
                    case 5:
                        switch (bbb) {
                            case 2:  this->tax(); break;    // 0xAA: aaa=101 bbb=010 cc=10
                            case 6:  this->tsx(); break;    // 0xBA: aaa=101 bbb=110 cc=10
                            default: this->ldx(); break;
                        }
                        break;
                    case 6:
                        switch (bbb) {
                            case 2:  this->dex(); break;    // 0xCA: aaa=110 bbb=010 cc=10
                            default: this->dec(); break;
                        }
                        break;
                    case 7:
                        switch (bbb) {
                            case 2:  this->nop(); break;    // 0xEA: aaa=111 bbb=010 cc=10
                            default: this->inc(); break;
                        }
                        break;
                }
                break;
        }
    }
    ExecCycle++;
}

} // namespace YAKC
