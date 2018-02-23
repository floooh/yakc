//------------------------------------------------------------------------------
//  mos6502dasm.cc
//------------------------------------------------------------------------------
#include "mos6502dasm.h"
#include "chips/m6502.h"
#include <stdio.h>

namespace mos6502dasm {

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
    A_JMP,      // special JMP abs
    A_JSR,      // special JSR abs

    A_INV,      // this is an invalid instruction
};
enum {
    M___,       // no memory access
    M_R_,        // read
    M__W,        // write in current cycle
    M_RW,       // read-modify-write
};
struct op_desc {
    uint8_t addr;       // addressing mode
    uint8_t mem;        // memory access mode
};

// opcode descriptions
op_desc ops[4][8][8] = {
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


int mos6502disasm(fetch_func fetch, unsigned short pc, char* buffer, void* userdata) {

    int pos = 0;
    uint8_t op = fetch(pc, pos++, userdata);
    uint8_t cc  = op & 0x03;
    uint8_t bbb = (op >> 2) & 0x07;
    uint8_t aaa = (op >> 5) & 0x07;
    char* dst = buffer;

    const char* n = "???";
    bool indirect = false;
    switch (cc) {
        case 0:
            switch (aaa) {
                case 0:
                    switch (bbb) {
                        case 0:  n = "brk"; break;
                        case 2:  n = "php"; break;
                        case 4:  n = "bpl"; break;
                        case 6:  n = "clc"; break;
                        default: n = "*nop"; break;
                    }
                    break;
                case 1:
                    switch (bbb) {
                        case 0:  n = "jsr"; break;
                        case 2:  n = "plp"; break;
                        case 4:  n = "bmi"; break;
                        case 5:  n = "*nop"; break;
                        case 6:  n = "sec"; break;
                        case 7:  n = "*nop"; break;
                        default: n = "bit"; break;
                    }
                    break;
                case 2:
                    switch (bbb) {
                        case 0:  n = "rti"; break;
                        case 2:  n = "pha"; break;
                        case 3:  n = "jmp"; break;
                        case 4:  n = "bvc"; break;
                        case 6:  n = "cli"; break;
                        default: n = "*nop"; break;
                    }
                    break;
                case 3:
                    switch (bbb) {
                        case 0:  n = "rts"; break;
                        case 2:  n = "pla"; break;
                        case 3:  n = "jmp"; indirect = true; break;  // jmp ()
                        case 4:  n = "bvs"; break;
                        case 6:  n = "sei"; break;
                        default: n = "*nop"; break;
                    }
                    break;
                case 4:
                    switch (bbb) {
                        case 0:  n = "*nop"; break;
                        case 2:  n = "dey"; break;
                        case 4:  n = "bcc"; break;
                        case 6:  n = "tya"; break;
                        default: n = "sty"; break;
                    }
                    break;
                case 5:
                    switch (bbb) {
                        case 2:  n = "tay"; break;
                        case 4:  n = "bcs"; break;
                        case 6:  n = "clv"; break;
                        default: n = "ldy"; break;
                    }
                    break;
                case 6:
                    switch (bbb) {
                        case 2:  n = "iny"; break;
                        case 4:  n = "bne"; break;
                        case 5:  n = "*nop"; break;
                        case 6:  n = "cld"; break;
                        case 7:  n = "*nop"; break;
                        default: n = "cpy"; break;
                    }
                    break;
                case 7:
                    switch (bbb) {
                        case 2:  n = "inx"; break;
                        case 4:  n = "beq"; break;
                        case 5:  n = "*nop"; break;
                        case 6:  n = "sed"; break;
                        case 7:  n = "*nop"; break;
                        default: n = "cpx"; break;
                    }
                    break;
            }
            break;

        case 1:
            switch (aaa) {
                case 0: n = "ora"; break;
                case 1: n = "and"; break; // AND A
                case 2: n = "eor"; break;
                case 3: n = "adc"; break;
                case 4:
                    switch (bbb) {
                        case 2:  n = "*nop"; break;
                        default: n = "sta"; break;
                    }
                    break;
                case 5: n = "lda"; break;
                case 6: n = "cmp"; break;
                case 7: n = "sbc"; break;
            }
            break;

        case 2:
            switch (aaa) {
                case 0:
                    switch (bbb) {
                        case 6:  n = "*nop"; break;
                        default: n = "asl"; break;
                    }
                    break;
                case 1:
                    switch (bbb) {
                        case 6:  n = "*nop"; break;
                        default: n = "rol"; break;
                    }
                    break;
                case 2:
                    switch (bbb) {
                        case 6:  n = "*nop"; break;
                        default: n = "lsr"; break;
                    }
                    break;
                case 3:
                    switch (bbb) {
                        case 6:  n = "*nop"; break;
                        default: n = "ror"; break;
                    }
                    break;
                case 4:
                    switch (bbb) {
                        case 0:  n = "*nop"; break;
                        case 2:  n = "txa"; break;
                        case 6:  n = "txs"; break;
                        default: n = "stx"; break;
                    }
                    break;
                case 5:
                    switch (bbb) {
                        case 2:  n = "tax"; break;
                        case 6:  n = "tsx"; break;
                        default: n = "ldx"; break;
                    }
                    break;
                case 6:
                    switch (bbb) {
                        case 0:  n = "*nop"; break;
                        case 2:  n = "dex"; break;
                        case 6:  n = "*nop"; break;
                        default: n = "dec"; break;
                    }
                    break;
                case 7:
                    switch (bbb) {
                        case 0:  n = "*nop"; break;
                        case 2:  n = "nop"; break;
                        case 6:  n = "*nop"; break;
                        default: n = "inc"; break;
                    }
                    break;
            }
            break;

        case 3:
            switch (aaa) {
                case 0: n = "*slo"; break;
                case 1: n = "*rla"; break;
                case 2: n = "*sre"; break;
                case 3: n = "*rra"; break;
                case 4: n = "*sax"; break;
                case 5: n = "*lax"; break;
                case 6: n = "*dcp"; break;
                case 7:
                    switch (bbb) {
                        case 2:  n = "*sbc"; break;
                        default: n = "*isb"; break;
                    }
                    break;
            }
    }
    dst += sprintf(dst, "%s", n);

    uint8_t l,h;
    switch (ops[cc][bbb][aaa].addr) {
        case A_IMM:
            l = fetch(pc, pos++, userdata);
            dst += sprintf(dst, " #$%02X", l);
            break;
        case A_ZER:
            l = fetch(pc, pos++, userdata);
            dst += sprintf(dst, " $%02X", l);
            break;
        case A_ZPX:
            l = fetch(pc, pos++, userdata);
            dst += sprintf(dst, " $%02X,X", l);
            break;
        case A_ZPY:
            l = fetch(pc, pos++, userdata);
            dst += sprintf(dst, " $%02X,Y", l);
            break;
        case A_ABS:
        case A_JSR:
        case A_JMP:
            l = fetch(pc, pos++, userdata);
            h = fetch(pc, pos++, userdata);
            if (indirect) {
                dst += sprintf(dst, " ($%02X%02X)", h, l);
            }
            else {
                dst += sprintf(dst, " $%02X%02X", h, l);
            }
            break;
        case A_ABX:
            l = fetch(pc, pos++, userdata);
            h = fetch(pc, pos++, userdata);
            dst += sprintf(dst, " $%02X%02X,X", h, l);
            break;
        case A_ABY:
            l = fetch(pc, pos++, userdata);
            h = fetch(pc, pos++, userdata);
            dst += sprintf(dst, " $%02X%02X,Y", h, l);
            break;
        case A_IDX:
            l = fetch(pc, pos++, userdata);
            dst += sprintf(dst, " ($%02X,X)", l);
            break;
        case A_IDY:
            l = fetch(pc, pos++, userdata);
            dst += sprintf(dst, " ($%02X),Y", l);
            break;
    }
    *dst = 0;
    return pos;
}
} // namespace mos6502dasm



