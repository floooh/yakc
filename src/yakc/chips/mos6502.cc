//------------------------------------------------------------------------------
//  mos6502.cc
//------------------------------------------------------------------------------
#include "mos6502.h"

namespace YAKC {

//------------------------------------------------------------------------------
//  address mode table [cc][bbb][aaa]
//
using namespace mos6502_enums;

// opcode descriptions
mos6502::op_desc mos6502::ops[4][8][8] = {
// cc = 00
{
//---         BIT          JMP          JMP()        STY          LDY          CPY          CPX
{{A____,M___},{A_JSR,M_R_},{A____,M_R_},{A____,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_}},
{{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M__W},{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M_R_}},
{{A____,M___},{A____,M___},{A____,M__W},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___}},
{{A_ABS,M_R_},{A_ABS,M_R_},{A_JMP,M_R_},{A_JMP,M_R_},{A_ABS,M__W},{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M_R_}},
{{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_}},  // relative branches
{{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M__W},{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M_R_}},
{{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___}},
{{A_ABX,M_R_},{A_ABX,M_R_},{A_ABS,M_R_},{A_ABS,M_R_},{A_INV,M___},{A_ABX,M_R_},{A_ABX,M_R_},{A_ABX,M_R_}}
},
// cc = 01
{
//ORA         AND          EOR          ADC          STA          LDA          CMP          SBC
{{A_IDX,M_R_},{A_IDX,M_R_},{A_IDX,M_R_},{A_IDX,M_R_},{A_IDX,M__W},{A_IDX,M_R_},{A_IDX,M_R_},{A_IDX,M_R_}},
{{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M__W},{A_ZER,M_R_},{A_ZER,M_R_},{A_ZER,M_R_}},
{{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_}},
{{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M__W},{A_ABS,M_R_},{A_ABS,M_R_},{A_ABS,M_R_}},
{{A_IDY,M_R_},{A_IDY,M_R_},{A_IDY,M_R_},{A_IDY,M_R_},{A_IDY,M__W},{A_IDY,M_R_},{A_IDY,M_R_},{A_IDY,M_R_}},
{{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M__W},{A_ZPX,M_R_},{A_ZPX,M_R_},{A_ZPX,M_R_}},
{{A_ABY,M_R_},{A_ABY,M_R_},{A_ABY,M_R_},{A_ABY,M_R_},{A_ABY,M__W},{A_ABY,M_R_},{A_ABY,M_R_},{A_ABY,M_R_}},
{{A_ABX,M_R_},{A_ABX,M_R_},{A_ABX,M_R_},{A_ABX,M_R_},{A_ABX,M__W},{A_ABX,M_R_},{A_ABX,M_R_},{A_ABX,M_R_}},
},
// cc = 02
{
//ASL         ROL          LSR          ROR          STX          LDX          DEC          INC
{{A_INV,M_RW},{A_INV,M_RW},{A_INV,M_RW},{A_INV,M_RW},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_},{A_IMM,M_R_}},
{{A_ZER,M_RW},{A_ZER,M_RW},{A_ZER,M_RW},{A_ZER,M_RW},{A_ZER,M__W},{A_ZER,M_R_},{A_ZER,M_RW},{A_ZER,M_RW}},
{{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___},{A____,M___}},
{{A_ABS,M_RW},{A_ABS,M_RW},{A_ABS,M_RW},{A_ABS,M_RW},{A_ABS,M__W},{A_ABS,M_R_},{A_ABS,M_RW},{A_ABS,M_RW}},
{{A_INV,M_RW},{A_INV,M_RW},{A_INV,M_RW},{A_INV,M_RW},{A_INV,M__W},{A_INV,M_R_},{A_INV,M_RW},{A_INV,M_RW}},
{{A_ZPX,M_RW},{A_ZPX,M_RW},{A_ZPX,M_RW},{A_ZPX,M_RW},{A_ZPY,M__W},{A_ZPY,M_R_},{A_ZPX,M_RW},{A_ZPX,M_RW}},
{{A____,M_R_},{A____,M_R_},{A____,M_R_},{A____,M_R_},{A____,M___},{A____,M___},{A____,M_R_},{A____,M_R_}},
{{A_ABX,M_RW},{A_ABX,M_RW},{A_ABX,M_RW},{A_ABX,M_RW},{A_INV,M__W},{A_ABY,M_R_},{A_ABX,M_RW},{A_ABX,M_RW}},
},
// cc = 03
{
{{A_IDX,M_RW},{A_IDX,M_RW},{A_IDX,M_RW},{A_IDX,M_RW},{A_IDX,M__W},{A_IDX,M_R_},{A_IDX,M_RW},{A_IDX,M_RW}},
{{A_ZER,M_RW},{A_ZER,M_RW},{A_ZER,M_RW},{A_ZER,M_RW},{A_ZER,M__W},{A_ZER,M_R_},{A_ZER,M_RW},{A_ZER,M_RW}},
{{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_INV,M___},{A_IMM,M_R_}},
{{A_ABS,M_RW},{A_ABS,M_RW},{A_ABS,M_RW},{A_ABS,M_RW},{A_ABS,M__W},{A_ABS,M_R_},{A_ABS,M_RW},{A_ABS,M_RW}},
{{A_IDY,M_RW},{A_IDY,M_RW},{A_IDY,M_RW},{A_IDY,M_RW},{A_INV,M___},{A_IDY,M_R_},{A_IDY,M_RW},{A_IDY,M_RW}},
{{A_ZPX,M_RW},{A_ZPX,M_RW},{A_ZPX,M_RW},{A_ZPX,M_RW},{A_ZPY,M__W},{A_ZPY,M_R_},{A_ZPX,M_RW},{A_ZPX,M_RW}},
{{A_ABY,M_RW},{A_ABY,M_RW},{A_ABY,M_RW},{A_ABY,M_RW},{A_INV,M___},{A_INV,M___},{A_ABY,M_RW},{A_ABY,M_RW}},
{{A_ABX,M_RW},{A_ABX,M_RW},{A_ABX,M_RW},{A_ABX,M_RW},{A_INV,M___},{A_ABY,M_R_},{A_ABX,M_RW},{A_ABX,M_RW}}
}
};

