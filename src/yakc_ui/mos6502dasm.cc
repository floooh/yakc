//------------------------------------------------------------------------------
//  mos6502dasm.cc
//------------------------------------------------------------------------------
#include "mos6502dasm.h"
#include "chips/m6502.h"
#include <stdio.h>

namespace mos6502dasm {

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
    switch (mos6502_ops[cc][bbb][aaa].addr) {
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



