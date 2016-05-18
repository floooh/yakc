#pragma once
//------------------------------------------------------------------------------
/**
    The 'algorithmic' Z80 instruction decoder parts.
    
    See: http://www.z80.info/decoding.htm
*/
#include "core/common.h"
#include "core/z80.h"

namespace yakc {

//------------------------------------------------------------------------------
inline bool
z80::cc(ubyte y) const {
    switch (y) {
        case 0: return !(F & ZF);  // NZ
        case 1: return F & ZF;     // Z
        case 2: return !(F & CF);  // NC
        case 3: return F & CF;     // C
        case 4: return !(F & PF);  // PO
        case 5: return F & PF;     // PE
        case 6: return !(F & SF);  // P
        default: return F & SF;    // M
    }
}

//------------------------------------------------------------------------------
inline uword
z80::iHLIXIYd(bool ext) {
    return HLIXIY + (ext ? mem.rs8(PC++) : 0);
}

//------------------------------------------------------------------------------
inline void
z80::alu8(ubyte alu, ubyte val) {
    switch (alu) {
        case 0: add8(val); break;
        case 1: adc8(val); break;
        case 2: sub8(val); break;
        case 3: sbc8(val); break;
        case 4: and8(val); break;
        case 5: xor8(val); break;
        case 6: or8(val); break;
        case 7: cp8(val); break;
    }
}

//------------------------------------------------------------------------------
inline uint32_t
z80::do_cb(ubyte op, bool ext, int off) {
    // FIXME: if executed with DD/FD prefix, some of the CB instructions
    // also copy the result to a register, these are undocumented so
    // we don't care for now!
    const ubyte x = op>>6;
    const ubyte y = (op>>3) & 7;
    const ubyte z = op & 7;
    uint32_t ext_cycles = ext ? 4 : 0;
    if (x == 0) {
        // roll and shift ops
        ubyte val;
        uword addr;
        if (z == 6) {
            addr = HLIXIY+off;
            val = mem.r8(addr);
        }
        else {
            val = R8[r[z]];
        }
        switch (y) {
            case 0: val = rlc8(val, true); break;   //--- RLC
            case 1: val = rrc8(val, true); break;   //--- RRC
            case 2: val = rl8(val, true); break;    //--- RL
            case 3: val = rr8(val, true); break;    //--- RR
            case 4: val = sla8(val); break;         //--- SLA
            case 5: val = sra8(val); break;         //--- SRA
            case 6: val = sll8(val); break;         //--- SLL
            case 7: val = srl8(val); break;         //--- SRL
        }
        if (z == 6) {
            mem.w8(addr, val);
            return 15 + ext_cycles;
        }
        else {
            R8[r[z]] = val;
            return 8;
        }
    }
    else if (x == 1) {
        // BIT y,r[z]
        if (z == 6) {
            bit(mem.r8(HLIXIY+off), 1<<y);
            return 12 + ext_cycles;
        }
        else {
            bit(R8[r[z]], 1<<y);
            return 8;
        }
    }
    else if (x == 2) {
        // RES y,r[z]
        if (z == 6) {
            const uword addr = HLIXIY+off;
            mem.w8(addr, mem.r8(addr) & ~(1<<y));
            return 15 + ext_cycles;
        }
        else {
            R8[r[z]] &= ~(1<<y);
            return 8;
        }
    }
    else if (x == 3) {
        // SET y,r[z]
        if (z == 6) {
            const uword addr = HLIXIY+off;
            mem.w8(addr, mem.r8(addr) | (1<<y));
            return 15 + ext_cycles;
        }
        else {
            R8[r[z]] |= (1<<y);
            return 8;
        }
    }
    YAKC_ASSERT(false);
    return 4;   // can't happen
}

//------------------------------------------------------------------------------
inline uint32_t
z80::do_ed(ubyte op) {
    const ubyte x = op>>6;
    const ubyte y = (op>>3) & 7;
    const ubyte z = op & 7;
    const ubyte p = y >> 1;
    const ubyte q = y & 1;
    if (x == 1) {
        switch (z) {
            //--- IN
            case 0:
                R8[r[y]] = in(BC);          // NOTE: IN F,(C) is undocumented
                F = szp[R8[r[y]]]|(F&CF);
                return 12;
            //-- OUT
            case 1:
                out(BC, R8[r[y]]);
                return 12;
            //--- SBC/ADC HL
            case 2:
                if (q == 0) {
                    HL = sbc16(HL,R16[rp[p]]);                        
                }
                else {
                    HL = adc16(HL,R16[rp[p]]);
                }
                return 15;
            //--- load store 16-bit with immediate address
            case 3:
                if (q == 0) {
                    mem.w16(mem.r16(PC), R16[rp[p]]);
                }
                else {
                    R16[rp[p]] = mem.r16(mem.r16(PC));
                }
                PC += 2;
                return 20;
            //--- NEG
            case 4:
                neg8();
                return 8;
            //--- RETN, RETI
            case 5:
                if (y == 1) {
                    reti();
                }
                else {
                    // RETN not implemented
                    YAKC_ASSERT(false);
                }
                return 15;
                    
            //--- IM
            case 6:
                if ((y & 3) > 1) {
                    IM = (y & 3) - 1;
                }
                else {
                    IM = 0;
                }
                return 8;

            //--- assorted ops
            case 7:
                switch (y) {
                    case 0: I=A; return 9;          // LD I,A
                    case 1: R=A; return 9;          // LD R,A
                    case 2: A=I; F=sziff2(I,IFF2)|(F&CF); return 9; // LD A,I
                    case 3: A=R; F=sziff2(R,IFF2)|(F&CF); return 9; // LD A,R
                    case 4: rrd(); return 18;       // RRD
                    case 5: rld(); return 18;       // RLD
                    case 6: return 4;               // NOP
                    case 7: return 4;               // NOP
                }
                break;
        }
    }
    else if (x == 2) {
        //--- block instructions
        switch (z) {
            //--- LDI,LDD,LDIR,LDDR
            case 0:
                switch (y) {
                    case 4: ldi(); return 16;
                    case 5: ldd(); return 16;
                    case 6: return ldir();
                    case 7: return lddr();
                }
                break;
            //--- CPI,CPD,CPID,CPDR
            case 1:
                switch (y) {
                    case 4: cpi(); return 16;
                    case 5: cpd(); return 16;
                    case 6: return cpir();
                    case 7: return cpdr();
                }
                break;
            //--- INI,IND,INID,INDR
            case 2:
                switch (y) {
                    case 4: ini(); return 16;
                    case 5: ind(); return 16;
                    case 6: return inir();
                    case 7: return indr();
                }
                break;
            //--- OUTI,OUTD,OTIR,OTDR
            case 3:
                switch (y) {
                    case 4: outi(); return 16;
                    case 5: outd(); return 16;
                    case 6: return otir();
                    case 7: return otdr();
                }
                break;
            default:
                return invalid_opcode(2);
        }
    }
    else {
        return invalid_opcode(2);
    }
    YAKC_ASSERT(false);
    return 4; // can't happen
}

//------------------------------------------------------------------------------
inline uint32_t
z80::do_op(ubyte op, bool ext) {
    const ubyte x = op>>6;
    const ubyte y = (op>>3) & 7;
    const ubyte z = op & 7;
    uint32_t ext_cycles = ext ? 8 : 0;

    // 8-bit load, or special case HALT for LD (HL),(HL)
    if (x == 1) {
        if (y == 6) {
            if (z == 6) {
                halt();
                return 4;
            }
            else {
                // LD (HL),r; LD (IX+d),r; LD (IY+d),r
                mem.w8(iHLIXIYd(ext), R8[r2[z]]);
                return 7 + ext_cycles;
            }
        }
        else if (z == 6) {
            // LD r,(HL); LD r,(IX+d); LD r,(IY+d)
            R8[r2[y]] = mem.r8(iHLIXIYd(ext));
            return 7 + ext_cycles;
        }
        else {
            // LD r,s
            R8[r[y]] = R8[r[z]];
            return 4;
        }
    }

    // 8-bit ALU instruction with register or (HL), (IX+d), (IY+d)
    else if (x == 2) {
        if (z == 6) {
            alu8(y, mem.r8(iHLIXIYd(ext)));
            return 7 + ext_cycles;
        }
        else {
            alu8(y, R8[r[z]]);
            return 4;
        }
    }

    // misc opcodes
    else if (x == 0) {
        const ubyte p = y >> 1;
        const ubyte q = y & 1;
        switch (z) {
            //--- relative jumps and assorted ops
            case 0:
                switch (y) {
                    //--- NOP
                    case 0:
                        return 4;
                    //--- EX AF,AF'
                    case 1:
                        swap16(AF, AF_);
                        return 4;
                    //--- DJNZ d
                    case 2:
                        if (--B > 0) {
                            PC += mem.rs8(PC) + 1;
                            return 13;
                        }
                        else {
                            PC++;
                            return 8;
                        }
                    //--- JR d
                    case 3:
                        PC += mem.rs8(PC) + 1;
                        return 12;
                    //--- JR cc,d
                    default:
                        if (cc(y-4)) {
                            PC += mem.rs8(PC) + 1;
                            return 12;
                        }
                        else {
                            PC++;
                            return 7;
                        }
                }
                break;

            //--- 16-bit immediate load, and ADD HL,rr
            case 1:
                if (q == 0) {
                    R16[rp[p]] = mem.r16(PC);
                    PC += 2;
                    return 10;
                }
                else {
                    // ADD HL,rr, ADD IX,rr, ADD IY,rr
                    HLIXIY = add16(HLIXIY, R16[rp[p]]);
                    return 11;
                }
                break;

            //--- indirect loads
            case 2:
                switch (y) {
                    case 0: mem.w8(BC,A); return 7;     //--- LD (BC),A
                    case 1: A=mem.r8(BC); return 7;     //--- LD A,(BC)
                    case 2: mem.w8(DE,A); return 7;     //--- LD (DE),A
                    case 3: A=mem.r8(DE); return 7;     //--- LD A,(DE)
                    case 4: mem.w16(mem.r16(PC),HLIXIY); PC+=2; return 16;  //--- LD (nn),HL; LD (nn),IX; LD (nn),IY
                    case 5: HLIXIY=mem.r16(mem.r16(PC)); PC+=2; return 16;  //--- LD HL,(nn), LD IX,(nn), LD IY,(nn)
                    case 6: mem.w8(mem.r16(PC),A); PC+=2; return 13;        //--- LD (nn),A
                    case 7: A=mem.r8(mem.r16(PC)); PC+=2; return 13;        //--- LD A,(nn)
                }
                break;

            //--- INC/DEC rp[p] (16-bit)
            case 3:
                if (q == 0) {
                    R16[rp[p]]++;
                }
                else {
                    R16[rp[p]]--;
                }
                return 6;

            //--- INC r[y] (8-bit)
            case 4:
                if (y == 6) {
                    // INC (HL), INC (IX+d), INC (IY+d)
                    const uword addr = iHLIXIYd(ext);
                    mem.w8(addr, inc8(mem.r8(addr)));
                    return 11 + ext_cycles;
                }
                else {
                    R8[r[y]] = inc8(R8[r[y]]);
                    return 4;
                }
                break;
                
            //--- DEC r[y] (8-bit)
            case 5:
                if (y == 6) {
                    // DEC (HL), INC (IX+d), INC (IY+d)
                    const uword addr = iHLIXIYd(ext);
                    mem.w8(addr, dec8(mem.r8(addr)));
                    return 11 + ext_cycles;
                }
                else {
                    R8[r[y]] = dec8(R8[r[y]]);
                    return 4;
                }
                break;

            //--- LD r[y],n
            case 6:
                if (y == 6) {
                    const uword addr = iHLIXIYd(ext);
                    mem.w8(addr, mem.r8(PC++));
                    return ext ? 15 : 10;
                }
                else {
                    R8[r[y]] = mem.r8(PC++);
                    return 7;
                }
                break;

            //--- assorted ops on accumulator and flags
            case 7:
                switch (y) {
                    case 0: A=rlc8(A,false); break;  //--- RLCA
                    case 1: A=rrc8(A,false); break;  //--- RRCA
                    case 2: A=rl8(A,false); break;   //--- RLA
                    case 3: A=rr8(A,false); break;   //--- RRA
                    case 4: daa(); break;               //--- DAA
                    case 5: A^=0xFF; F=(F&(SF|ZF|PF|CF))|HF|NF|(A&(YF|XF)); break;  //--- CPL
                    case 6: F=(F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF)); break;           //--- SCF
                    case 7: F=((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF; break; //--- CCF
                }
                return 4;
        }
        YAKC_ASSERT(false);
    }
    else if (x == 3) {
        const ubyte p = y >> 1;
        const ubyte q = y & 1;
        switch (z) {
            //--- RET cc
            case 0:
                if (cc(y)) {
                    PC = mem.r16(SP);
                    SP += 2;
                    return 11;
                }
                return 5;

            //--- POP and various ops
            case 1:
                if (q == 0) {
                    // POP rp2[p] (BC=0, DE=1, HL,IX,IY=2, AF=3)
                    R16[rp2[p]] = mem.r16(SP);
                    SP += 2;
                    return 10;
                }
                else {
                    switch (p) {
                        case 0:
                            //--- RET
                            PC = mem.r16(SP);
                            SP += 2;
                            return 10;
                        case 1:
                            //--- EXX (swap BC,DE,HL)
                            swap16(BC, BC_);
                            swap16(DE, DE_);
                            swap16(HL, HL_);
                            return 4;
                        case 2:
                            //--- JP HL; JP IX; JP IY
                            PC = HLIXIY;
                            return 4;
                        case 3:
                            //--- LD SP,HL; LD SP,IX; LD SP,IY
                            SP = HLIXIY;
                            return 6;
                    }
                }
                break;

            //--- JP cc
            case 2:
                if (cc(y)) {
                    PC = mem.r16(PC);
                }
                else {
                    PC += 2;
                }
                return 10;

            //--- assorted ops
            case 3:
                switch (y) {
                    case 0:
                        //--- JP nn
                        PC = mem.r16(PC);
                        return 10;
                    case 1:
                        //--- CB prefix
                        {
                            const int off = ext ? mem.rs8(PC++) : 0;
                            const ubyte op = fetch_op();
                            return do_cb(op, ext, off);
                        }
                    case 2:
                        //--- OUT (n),A
                        out((A<<8)|mem.r8(PC++), A);
                        return 11;
                    case 3:
                        //--- IN A,(n)
                        A = in((A<<8)|(mem.r8(PC++)));
                        return 11;
                    case 4:
                        //--- EX (SP),HL; EX (SP),IX; EX (SP),IY
                        {
                            uword tmp = mem.r16(SP);
                            mem.w16(SP, HLIXIY);
                            HLIXIY = tmp;
                        }
                        return 19;
                    case 5:
                        //--- EX DE,HL
                        swap16(DE, HL);
                        return 4;
                    case 6:
                        //--- DI
                        di();
                        return 4;
                    case 7:
                        //--- EI
                        ei();
                        return 4;
                }
                break;

            //--- CALL cc
            case 4:
                if (cc(y)) {
                    SP -= 2;
                    mem.w16(SP, PC+2);
                    PC = mem.r16(PC);
                    return 17;
                }
                else {
                    PC += 2;
                    return 10;
                }
                break;

            //--- PUSH and various ops
            case 5:
                if (q == 0) {
                    // PUSH rp2[p] (BC=0, DE=1, HL=2, AF=3), IX, IY
                    SP -= 2;
                    mem.w16(SP, R16[rp2[p]]);
                    return 11;
                }
                else {
                    switch (p) {
                        case 0:
                            //--- CALL nn
                            SP -= 2;
                            mem.w16(SP, PC+2);
                            PC = mem.r16(PC);
                            return 17;
                        case 1:
                            //--- DD prefix
                            if (!ext) {
                                // patch register lookup tables (replace HL with IX)
                                rp[2] = rp2[2] = 4;
                                r[4] = 9; r[5] = 8;
                                uint32_t cycles = do_op(fetch_op(), true) + 4;
                                rp[2] = rp2[2] = 2;
                                r[4] = 5; r[5] = 4;
                                return cycles;
                            }
                            else {
                                return invalid_opcode(2);
                            }
                        case 2:
                            //--- ED prefix
                            if (!ext) {
                                return do_ed(fetch_op());
                            }
                            else {
                                return invalid_opcode(2);
                            }
                        case 3:
                            // FD prefix
                            if (!ext) {
                                // patch register lookup tables (replace HL with IY)
                                rp[2] = rp2[2] = 5;
                                r[4] = 11; r[5] = 10;
                                uint32_t cycles = do_op(fetch_op(), true) + 4;
                                rp[2] = rp2[2] = 2;
                                r[4] = 5; r[5] = 4;
                                return cycles;
                            }
                            else {
                                return invalid_opcode(2);
                            }
                    }
                }
                break;

            //--- 8-bit ALU with immediate operand
            case 6:
                alu8(y, mem.r8(PC++));
                return 7;

            //--- RST
            case 7:
                rst(y*8);
                return 11;
        }
    }
    YAKC_ASSERT(false);
    return 4; // can't happen
}

} // namespace yakc