//------------------------------------------------------------------------------
mos6502::mos6502() {
    this->init(nullptr);
}

//------------------------------------------------------------------------------
void
mos6502::init(system_bus* bus_) {
    bus = bus_;
    bcd_enabled = true;
    A = 0; X = 0; Y = 0; S = 0; P = XF; PC = 0;
    IR = 0;
    RW = true;
    Cycle = 0;
    AddrMode = A____;
    MemAccess = M___;
    IRQ = false;
    NMI = false;
    irq_taken = false;
}

//------------------------------------------------------------------------------
void
mos6502::reset() {
    P = (IF | XF);
    S = 0xFD;
    PC = mem.r16io(0xFFFC);
    RW = true;
    Cycle = 0;
    AddrMode = A____;
    MemAccess = M___;
    IRQ = false;
    NMI = false;
    irq_taken = false;
}

//------------------------------------------------------------------------------
void
mos6502::addr() {
    // execute the various addressing mode steps, result
    // is the address on the address bus (ADDR)
    uint8_t cc  = IR & 0x03;
    uint8_t bbb = (IR >> 2) & 0x07;
    uint8_t aaa = (IR >> 5) & 0x07;
    AddrMode  = ops[cc][bbb][aaa].addr;
    MemAccess = ops[cc][bbb][aaa].mem;    
    switch (AddrMode) {
        //-- no addressing, this still puts the PC on the ADDR bus
        //-- without incrementing the PC, so the next instruction
        //-- byte is read
        case A____:
            //--
            ADDR = PC;
            break;
        //-- immediate mode, special JSR mode
        case A_IMM:
            //--
            ADDR = PC++;
            break;
        //-- zero page
        case A_ZER:
            //--
            ADDR = PC++;
            read();
            //--
            ADDR = DATA;
            break;
        //-- zero page + X
        case A_ZPX:
            //--
            ADDR = PC++;
            read();
            //--
            ADDR = DATA;
            read();
            //--
            ADDR = (ADDR + X) & 0x00FF;
            break;
        //-- zero page + Y
        case A_ZPY:
            //--
            ADDR = PC++;
            read();
            //--
            ADDR = DATA;
            read();
            //--
            ADDR = (ADDR + Y) & 0x00FF;
            break;
        //--- absolute
        case A_ABS:
            //--
            ADDR = PC++;
            read();
            //--
            tmp16 = DATA; ADDR = PC++;
            read();
            //--
            ADDR = DATA<<8 | tmp16;
            break;
        //--- absolute + X
        case A_ABX:
            //--
            ADDR = PC++;
            read();
            //--
            tmp16 = DATA + X; ADDR = PC++;
            read();
            //--
            ADDR = (DATA<<8) | (tmp16&0xFF);
            if (((tmp16 & 0xFF00) == 0x0000) && (MemAccess == M_R_)) {
                // page boundary was not crossed, and not store, can exit early
                break;
            }
            read();
            //-- page boundary was crossed, add carry and read again
            ADDR = (ADDR & 0xFF00) + tmp16;
            break;

        //--- absolute + Y
        case A_ABY:
            //--
            ADDR = PC++;
            read();
            //--
            tmp16 = DATA + Y; ADDR = PC++;
            read();
            //--
            ADDR = (DATA<<8) | (tmp16&0xFF);
            if (((tmp16 & 0xFF00) == 0x0000) && (MemAccess == M_R_)) {
                // page boundary was not crossed, and not store, can exit early
                break;
            }
            read();
            //-- page boundary was crossed, add carry and read again
            ADDR = (ADDR & 0xFF00) + tmp16;
            break;
        //--- (zp,X)
        case A_IDX:
            //--
            ADDR = PC++;
            read();
            //--
            ADDR = DATA;
            read();
            //--
            ADDR = (ADDR + X) & 0x00FF;
            read();
            //--
            tmp16 = DATA; ADDR = (ADDR + 1) & 0x00FF;
            read();
            //--
            ADDR = (DATA<<8) | tmp16;
            break;
        //--- (zp),Y
        case A_IDY:
            //--
            ADDR = PC++;
            read();
            //--
            ADDR = DATA;
            read();
            //--
            tmp16 = DATA + Y; ADDR = (ADDR + 1) & 0x00FF;
            read();
            //--
            ADDR = (DATA<<8) | (tmp16&0xFF);
            if (((tmp16 & 0xFF00) == 0x0000) && (MemAccess == M_R_)) {
                // page boundary was not crossed, and not store, can exit early
                break;
            }
            read();
            //-- page boundary was crossed, add carry and read again
            ADDR = (ADDR & 0xFF00) + tmp16;
            break;
        //--- JMP, just load the next two bytes into ADDR
        case A_JMP:
            //--
            ADDR = PC++;
            read();
            //--
            tmp16 = DATA; ADDR = PC++;
            break;
        //--- JSR, this is weird, reads one byte from PC++, then writes
        //--- PC to stack before reading the next target address byte
        case A_JSR:
            //--
            ADDR = PC++;
            break;
        //--- can't happen
        default: YAKC_ASSERT(false); break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::exec() {
    uint8_t cc  = IR & 0x03;
    uint8_t aaa = (IR >> 5) & 0x07;
    uint8_t bbb = (IR >> 2) & 0x07;
    switch (cc) {
        case 0:
            switch (aaa) {
                case 0:
                    switch (bbb) {
                        case 0:  this->brk(); break;        // 0x00: aaa=000 bbb=000 cc=00
                        case 1:  this->u_nop(); break;      // 0x04: undoc DOP zp
                        case 2:  this->php(); break;        // 0x08: aaa=000 bbb=010 cc=00
                        case 3:  this->u_nop(); break;      // 0x0C: undoc TOP abs
                        case 4:  this->br(NF, 0); break;    // 0x10: aaa=000 bbb=100 cc=00 (BPL)
                        case 5:  this->u_nop(); break;      // 0x14: undoc DOP zp,X
                        case 6:  this->cl(CF); break;       // 0x18: aaa=000 bbb=110 cc=00
                        case 7:  this->u_nop(); break;      // 0x1C: undoc TOP abs,X
                    }
                    break;
                case 1:
                    switch (bbb) {
                        case 0:  this->jsr(); break;        // 0x20: aaa=001 bbb=000 cc=00
                        case 2:  this->plp(); break;        // 0x28: aaa=001 bbb=010 cc=00
                        case 4:  this->br(NF, NF); break;   // 0x30: aaa=001 bbb=100 cc=00 (BMI)
                        case 5:  this->u_nop(); break;      // 0x34: undoc DOP zp,X
                        case 6:  this->se(CF); break;       // 0x38: aaa=001 bbb=110 cc=00
                        case 7:  this->u_nop(); break;      // 0x3C: undoc TOP abs,X
                        default: this->bit(); break;
                    }
                    break;
                case 2:
                    switch (bbb) {
                        case 0:  this->rti(); break;        // 0x40: aaa=010 bbb=000 cc=00
                        case 1:  this->u_nop(); break;      // 0x44: undoc DOP zp
                        case 2:  this->pha(); break;        // 0x48: aaa=010 bbb=010 cc=00
                        case 3:  this->jmp(); break;        // 0x4C: aaa=010 bbb=011 cc=00
                        case 4:  this->br(VF, 0); break;    // 0x50: aaa=010 bbb=100 cc=00 (BVC)
                        case 5:  this->u_nop(); break;      // 0x54: undoc DOP zp,X
                        case 6:  this->cl(IF); break;       // 0x58: aaa=010 bbb=110 cc=00
                        case 7:  this->u_nop(); break;      // 0x5C: undoc TOP abs,X
                    }
                    break;
                case 3:
                    switch (bbb) {
                        case 0:  this->rts(); break;        // 0x60: aaa=011 bbb=000 cc=00
                        case 1:  this->u_nop(); break;      // 0x64: undoc DOP zp
                        case 2:  this->pla(); break;        // 0x68: aaa=011 bbb=010 cc=00
                        case 3:  this->jmpi(); break;       // 0x6C: aaa=011 bbb=011 cc=00
                        case 4:  this->br(VF, VF); break;   // 0x70: aaa=011 bbb=100 cc=00 (BVS)
                        case 5:  this->u_nop(); break;      // 0x74: undoc DOP zx,X
                        case 6:  this->se(IF); break;       // 0x78: aaa=011 bbb=110 cc=00
                        case 7:  this->u_nop(); break;      // 0x7C: undoc TOP abs,X
                    }
                    break;
                case 4:
                    switch (bbb) {
                        case 0:  this->u_nop(); break;      // 0x80: undoc DOP #
                        case 2:  this->dey(); break;        // 0x88: aaa=100 bbb=010 cc=00
                        case 4:  this->br(CF, 0); break;    // 0x90: aaa=100 bbb=100 cc=00 (BCC)
                        case 6:  this->tya(); break;        // 0x98: aaa=100 bbb=110 cc=00
                        default: this->sty(); break;
                    }
                    break;
                case 5:
                    switch (bbb) {
                        case 2:  this->tay(); break;        // 0xA8: aaa=101 bbb=010 cc=00
                        case 4:  this->br(CF, CF); break;   // 0xB0: aaa=101 bbb=100 cc=00 (BCS)
                        case 6:  this->cl(VF); break;       // 0xB8: aaa=101 bbb=110 cc=00
                        default: this->ldy(); break;
                    }
                    break;
                case 6:
                    switch (bbb) {
                        case 2:  this->iny(); break;        // 0xC8: aaa=110 bbb=010 cc=00
                        case 4:  this->br(ZF, 0); break;    // 0xD0: aaa=110 bbb=100 cc=00 (BNE)
                        case 5:  this->u_nop(); break;      // 0xD4: undoc NOP zp,X
                        case 6:  this->cl(DF); break;       // 0xD8: aaa=110 bbb=110 cc=00
                        case 7:  this->u_nop(); break;      // 0xDC: undoc TOP abs,X
                        default: this->cpy(); break;
                    }
                    break;
                case 7:
                    switch (bbb) {
                        case 2:  this->inx(); break;        // 0xE8: aaa=111 bbb=010 cc=00
                        case 4:  this->br(ZF, ZF); break;   // 0xF0: aaa=111 bbb=100 cc=00 (BEQ)
                        case 5:  this->u_nop(); break;      // 0xF4: undoc NOP zp,X
                        case 6:  this->se(DF); break;       // 0xF8: aaa=111 bbb=110 cc=00
                        case 7:  this->u_nop(); break;      // 0xFC: undoc TOP abs,X
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
                case 4:
                    switch (bbb) {
                        case 2:  this->u_nop(); break;  // 0x89: undoc NOP #
                        default: this->sta(); break;
                    }
                    break;
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
                        case 6:  this->u_nop(); break;  // 0x1A: undoc NOP
                        default: this->asl(); break;
                    }
                    break;
                case 1:
                    switch (bbb) {
                        case 2:  this->rola(); break;
                        case 6:  this->u_nop(); break;  // 0x3A: undoc NOP
                        default: this->rol(); break;
                    }
                    break;
                case 2:
                    switch (bbb) {
                        case 2:  this->lsra(); break;
                        case 6:  this->u_nop(); break;  // 0x5A: undoc NOP
                        default: this->lsr(); break;
                    }
                    break;
                case 3:
                    switch (bbb) {
                        case 2:  this->rora(); break;
                        case 6:  this->u_nop(); break;  // 0x7A: undoc NOP
                        default: this->ror(); break;
                    }
                    break;
                case 4:
                    switch (bbb) {
                        case 0:  this->u_nop(); break;  // 0x82: undoc DOP #
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
                        case 0:  this->u_nop(); break;  // 0xC2: undoc NOP #
                        case 2:  this->dex(); break;    // 0xCA: aaa=110 bbb=010 cc=10
                        case 6:  this->u_nop(); break;  // 0xDA: undoc NOP
                        default: this->dec(); break;
                    }
                    break;
                case 7:
                    switch (bbb) {
                        case 0:  this->u_nop(); break;  // 0xE2: undoc NOP #
                        case 2:  this->nop(); break;    // 0xEA: aaa=111 bbb=010 cc=10
                        case 6:  this->u_nop(); break;  // 0xFA: undoc NOP
                        default: this->inc(); break;
                    }
                    break;
            }
            break;

        case 3:
            switch (aaa) {
                case 0: this->u_slo(); break;   // undoc SLO
                case 1: this->u_rla(); break;   // undoc RLA
                case 2: this->u_sre(); break;   // undoc SRE
                case 3: this->u_rra(); break;   // undoc RRA
                case 4: this->u_sax(); break;   // undoc SAX
                case 5: this->u_lax(); break;   // undoc LAX
                case 6: this->u_dcp(); break;   // undoc DCP
                case 7:
                    switch (bbb) {
                        case 2:  this->u_sbc(); break;  // undoc SBC #
                        default: this->u_isb(); break;  // undoc ISB
                    }
                    break;
            }
    }
}

} // namespace YAKC
