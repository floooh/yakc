#pragma once
// #version:1#
// machine generated, do not edit!
#include "core/common.h"
namespace yakc {
inline unsigned int z80::step() {
    int d;
    uword u16tmp;
    state.INV = false;
    if (enable_interrupt) {
        state.IFF1 = state.IFF2 = true;
        enable_interrupt = false;
    }
    switch (fetch_op()) {
    case 0x0:
        // NOP
        return 4;
        break;
    case 0x1:
        // LD BC,nn
        state.BC = mem.r16(state.PC);
        state.PC += 2;
        return 10;
        break;
    case 0x2:
        // LD (BC),A
        mem.w8(state.BC, state.A);
        return 7;
        break;
    case 0x3:
        // INC BC
        state.BC++;
        return 6;
        break;
    case 0x4:
        // INC B
        state.B = inc8(state.B);
        return 4;
        break;
    case 0x5:
        // DEC B
        state.B = dec8(state.B);
        return 4;
        break;
    case 0x6:
        // LD B,n
        state.B = mem.r8(state.PC++);
        return 7;
        break;
    case 0x7:
        // RLCA
        state.A = rlc8(state.A, false);
        return 4;
        break;
    case 0x8:
        // EX AF,AF'
        swap16(state.AF, state.AF_);
        return 4;
        break;
    case 0x9:
        // ADD HL,BC
        state.HL = add16(state.HL, state.BC);
        return 11;
        break;
    case 0xa:
        // LD A,(BC)
        state.A = mem.r8(state.BC);
        return 7;
        break;
    case 0xb:
        // DEC BC
        state.BC--;
        return 6;
        break;
    case 0xc:
        // INC C
        state.C = inc8(state.C);
        return 4;
        break;
    case 0xd:
        // DEC C
        state.C = dec8(state.C);
        return 4;
        break;
    case 0xe:
        // LD C,n
        state.C = mem.r8(state.PC++);
        return 7;
        break;
    case 0xf:
        // RRCA
        state.A = rrc8(state.A, false);
        return 4;
        break;
    case 0x10:
        // DJNZ e
        if (--state.B > 0) {
            state.PC += mem.rs8(state.PC++);
            return 13;
        }
        else {
            state.PC++;
            return 8;
        }
        break;
    case 0x11:
        // LD DE,nn
        state.DE = mem.r16(state.PC);
        state.PC += 2;
        return 10;
        break;
    case 0x12:
        // LD (DE),A
        mem.w8(state.DE, state.A);
        return 7;
        break;
    case 0x13:
        // INC DE
        state.DE++;
        return 6;
        break;
    case 0x14:
        // INC D
        state.D = inc8(state.D);
        return 4;
        break;
    case 0x15:
        // DEC D
        state.D = dec8(state.D);
        return 4;
        break;
    case 0x16:
        // LD D,n
        state.D = mem.r8(state.PC++);
        return 7;
        break;
    case 0x17:
        // RLA
        state.A = rl8(state.A, false);
        return 4;
        break;
    case 0x18:
        // JR e
        state.PC += mem.rs8(state.PC++);
        return 12;
        break;
    case 0x19:
        // ADD HL,DE
        state.HL = add16(state.HL, state.DE);
        return 11;
        break;
    case 0x1a:
        // LD A,(DE)
        state.A = mem.r8(state.DE);
        return 7;
        break;
    case 0x1b:
        // DEC DE
        state.DE--;
        return 6;
        break;
    case 0x1c:
        // INC E
        state.E = inc8(state.E);
        return 4;
        break;
    case 0x1d:
        // DEC E
        state.E = dec8(state.E);
        return 4;
        break;
    case 0x1e:
        // LD E,n
        state.E = mem.r8(state.PC++);
        return 7;
        break;
    case 0x1f:
        // RRA
        state.A = rr8(state.A, false);
        return 4;
        break;
    case 0x20:
        // JR NZ,e
        if (!(state.F & ZF)) {
            state.PC += mem.rs8(state.PC++);
            return 12;
        }
        else {
            state.PC++;
            return 7;
        }
        break;
    case 0x21:
        // LD HL,nn
        state.HL = mem.r16(state.PC);
        state.PC += 2;
        return 10;
        break;
    case 0x22:
        // LD (nn),HL
        mem.w16(mem.r16(state.PC), state.HL);
        state.PC += 2;
        return 16;
        break;
    case 0x23:
        // INC HL
        state.HL++;
        return 6;
        break;
    case 0x24:
        // INC H
        state.H = inc8(state.H);
        return 4;
        break;
    case 0x25:
        // DEC H
        state.H = dec8(state.H);
        return 4;
        break;
    case 0x26:
        // LD H,n
        state.H = mem.r8(state.PC++);
        return 7;
        break;
    case 0x27:
        // DAA
        daa();
        return 4;
        break;
    case 0x28:
        // JR Z,e
        if (state.F & ZF) {
            state.PC += mem.rs8(state.PC++);
            return 12;
        }
        else {
            state.PC++;
            return 7;
        }
        break;
    case 0x29:
        // ADD HL,HL
        state.HL = add16(state.HL, state.HL);
        return 11;
        break;
    case 0x2a:
        // LD HL,(nn)
        state.HL = mem.r16(mem.r16(state.PC));
        state.PC += 2;
        return 16;
        break;
    case 0x2b:
        // DEC HL
        state.HL--;
        return 6;
        break;
    case 0x2c:
        // INC L
        state.L = inc8(state.L);
        return 4;
        break;
    case 0x2d:
        // DEC L
        state.L = dec8(state.L);
        return 4;
        break;
    case 0x2e:
        // LD L,n
        state.L = mem.r8(state.PC++);
        return 7;
        break;
    case 0x2f:
        // CPL
        state.A ^= 0xFF;
        state.F = (state.F&(SF|ZF|PF|CF)) | HF | NF | (state.A&(YF|XF));
        return 4;
        break;
    case 0x30:
        // JR NC,e
        if (!(state.F & CF)) {
            state.PC += mem.rs8(state.PC++);
            return 12;
        }
        else {
            state.PC++;
            return 7;
        }
        break;
    case 0x31:
        // LD SP,nn
        state.SP = mem.r16(state.PC);
        state.PC += 2;
        return 10;
        break;
    case 0x32:
        // LD (nn),A
        mem.w8(mem.r16(state.PC), state.A);
        state.PC += 2;
        return 13;
        break;
    case 0x33:
        // INC SP
        state.SP++;
        return 6;
        break;
    case 0x34:
        // INC (HL)
        mem.w8(state.HL, inc8(mem.r8(state.HL)));
        return 11;
        break;
    case 0x35:
        // DEC (HL)
        mem.w8(state.HL, dec8(mem.r8(state.HL)));
        return 11;
        break;
    case 0x36:
        // LD (HL),n
        mem.w8(state.HL, mem.r8(state.PC++));
        return 10;
        break;
    case 0x37:
        // SCF
        state.F = (state.F&(SF|ZF|YF|XF|PF))|CF|(state.A&(YF|XF));
        return 4;
        break;
    case 0x38:
        // JR C,e
        if (state.F & CF) {
            state.PC += mem.rs8(state.PC++);
            return 12;
        }
        else {
            state.PC++;
            return 7;
        }
        break;
    case 0x39:
        // ADD HL,SP
        state.HL = add16(state.HL, state.SP);
        return 11;
        break;
    case 0x3a:
        // LD A,(nn)
        state.A = mem.r8(mem.r16(state.PC));
        state.PC += 2;
        return 13;
        break;
    case 0x3b:
        // DEC SP
        state.SP--;
        return 6;
        break;
    case 0x3c:
        // INC A
        state.A = inc8(state.A);
        return 4;
        break;
    case 0x3d:
        // DEC A
        state.A = dec8(state.A);
        return 4;
        break;
    case 0x3e:
        // LD A,n
        state.A = mem.r8(state.PC++);
        return 7;
        break;
    case 0x3f:
        // CCF
        state.F = ((state.F&(SF|ZF|YF|XF|PF|CF))|((state.F&CF)<<4)|(state.A&(YF|XF)))^CF;
        return 4;
        break;
    case 0x40:
        // LD B,B
        state.B = state.B;
        return 4;
        break;
    case 0x41:
        // LD B,C
        state.B = state.C;
        return 4;
        break;
    case 0x42:
        // LD B,D
        state.B = state.D;
        return 4;
        break;
    case 0x43:
        // LD B,E
        state.B = state.E;
        return 4;
        break;
    case 0x44:
        // LD B,H
        state.B = state.H;
        return 4;
        break;
    case 0x45:
        // LD B,L
        state.B = state.L;
        return 4;
        break;
    case 0x46:
        // LD B,(HL)
        state.B = mem.r8(state.HL);
        return 7;
        break;
    case 0x47:
        // LD B,A
        state.B = state.A;
        return 4;
        break;
    case 0x48:
        // LD C,B
        state.C = state.B;
        return 4;
        break;
    case 0x49:
        // LD C,C
        state.C = state.C;
        return 4;
        break;
    case 0x4a:
        // LD C,D
        state.C = state.D;
        return 4;
        break;
    case 0x4b:
        // LD C,E
        state.C = state.E;
        return 4;
        break;
    case 0x4c:
        // LD C,H
        state.C = state.H;
        return 4;
        break;
    case 0x4d:
        // LD C,L
        state.C = state.L;
        return 4;
        break;
    case 0x4e:
        // LD C,(HL)
        state.C = mem.r8(state.HL);
        return 7;
        break;
    case 0x4f:
        // LD C,A
        state.C = state.A;
        return 4;
        break;
    case 0x50:
        // LD D,B
        state.D = state.B;
        return 4;
        break;
    case 0x51:
        // LD D,C
        state.D = state.C;
        return 4;
        break;
    case 0x52:
        // LD D,D
        state.D = state.D;
        return 4;
        break;
    case 0x53:
        // LD D,E
        state.D = state.E;
        return 4;
        break;
    case 0x54:
        // LD D,H
        state.D = state.H;
        return 4;
        break;
    case 0x55:
        // LD D,L
        state.D = state.L;
        return 4;
        break;
    case 0x56:
        // LD D,(HL)
        state.D = mem.r8(state.HL);
        return 7;
        break;
    case 0x57:
        // LD D,A
        state.D = state.A;
        return 4;
        break;
    case 0x58:
        // LD E,B
        state.E = state.B;
        return 4;
        break;
    case 0x59:
        // LD E,C
        state.E = state.C;
        return 4;
        break;
    case 0x5a:
        // LD E,D
        state.E = state.D;
        return 4;
        break;
    case 0x5b:
        // LD E,E
        state.E = state.E;
        return 4;
        break;
    case 0x5c:
        // LD E,H
        state.E = state.H;
        return 4;
        break;
    case 0x5d:
        // LD E,L
        state.E = state.L;
        return 4;
        break;
    case 0x5e:
        // LD E,(HL)
        state.E = mem.r8(state.HL);
        return 7;
        break;
    case 0x5f:
        // LD E,A
        state.E = state.A;
        return 4;
        break;
    case 0x60:
        // LD H,B
        state.H = state.B;
        return 4;
        break;
    case 0x61:
        // LD H,C
        state.H = state.C;
        return 4;
        break;
    case 0x62:
        // LD H,D
        state.H = state.D;
        return 4;
        break;
    case 0x63:
        // LD H,E
        state.H = state.E;
        return 4;
        break;
    case 0x64:
        // LD H,H
        state.H = state.H;
        return 4;
        break;
    case 0x65:
        // LD H,L
        state.H = state.L;
        return 4;
        break;
    case 0x66:
        // LD H,(HL)
        state.H = mem.r8(state.HL);
        return 7;
        break;
    case 0x67:
        // LD H,A
        state.H = state.A;
        return 4;
        break;
    case 0x68:
        // LD L,B
        state.L = state.B;
        return 4;
        break;
    case 0x69:
        // LD L,C
        state.L = state.C;
        return 4;
        break;
    case 0x6a:
        // LD L,D
        state.L = state.D;
        return 4;
        break;
    case 0x6b:
        // LD L,E
        state.L = state.E;
        return 4;
        break;
    case 0x6c:
        // LD L,H
        state.L = state.H;
        return 4;
        break;
    case 0x6d:
        // LD L,L
        state.L = state.L;
        return 4;
        break;
    case 0x6e:
        // LD L,(HL)
        state.L = mem.r8(state.HL);
        return 7;
        break;
    case 0x6f:
        // LD L,A
        state.L = state.A;
        return 4;
        break;
    case 0x70:
        // LD (HL),B
        mem.w8(state.HL, state.B);
        return 7;
        break;
    case 0x71:
        // LD (HL),C
        mem.w8(state.HL, state.C);
        return 7;
        break;
    case 0x72:
        // LD (HL),D
        mem.w8(state.HL, state.D);
        return 7;
        break;
    case 0x73:
        // LD (HL),E
        mem.w8(state.HL, state.E);
        return 7;
        break;
    case 0x74:
        // LD (HL),H
        mem.w8(state.HL, state.H);
        return 7;
        break;
    case 0x75:
        // LD (HL),L
        mem.w8(state.HL, state.L);
        return 7;
        break;
    case 0x76:
        // HALT
        halt();
        return 4;
        break;
    case 0x77:
        // LD (HL),A
        mem.w8(state.HL, state.A);
        return 7;
        break;
    case 0x78:
        // LD A,B
        state.A = state.B;
        return 4;
        break;
    case 0x79:
        // LD A,C
        state.A = state.C;
        return 4;
        break;
    case 0x7a:
        // LD A,D
        state.A = state.D;
        return 4;
        break;
    case 0x7b:
        // LD A,E
        state.A = state.E;
        return 4;
        break;
    case 0x7c:
        // LD A,H
        state.A = state.H;
        return 4;
        break;
    case 0x7d:
        // LD A,L
        state.A = state.L;
        return 4;
        break;
    case 0x7e:
        // LD A,(HL)
        state.A = mem.r8(state.HL);
        return 7;
        break;
    case 0x7f:
        // LD A,A
        state.A = state.A;
        return 4;
        break;
    case 0x80:
        // ADD B
        add8(state.B);
        return 4;
        break;
    case 0x81:
        // ADD C
        add8(state.C);
        return 4;
        break;
    case 0x82:
        // ADD D
        add8(state.D);
        return 4;
        break;
    case 0x83:
        // ADD E
        add8(state.E);
        return 4;
        break;
    case 0x84:
        // ADD H
        add8(state.H);
        return 4;
        break;
    case 0x85:
        // ADD L
        add8(state.L);
        return 4;
        break;
    case 0x86:
        // ADD (HL)
        add8(mem.r8(state.HL));
        return 7;
        break;
    case 0x87:
        // ADD A
        add8(state.A);
        return 4;
        break;
    case 0x88:
        // ADC B
        adc8(state.B);
        return 4;
        break;
    case 0x89:
        // ADC C
        adc8(state.C);
        return 4;
        break;
    case 0x8a:
        // ADC D
        adc8(state.D);
        return 4;
        break;
    case 0x8b:
        // ADC E
        adc8(state.E);
        return 4;
        break;
    case 0x8c:
        // ADC H
        adc8(state.H);
        return 4;
        break;
    case 0x8d:
        // ADC L
        adc8(state.L);
        return 4;
        break;
    case 0x8e:
        // ADC (HL)
        adc8(mem.r8(state.HL));
        return 7;
        break;
    case 0x8f:
        // ADC A
        adc8(state.A);
        return 4;
        break;
    case 0x90:
        // SUB B
        sub8(state.B);
        return 4;
        break;
    case 0x91:
        // SUB C
        sub8(state.C);
        return 4;
        break;
    case 0x92:
        // SUB D
        sub8(state.D);
        return 4;
        break;
    case 0x93:
        // SUB E
        sub8(state.E);
        return 4;
        break;
    case 0x94:
        // SUB H
        sub8(state.H);
        return 4;
        break;
    case 0x95:
        // SUB L
        sub8(state.L);
        return 4;
        break;
    case 0x96:
        // SUB (HL)
        sub8(mem.r8(state.HL));
        return 7;
        break;
    case 0x97:
        // SUB A
        sub8(state.A);
        return 4;
        break;
    case 0x98:
        // SBC B
        sbc8(state.B);
        return 4;
        break;
    case 0x99:
        // SBC C
        sbc8(state.C);
        return 4;
        break;
    case 0x9a:
        // SBC D
        sbc8(state.D);
        return 4;
        break;
    case 0x9b:
        // SBC E
        sbc8(state.E);
        return 4;
        break;
    case 0x9c:
        // SBC H
        sbc8(state.H);
        return 4;
        break;
    case 0x9d:
        // SBC L
        sbc8(state.L);
        return 4;
        break;
    case 0x9e:
        // SBC (HL)
        sbc8(mem.r8(state.HL));
        return 7;
        break;
    case 0x9f:
        // SBC A
        sbc8(state.A);
        return 4;
        break;
    case 0xa0:
        // AND B
        and8(state.B);
        return 4;
        break;
    case 0xa1:
        // AND C
        and8(state.C);
        return 4;
        break;
    case 0xa2:
        // AND D
        and8(state.D);
        return 4;
        break;
    case 0xa3:
        // AND E
        and8(state.E);
        return 4;
        break;
    case 0xa4:
        // AND H
        and8(state.H);
        return 4;
        break;
    case 0xa5:
        // AND L
        and8(state.L);
        return 4;
        break;
    case 0xa6:
        // AND (HL)
        and8(mem.r8(state.HL));
        return 7;
        break;
    case 0xa7:
        // AND A
        and8(state.A);
        return 4;
        break;
    case 0xa8:
        // XOR B
        xor8(state.B);
        return 4;
        break;
    case 0xa9:
        // XOR C
        xor8(state.C);
        return 4;
        break;
    case 0xaa:
        // XOR D
        xor8(state.D);
        return 4;
        break;
    case 0xab:
        // XOR E
        xor8(state.E);
        return 4;
        break;
    case 0xac:
        // XOR H
        xor8(state.H);
        return 4;
        break;
    case 0xad:
        // XOR L
        xor8(state.L);
        return 4;
        break;
    case 0xae:
        // XOR (HL)
        xor8(mem.r8(state.HL));
        return 7;
        break;
    case 0xaf:
        // XOR A
        xor8(state.A);
        return 4;
        break;
    case 0xb0:
        // OR B
        or8(state.B);
        return 4;
        break;
    case 0xb1:
        // OR C
        or8(state.C);
        return 4;
        break;
    case 0xb2:
        // OR D
        or8(state.D);
        return 4;
        break;
    case 0xb3:
        // OR E
        or8(state.E);
        return 4;
        break;
    case 0xb4:
        // OR H
        or8(state.H);
        return 4;
        break;
    case 0xb5:
        // OR L
        or8(state.L);
        return 4;
        break;
    case 0xb6:
        // OR (HL)
        or8(mem.r8(state.HL));
        return 7;
        break;
    case 0xb7:
        // OR A
        or8(state.A);
        return 4;
        break;
    case 0xb8:
        // CP B
        cp8(state.B);
        return 4;
        break;
    case 0xb9:
        // CP C
        cp8(state.C);
        return 4;
        break;
    case 0xba:
        // CP D
        cp8(state.D);
        return 4;
        break;
    case 0xbb:
        // CP E
        cp8(state.E);
        return 4;
        break;
    case 0xbc:
        // CP H
        cp8(state.H);
        return 4;
        break;
    case 0xbd:
        // CP L
        cp8(state.L);
        return 4;
        break;
    case 0xbe:
        // CP (HL)
        cp8(mem.r8(state.HL));
        return 7;
        break;
    case 0xbf:
        // CP A
        cp8(state.A);
        return 4;
        break;
    case 0xc0:
        // RET NZ
        if ((!(state.F & ZF))) {
            state.PC = mem.r16(state.SP);
            state.SP += 2;
            return 11;
        }
        else {
            return 5;
        }
        break;
    case 0xc1:
        // POP BC
        state.BC = mem.r16(state.SP);
        state.SP += 2;
        return 10;
        break;
    case 0xc2:
        // JP NZ,nn
        state.PC = (!(state.F & ZF)) ? mem.r16(state.PC) : state.PC+2;
        return 10;
        break;
    case 0xc3:
        // JP nn
        state.PC = mem.r16(state.PC);
        return 10;
        break;
    case 0xc4:
        // CALL NZ,nn
        if ((!(state.F & ZF))) {
            state.SP -= 2;
            mem.w16(state.SP, state.PC+2);
            state.PC = mem.r16(state.PC);
            return 17;
        }
        else {
            state.PC += 2;
            return 10;
        }
        break;
    case 0xc5:
        // PUSH BC
        state.SP -= 2;
        mem.w16(state.SP, state.BC);
        return 11;
        break;
    case 0xc6:
        // ADD n
        add8(mem.r8(state.PC++));
        return 7;
        break;
    case 0xc8:
        // RET Z
        if ((state.F & ZF)) {
            state.PC = mem.r16(state.SP);
            state.SP += 2;
            return 11;
        }
        else {
            return 5;
        }
        break;
    case 0xc9:
        // RET
        state.PC = mem.r16(state.SP);
        state.SP += 2;
        return 10;
        break;
    case 0xca:
        // JP Z,nn
        state.PC = (state.F & ZF) ? mem.r16(state.PC) : state.PC+2;
        return 10;
        break;
    case 0xcb:
        switch (fetch_op()) {
        case 0x0:
            // RLC B
            state.B = rlc8(state.B, true);
            return 8;
            break;
        case 0x1:
            // RLC C
            state.C = rlc8(state.C, true);
            return 8;
            break;
        case 0x2:
            // RLC D
            state.D = rlc8(state.D, true);
            return 8;
            break;
        case 0x3:
            // RLC E
            state.E = rlc8(state.E, true);
            return 8;
            break;
        case 0x4:
            // RLC H
            state.H = rlc8(state.H, true);
            return 8;
            break;
        case 0x5:
            // RLC L
            state.L = rlc8(state.L, true);
            return 8;
            break;
        case 0x6:
            // RLC (HL)
            mem.w8(state.HL, rlc8(mem.r8(state.HL), true));
            return 15;
            break;
        case 0x7:
            // RLC A
            state.A = rlc8(state.A, true);
            return 8;
            break;
        case 0x8:
            // RRC B
            state.B = rrc8(state.B, true);
            return 8;
            break;
        case 0x9:
            // RRC C
            state.C = rrc8(state.C, true);
            return 8;
            break;
        case 0xa:
            // RRC D
            state.D = rrc8(state.D, true);
            return 8;
            break;
        case 0xb:
            // RRC E
            state.E = rrc8(state.E, true);
            return 8;
            break;
        case 0xc:
            // RRC H
            state.H = rrc8(state.H, true);
            return 8;
            break;
        case 0xd:
            // RRC L
            state.L = rrc8(state.L, true);
            return 8;
            break;
        case 0xe:
            // RRC (HL)
            mem.w8(state.HL, rrc8(mem.r8(state.HL), true));
            return 15;
            break;
        case 0xf:
            // RRC A
            state.A = rrc8(state.A, true);
            return 8;
            break;
        case 0x10:
            // RL B
            state.B = rl8(state.B, true);
            return 8;
            break;
        case 0x11:
            // RL C
            state.C = rl8(state.C, true);
            return 8;
            break;
        case 0x12:
            // RL D
            state.D = rl8(state.D, true);
            return 8;
            break;
        case 0x13:
            // RL E
            state.E = rl8(state.E, true);
            return 8;
            break;
        case 0x14:
            // RL H
            state.H = rl8(state.H, true);
            return 8;
            break;
        case 0x15:
            // RL L
            state.L = rl8(state.L, true);
            return 8;
            break;
        case 0x16:
            // RL (HL)
            mem.w8(state.HL, rl8(mem.r8(state.HL), true));
            return 15;
            break;
        case 0x17:
            // RL A
            state.A = rl8(state.A, true);
            return 8;
            break;
        case 0x18:
            // RR B
            state.B = rr8(state.B, true);
            return 8;
            break;
        case 0x19:
            // RR C
            state.C = rr8(state.C, true);
            return 8;
            break;
        case 0x1a:
            // RR D
            state.D = rr8(state.D, true);
            return 8;
            break;
        case 0x1b:
            // RR E
            state.E = rr8(state.E, true);
            return 8;
            break;
        case 0x1c:
            // RR H
            state.H = rr8(state.H, true);
            return 8;
            break;
        case 0x1d:
            // RR L
            state.L = rr8(state.L, true);
            return 8;
            break;
        case 0x1e:
            // RR (HL)
            mem.w8(state.HL, rr8(mem.r8(state.HL), true));
            return 15;
            break;
        case 0x1f:
            // RR A
            state.A = rr8(state.A, true);
            return 8;
            break;
        case 0x20:
            // SLA B
            state.B = sla8(state.B);
            return 8;
            break;
        case 0x21:
            // SLA C
            state.C = sla8(state.C);
            return 8;
            break;
        case 0x22:
            // SLA D
            state.D = sla8(state.D);
            return 8;
            break;
        case 0x23:
            // SLA E
            state.E = sla8(state.E);
            return 8;
            break;
        case 0x24:
            // SLA H
            state.H = sla8(state.H);
            return 8;
            break;
        case 0x25:
            // SLA L
            state.L = sla8(state.L);
            return 8;
            break;
        case 0x26:
            // SLA (HL)
            mem.w8(state.HL, sla8(mem.r8(state.HL)));
            return 15;
            break;
        case 0x27:
            // SLA A
            state.A = sla8(state.A);
            return 8;
            break;
        case 0x28:
            // SRA B
            state.B = sra8(state.B);
            return 8;
            break;
        case 0x29:
            // SRA C
            state.C = sra8(state.C);
            return 8;
            break;
        case 0x2a:
            // SRA D
            state.D = sra8(state.D);
            return 8;
            break;
        case 0x2b:
            // SRA E
            state.E = sra8(state.E);
            return 8;
            break;
        case 0x2c:
            // SRA H
            state.H = sra8(state.H);
            return 8;
            break;
        case 0x2d:
            // SRA L
            state.L = sra8(state.L);
            return 8;
            break;
        case 0x2e:
            // SRA (HL)
            mem.w8(state.HL, sra8(mem.r8(state.HL)));
            return 15;
            break;
        case 0x2f:
            // SRA A
            state.A = sra8(state.A);
            return 8;
            break;
        case 0x30:
            // SLL B
            state.B = sll8(state.B);
            return 8;
            break;
        case 0x31:
            // SLL C
            state.C = sll8(state.C);
            return 8;
            break;
        case 0x32:
            // SLL D
            state.D = sll8(state.D);
            return 8;
            break;
        case 0x33:
            // SLL E
            state.E = sll8(state.E);
            return 8;
            break;
        case 0x34:
            // SLL H
            state.H = sll8(state.H);
            return 8;
            break;
        case 0x35:
            // SLL L
            state.L = sll8(state.L);
            return 8;
            break;
        case 0x36:
            // SLL (HL)
            mem.w8(state.HL, sll8(mem.r8(state.HL)));
            return 15;
            break;
        case 0x37:
            // SLL A
            state.A = sll8(state.A);
            return 8;
            break;
        case 0x38:
            // SRL B
            state.B = srl8(state.B);
            return 8;
            break;
        case 0x39:
            // SRL C
            state.C = srl8(state.C);
            return 8;
            break;
        case 0x3a:
            // SRL D
            state.D = srl8(state.D);
            return 8;
            break;
        case 0x3b:
            // SRL E
            state.E = srl8(state.E);
            return 8;
            break;
        case 0x3c:
            // SRL H
            state.H = srl8(state.H);
            return 8;
            break;
        case 0x3d:
            // SRL L
            state.L = srl8(state.L);
            return 8;
            break;
        case 0x3e:
            // SRL (HL)
            mem.w8(state.HL, srl8(mem.r8(state.HL)));
            return 15;
            break;
        case 0x3f:
            // SRL A
            state.A = srl8(state.A);
            return 8;
            break;
        case 0x40:
            // BIT 0,B
            bit(state.B, (1<<0));
            return 8;
            break;
        case 0x41:
            // BIT 0,C
            bit(state.C, (1<<0));
            return 8;
            break;
        case 0x42:
            // BIT 0,D
            bit(state.D, (1<<0));
            return 8;
            break;
        case 0x43:
            // BIT 0,E
            bit(state.E, (1<<0));
            return 8;
            break;
        case 0x44:
            // BIT 0,H
            bit(state.H, (1<<0));
            return 8;
            break;
        case 0x45:
            // BIT 0,L
            bit(state.L, (1<<0));
            return 8;
            break;
        case 0x46:
            // BIT 0,(HL)
            bit(mem.r8(state.HL), (1<<0));
            return 12;
            break;
        case 0x47:
            // BIT 0,A
            bit(state.A, (1<<0));
            return 8;
            break;
        case 0x48:
            // BIT 1,B
            bit(state.B, (1<<1));
            return 8;
            break;
        case 0x49:
            // BIT 1,C
            bit(state.C, (1<<1));
            return 8;
            break;
        case 0x4a:
            // BIT 1,D
            bit(state.D, (1<<1));
            return 8;
            break;
        case 0x4b:
            // BIT 1,E
            bit(state.E, (1<<1));
            return 8;
            break;
        case 0x4c:
            // BIT 1,H
            bit(state.H, (1<<1));
            return 8;
            break;
        case 0x4d:
            // BIT 1,L
            bit(state.L, (1<<1));
            return 8;
            break;
        case 0x4e:
            // BIT 1,(HL)
            bit(mem.r8(state.HL), (1<<1));
            return 12;
            break;
        case 0x4f:
            // BIT 1,A
            bit(state.A, (1<<1));
            return 8;
            break;
        case 0x50:
            // BIT 2,B
            bit(state.B, (1<<2));
            return 8;
            break;
        case 0x51:
            // BIT 2,C
            bit(state.C, (1<<2));
            return 8;
            break;
        case 0x52:
            // BIT 2,D
            bit(state.D, (1<<2));
            return 8;
            break;
        case 0x53:
            // BIT 2,E
            bit(state.E, (1<<2));
            return 8;
            break;
        case 0x54:
            // BIT 2,H
            bit(state.H, (1<<2));
            return 8;
            break;
        case 0x55:
            // BIT 2,L
            bit(state.L, (1<<2));
            return 8;
            break;
        case 0x56:
            // BIT 2,(HL)
            bit(mem.r8(state.HL), (1<<2));
            return 12;
            break;
        case 0x57:
            // BIT 2,A
            bit(state.A, (1<<2));
            return 8;
            break;
        case 0x58:
            // BIT 3,B
            bit(state.B, (1<<3));
            return 8;
            break;
        case 0x59:
            // BIT 3,C
            bit(state.C, (1<<3));
            return 8;
            break;
        case 0x5a:
            // BIT 3,D
            bit(state.D, (1<<3));
            return 8;
            break;
        case 0x5b:
            // BIT 3,E
            bit(state.E, (1<<3));
            return 8;
            break;
        case 0x5c:
            // BIT 3,H
            bit(state.H, (1<<3));
            return 8;
            break;
        case 0x5d:
            // BIT 3,L
            bit(state.L, (1<<3));
            return 8;
            break;
        case 0x5e:
            // BIT 3,(HL)
            bit(mem.r8(state.HL), (1<<3));
            return 12;
            break;
        case 0x5f:
            // BIT 3,A
            bit(state.A, (1<<3));
            return 8;
            break;
        case 0x60:
            // BIT 4,B
            bit(state.B, (1<<4));
            return 8;
            break;
        case 0x61:
            // BIT 4,C
            bit(state.C, (1<<4));
            return 8;
            break;
        case 0x62:
            // BIT 4,D
            bit(state.D, (1<<4));
            return 8;
            break;
        case 0x63:
            // BIT 4,E
            bit(state.E, (1<<4));
            return 8;
            break;
        case 0x64:
            // BIT 4,H
            bit(state.H, (1<<4));
            return 8;
            break;
        case 0x65:
            // BIT 4,L
            bit(state.L, (1<<4));
            return 8;
            break;
        case 0x66:
            // BIT 4,(HL)
            bit(mem.r8(state.HL), (1<<4));
            return 12;
            break;
        case 0x67:
            // BIT 4,A
            bit(state.A, (1<<4));
            return 8;
            break;
        case 0x68:
            // BIT 5,B
            bit(state.B, (1<<5));
            return 8;
            break;
        case 0x69:
            // BIT 5,C
            bit(state.C, (1<<5));
            return 8;
            break;
        case 0x6a:
            // BIT 5,D
            bit(state.D, (1<<5));
            return 8;
            break;
        case 0x6b:
            // BIT 5,E
            bit(state.E, (1<<5));
            return 8;
            break;
        case 0x6c:
            // BIT 5,H
            bit(state.H, (1<<5));
            return 8;
            break;
        case 0x6d:
            // BIT 5,L
            bit(state.L, (1<<5));
            return 8;
            break;
        case 0x6e:
            // BIT 5,(HL)
            bit(mem.r8(state.HL), (1<<5));
            return 12;
            break;
        case 0x6f:
            // BIT 5,A
            bit(state.A, (1<<5));
            return 8;
            break;
        case 0x70:
            // BIT 6,B
            bit(state.B, (1<<6));
            return 8;
            break;
        case 0x71:
            // BIT 6,C
            bit(state.C, (1<<6));
            return 8;
            break;
        case 0x72:
            // BIT 6,D
            bit(state.D, (1<<6));
            return 8;
            break;
        case 0x73:
            // BIT 6,E
            bit(state.E, (1<<6));
            return 8;
            break;
        case 0x74:
            // BIT 6,H
            bit(state.H, (1<<6));
            return 8;
            break;
        case 0x75:
            // BIT 6,L
            bit(state.L, (1<<6));
            return 8;
            break;
        case 0x76:
            // BIT 6,(HL)
            bit(mem.r8(state.HL), (1<<6));
            return 12;
            break;
        case 0x77:
            // BIT 6,A
            bit(state.A, (1<<6));
            return 8;
            break;
        case 0x78:
            // BIT 7,B
            bit(state.B, (1<<7));
            return 8;
            break;
        case 0x79:
            // BIT 7,C
            bit(state.C, (1<<7));
            return 8;
            break;
        case 0x7a:
            // BIT 7,D
            bit(state.D, (1<<7));
            return 8;
            break;
        case 0x7b:
            // BIT 7,E
            bit(state.E, (1<<7));
            return 8;
            break;
        case 0x7c:
            // BIT 7,H
            bit(state.H, (1<<7));
            return 8;
            break;
        case 0x7d:
            // BIT 7,L
            bit(state.L, (1<<7));
            return 8;
            break;
        case 0x7e:
            // BIT 7,(HL)
            bit(mem.r8(state.HL), (1<<7));
            return 12;
            break;
        case 0x7f:
            // BIT 7,A
            bit(state.A, (1<<7));
            return 8;
            break;
        case 0x80:
            // RES 0,B
            state.B &= ~(1<<0);
            return 8;
            break;
        case 0x81:
            // RES 0,C
            state.C &= ~(1<<0);
            return 8;
            break;
        case 0x82:
            // RES 0,D
            state.D &= ~(1<<0);
            return 8;
            break;
        case 0x83:
            // RES 0,E
            state.E &= ~(1<<0);
            return 8;
            break;
        case 0x84:
            // RES 0,H
            state.H &= ~(1<<0);
            return 8;
            break;
        case 0x85:
            // RES 0,L
            state.L &= ~(1<<0);
            return 8;
            break;
        case 0x86:
            // RES 0,(HL)
            mem.w8(state.HL, mem.r8(state.HL)&~(1<<0));
            return 15;
            break;
        case 0x87:
            // RES 0,A
            state.A &= ~(1<<0);
            return 8;
            break;
        case 0x88:
            // RES 1,B
            state.B &= ~(1<<1);
            return 8;
            break;
        case 0x89:
            // RES 1,C
            state.C &= ~(1<<1);
            return 8;
            break;
        case 0x8a:
            // RES 1,D
            state.D &= ~(1<<1);
            return 8;
            break;
        case 0x8b:
            // RES 1,E
            state.E &= ~(1<<1);
            return 8;
            break;
        case 0x8c:
            // RES 1,H
            state.H &= ~(1<<1);
            return 8;
            break;
        case 0x8d:
            // RES 1,L
            state.L &= ~(1<<1);
            return 8;
            break;
        case 0x8e:
            // RES 1,(HL)
            mem.w8(state.HL, mem.r8(state.HL)&~(1<<1));
            return 15;
            break;
        case 0x8f:
            // RES 1,A
            state.A &= ~(1<<1);
            return 8;
            break;
        case 0x90:
            // RES 2,B
            state.B &= ~(1<<2);
            return 8;
            break;
        case 0x91:
            // RES 2,C
            state.C &= ~(1<<2);
            return 8;
            break;
        case 0x92:
            // RES 2,D
            state.D &= ~(1<<2);
            return 8;
            break;
        case 0x93:
            // RES 2,E
            state.E &= ~(1<<2);
            return 8;
            break;
        case 0x94:
            // RES 2,H
            state.H &= ~(1<<2);
            return 8;
            break;
        case 0x95:
            // RES 2,L
            state.L &= ~(1<<2);
            return 8;
            break;
        case 0x96:
            // RES 2,(HL)
            mem.w8(state.HL, mem.r8(state.HL)&~(1<<2));
            return 15;
            break;
        case 0x97:
            // RES 2,A
            state.A &= ~(1<<2);
            return 8;
            break;
        case 0x98:
            // RES 3,B
            state.B &= ~(1<<3);
            return 8;
            break;
        case 0x99:
            // RES 3,C
            state.C &= ~(1<<3);
            return 8;
            break;
        case 0x9a:
            // RES 3,D
            state.D &= ~(1<<3);
            return 8;
            break;
        case 0x9b:
            // RES 3,E
            state.E &= ~(1<<3);
            return 8;
            break;
        case 0x9c:
            // RES 3,H
            state.H &= ~(1<<3);
            return 8;
            break;
        case 0x9d:
            // RES 3,L
            state.L &= ~(1<<3);
            return 8;
            break;
        case 0x9e:
            // RES 3,(HL)
            mem.w8(state.HL, mem.r8(state.HL)&~(1<<3));
            return 15;
            break;
        case 0x9f:
            // RES 3,A
            state.A &= ~(1<<3);
            return 8;
            break;
        case 0xa0:
            // RES 4,B
            state.B &= ~(1<<4);
            return 8;
            break;
        case 0xa1:
            // RES 4,C
            state.C &= ~(1<<4);
            return 8;
            break;
        case 0xa2:
            // RES 4,D
            state.D &= ~(1<<4);
            return 8;
            break;
        case 0xa3:
            // RES 4,E
            state.E &= ~(1<<4);
            return 8;
            break;
        case 0xa4:
            // RES 4,H
            state.H &= ~(1<<4);
            return 8;
            break;
        case 0xa5:
            // RES 4,L
            state.L &= ~(1<<4);
            return 8;
            break;
        case 0xa6:
            // RES 4,(HL)
            mem.w8(state.HL, mem.r8(state.HL)&~(1<<4));
            return 15;
            break;
        case 0xa7:
            // RES 4,A
            state.A &= ~(1<<4);
            return 8;
            break;
        case 0xa8:
            // RES 5,B
            state.B &= ~(1<<5);
            return 8;
            break;
        case 0xa9:
            // RES 5,C
            state.C &= ~(1<<5);
            return 8;
            break;
        case 0xaa:
            // RES 5,D
            state.D &= ~(1<<5);
            return 8;
            break;
        case 0xab:
            // RES 5,E
            state.E &= ~(1<<5);
            return 8;
            break;
        case 0xac:
            // RES 5,H
            state.H &= ~(1<<5);
            return 8;
            break;
        case 0xad:
            // RES 5,L
            state.L &= ~(1<<5);
            return 8;
            break;
        case 0xae:
            // RES 5,(HL)
            mem.w8(state.HL, mem.r8(state.HL)&~(1<<5));
            return 15;
            break;
        case 0xaf:
            // RES 5,A
            state.A &= ~(1<<5);
            return 8;
            break;
        case 0xb0:
            // RES 6,B
            state.B &= ~(1<<6);
            return 8;
            break;
        case 0xb1:
            // RES 6,C
            state.C &= ~(1<<6);
            return 8;
            break;
        case 0xb2:
            // RES 6,D
            state.D &= ~(1<<6);
            return 8;
            break;
        case 0xb3:
            // RES 6,E
            state.E &= ~(1<<6);
            return 8;
            break;
        case 0xb4:
            // RES 6,H
            state.H &= ~(1<<6);
            return 8;
            break;
        case 0xb5:
            // RES 6,L
            state.L &= ~(1<<6);
            return 8;
            break;
        case 0xb6:
            // RES 6,(HL)
            mem.w8(state.HL, mem.r8(state.HL)&~(1<<6));
            return 15;
            break;
        case 0xb7:
            // RES 6,A
            state.A &= ~(1<<6);
            return 8;
            break;
        case 0xb8:
            // RES 7,B
            state.B &= ~(1<<7);
            return 8;
            break;
        case 0xb9:
            // RES 7,C
            state.C &= ~(1<<7);
            return 8;
            break;
        case 0xba:
            // RES 7,D
            state.D &= ~(1<<7);
            return 8;
            break;
        case 0xbb:
            // RES 7,E
            state.E &= ~(1<<7);
            return 8;
            break;
        case 0xbc:
            // RES 7,H
            state.H &= ~(1<<7);
            return 8;
            break;
        case 0xbd:
            // RES 7,L
            state.L &= ~(1<<7);
            return 8;
            break;
        case 0xbe:
            // RES 7,(HL)
            mem.w8(state.HL, mem.r8(state.HL)&~(1<<7));
            return 15;
            break;
        case 0xbf:
            // RES 7,A
            state.A &= ~(1<<7);
            return 8;
            break;
        case 0xc0:
            // SET 0,B
            state.B |= (1<<0);
            return 8;
            break;
        case 0xc1:
            // SET 0,C
            state.C |= (1<<0);
            return 8;
            break;
        case 0xc2:
            // SET 0,D
            state.D |= (1<<0);
            return 8;
            break;
        case 0xc3:
            // SET 0,E
            state.E |= (1<<0);
            return 8;
            break;
        case 0xc4:
            // SET 0,H
            state.H |= (1<<0);
            return 8;
            break;
        case 0xc5:
            // SET 0,L
            state.L |= (1<<0);
            return 8;
            break;
        case 0xc6:
            // SET 0,(HL)
            mem.w8(state.HL, mem.r8(state.HL)|(1<<0));
            return 15;
            break;
        case 0xc7:
            // SET 0,A
            state.A |= (1<<0);
            return 8;
            break;
        case 0xc8:
            // SET 1,B
            state.B |= (1<<1);
            return 8;
            break;
        case 0xc9:
            // SET 1,C
            state.C |= (1<<1);
            return 8;
            break;
        case 0xca:
            // SET 1,D
            state.D |= (1<<1);
            return 8;
            break;
        case 0xcb:
            // SET 1,E
            state.E |= (1<<1);
            return 8;
            break;
        case 0xcc:
            // SET 1,H
            state.H |= (1<<1);
            return 8;
            break;
        case 0xcd:
            // SET 1,L
            state.L |= (1<<1);
            return 8;
            break;
        case 0xce:
            // SET 1,(HL)
            mem.w8(state.HL, mem.r8(state.HL)|(1<<1));
            return 15;
            break;
        case 0xcf:
            // SET 1,A
            state.A |= (1<<1);
            return 8;
            break;
        case 0xd0:
            // SET 2,B
            state.B |= (1<<2);
            return 8;
            break;
        case 0xd1:
            // SET 2,C
            state.C |= (1<<2);
            return 8;
            break;
        case 0xd2:
            // SET 2,D
            state.D |= (1<<2);
            return 8;
            break;
        case 0xd3:
            // SET 2,E
            state.E |= (1<<2);
            return 8;
            break;
        case 0xd4:
            // SET 2,H
            state.H |= (1<<2);
            return 8;
            break;
        case 0xd5:
            // SET 2,L
            state.L |= (1<<2);
            return 8;
            break;
        case 0xd6:
            // SET 2,(HL)
            mem.w8(state.HL, mem.r8(state.HL)|(1<<2));
            return 15;
            break;
        case 0xd7:
            // SET 2,A
            state.A |= (1<<2);
            return 8;
            break;
        case 0xd8:
            // SET 3,B
            state.B |= (1<<3);
            return 8;
            break;
        case 0xd9:
            // SET 3,C
            state.C |= (1<<3);
            return 8;
            break;
        case 0xda:
            // SET 3,D
            state.D |= (1<<3);
            return 8;
            break;
        case 0xdb:
            // SET 3,E
            state.E |= (1<<3);
            return 8;
            break;
        case 0xdc:
            // SET 3,H
            state.H |= (1<<3);
            return 8;
            break;
        case 0xdd:
            // SET 3,L
            state.L |= (1<<3);
            return 8;
            break;
        case 0xde:
            // SET 3,(HL)
            mem.w8(state.HL, mem.r8(state.HL)|(1<<3));
            return 15;
            break;
        case 0xdf:
            // SET 3,A
            state.A |= (1<<3);
            return 8;
            break;
        case 0xe0:
            // SET 4,B
            state.B |= (1<<4);
            return 8;
            break;
        case 0xe1:
            // SET 4,C
            state.C |= (1<<4);
            return 8;
            break;
        case 0xe2:
            // SET 4,D
            state.D |= (1<<4);
            return 8;
            break;
        case 0xe3:
            // SET 4,E
            state.E |= (1<<4);
            return 8;
            break;
        case 0xe4:
            // SET 4,H
            state.H |= (1<<4);
            return 8;
            break;
        case 0xe5:
            // SET 4,L
            state.L |= (1<<4);
            return 8;
            break;
        case 0xe6:
            // SET 4,(HL)
            mem.w8(state.HL, mem.r8(state.HL)|(1<<4));
            return 15;
            break;
        case 0xe7:
            // SET 4,A
            state.A |= (1<<4);
            return 8;
            break;
        case 0xe8:
            // SET 5,B
            state.B |= (1<<5);
            return 8;
            break;
        case 0xe9:
            // SET 5,C
            state.C |= (1<<5);
            return 8;
            break;
        case 0xea:
            // SET 5,D
            state.D |= (1<<5);
            return 8;
            break;
        case 0xeb:
            // SET 5,E
            state.E |= (1<<5);
            return 8;
            break;
        case 0xec:
            // SET 5,H
            state.H |= (1<<5);
            return 8;
            break;
        case 0xed:
            // SET 5,L
            state.L |= (1<<5);
            return 8;
            break;
        case 0xee:
            // SET 5,(HL)
            mem.w8(state.HL, mem.r8(state.HL)|(1<<5));
            return 15;
            break;
        case 0xef:
            // SET 5,A
            state.A |= (1<<5);
            return 8;
            break;
        case 0xf0:
            // SET 6,B
            state.B |= (1<<6);
            return 8;
            break;
        case 0xf1:
            // SET 6,C
            state.C |= (1<<6);
            return 8;
            break;
        case 0xf2:
            // SET 6,D
            state.D |= (1<<6);
            return 8;
            break;
        case 0xf3:
            // SET 6,E
            state.E |= (1<<6);
            return 8;
            break;
        case 0xf4:
            // SET 6,H
            state.H |= (1<<6);
            return 8;
            break;
        case 0xf5:
            // SET 6,L
            state.L |= (1<<6);
            return 8;
            break;
        case 0xf6:
            // SET 6,(HL)
            mem.w8(state.HL, mem.r8(state.HL)|(1<<6));
            return 15;
            break;
        case 0xf7:
            // SET 6,A
            state.A |= (1<<6);
            return 8;
            break;
        case 0xf8:
            // SET 7,B
            state.B |= (1<<7);
            return 8;
            break;
        case 0xf9:
            // SET 7,C
            state.C |= (1<<7);
            return 8;
            break;
        case 0xfa:
            // SET 7,D
            state.D |= (1<<7);
            return 8;
            break;
        case 0xfb:
            // SET 7,E
            state.E |= (1<<7);
            return 8;
            break;
        case 0xfc:
            // SET 7,H
            state.H |= (1<<7);
            return 8;
            break;
        case 0xfd:
            // SET 7,L
            state.L |= (1<<7);
            return 8;
            break;
        case 0xfe:
            // SET 7,(HL)
            mem.w8(state.HL, mem.r8(state.HL)|(1<<7));
            return 15;
            break;
        case 0xff:
            // SET 7,A
            state.A |= (1<<7);
            return 8;
            break;
        default:
            return invalid_opcode(2);
            break;
        }
        break;
    case 0xcc:
        // CALL Z,nn
        if ((state.F & ZF)) {
            state.SP -= 2;
            mem.w16(state.SP, state.PC+2);
            state.PC = mem.r16(state.PC);
            return 17;
        }
        else {
            state.PC += 2;
            return 10;
        }
        break;
    case 0xcd:
        // CALL nn
        state.SP -= 2;
        mem.w16(state.SP, state.PC+2);
        state.PC = mem.r16(state.PC);
        return 17;
        break;
    case 0xce:
        // ADC n
        adc8(mem.r8(state.PC++));
        return 7;
        break;
    case 0xd0:
        // RET NC
        if ((!(state.F & CF))) {
            state.PC = mem.r16(state.SP);
            state.SP += 2;
            return 11;
        }
        else {
            return 5;
        }
        break;
    case 0xd1:
        // POP DE
        state.DE = mem.r16(state.SP);
        state.SP += 2;
        return 10;
        break;
    case 0xd2:
        // JP NC,nn
        state.PC = (!(state.F & CF)) ? mem.r16(state.PC) : state.PC+2;
        return 10;
        break;
    case 0xd3:
        // OUT (n),A
        out((state.A<<8)|mem.r8(state.PC++),state.A);
        return 11;
        break;
    case 0xd4:
        // CALL NC,nn
        if ((!(state.F & CF))) {
            state.SP -= 2;
            mem.w16(state.SP, state.PC+2);
            state.PC = mem.r16(state.PC);
            return 17;
        }
        else {
            state.PC += 2;
            return 10;
        }
        break;
    case 0xd5:
        // PUSH DE
        state.SP -= 2;
        mem.w16(state.SP, state.DE);
        return 11;
        break;
    case 0xd6:
        // SUB n
        sub8(mem.r8(state.PC++));
        return 7;
        break;
    case 0xd8:
        // RET C
        if ((state.F & CF)) {
            state.PC = mem.r16(state.SP);
            state.SP += 2;
            return 11;
        }
        else {
            return 5;
        }
        break;
    case 0xd9:
        // EXX
        swap16(state.BC, state.BC_);
        swap16(state.DE, state.DE_);
        swap16(state.HL, state.HL_);
        return 4;
        break;
    case 0xda:
        // JP C,nn
        state.PC = (state.F & CF) ? mem.r16(state.PC) : state.PC+2;
        return 10;
        break;
    case 0xdb:
        // IN A,(n)
        state.A = in((state.A<<8)|(mem.r8(state.PC++)));
        return 11;
        break;
    case 0xdc:
        // CALL C,nn
        if ((state.F & CF)) {
            state.SP -= 2;
            mem.w16(state.SP, state.PC+2);
            state.PC = mem.r16(state.PC);
            return 17;
        }
        else {
            state.PC += 2;
            return 10;
        }
        break;
    case 0xdd:
        switch (fetch_op()) {
        case 0x9:
            // ADD IX,BC
            state.IX = add16(state.IX, state.BC);
            return 15;
            break;
        case 0x19:
            // ADD IX,DE
            state.IX = add16(state.IX, state.DE);
            return 15;
            break;
        case 0x21:
            // LD IX,nn
            state.IX = mem.r16(state.PC);
            state.PC += 2;
            return 14;
            break;
        case 0x22:
            // LD (nn),IX
            mem.w16(mem.r16(state.PC), state.IX);
            state.PC += 2;
            return 20;
            break;
        case 0x23:
            // INC IX
            state.IX++;
            return 10;
            break;
        case 0x24:
            // INC IXH
            state.IXH = inc8(state.IXH);
            return 8;
            break;
        case 0x25:
            // DEC IXH
            state.IXH = dec8(state.IXH);
            return 8;
            break;
        case 0x26:
            // LD IXH,n
            state.IXH = mem.r8(state.PC++);
            return 11;
            break;
        case 0x29:
            // ADD IX,IX
            state.IX = add16(state.IX, state.IX);
            return 15;
            break;
        case 0x2a:
            // LD IX,(nn)
            state.IX = mem.r16(mem.r16(state.PC));
            state.PC += 2;
            return 20;
            break;
        case 0x2b:
            // DEC IX
            state.IX--;
            return 10;
            break;
        case 0x2c:
            // INC IXL
            state.IXL = inc8(state.IXL);
            return 8;
            break;
        case 0x2d:
            // DEC IXL
            state.IXL = dec8(state.IXL);
            return 8;
            break;
        case 0x2e:
            // LD IXL,n
            state.IXL = mem.r8(state.PC++);
            return 11;
            break;
        case 0x34:
            // INC (IX+d)
            d = mem.rs8(state.PC++);
            mem.w8(state.IX+d, inc8(mem.r8(state.IX+d)));
            return 23;
            break;
        case 0x35:
            // DEC (IX+d)
            d = mem.rs8(state.PC++);
            mem.w8(state.IX+d, dec8(mem.r8(state.IX+d)));
            return 23;
            break;
        case 0x36:
            // LD (IX+d),n
            d = mem.rs8(state.PC++);
            mem.w8(state.IX + d, mem.r8(state.PC++));
            return 19;
            break;
        case 0x39:
            // ADD IX,SP
            state.IX = add16(state.IX, state.SP);
            return 15;
            break;
        case 0x40:
            // LD B,B
            state.B = state.B;
            return 8;
            break;
        case 0x41:
            // LD B,C
            state.B = state.C;
            return 8;
            break;
        case 0x42:
            // LD B,D
            state.B = state.D;
            return 8;
            break;
        case 0x43:
            // LD B,E
            state.B = state.E;
            return 8;
            break;
        case 0x44:
            // LD B,IXH
            state.B = state.IXH;
            return 8;
            break;
        case 0x45:
            // LD B,IXL
            state.B = state.IXL;
            return 8;
            break;
        case 0x46:
            // LD B,(IX+d)
            d = mem.rs8(state.PC++);
            state.B = mem.r8(state.IX + d);
            return 19;
            break;
        case 0x47:
            // LD B,A
            state.B = state.A;
            return 8;
            break;
        case 0x48:
            // LD C,B
            state.C = state.B;
            return 8;
            break;
        case 0x49:
            // LD C,C
            state.C = state.C;
            return 8;
            break;
        case 0x4a:
            // LD C,D
            state.C = state.D;
            return 8;
            break;
        case 0x4b:
            // LD C,E
            state.C = state.E;
            return 8;
            break;
        case 0x4c:
            // LD C,IXH
            state.C = state.IXH;
            return 8;
            break;
        case 0x4d:
            // LD C,IXL
            state.C = state.IXL;
            return 8;
            break;
        case 0x4e:
            // LD C,(IX+d)
            d = mem.rs8(state.PC++);
            state.C = mem.r8(state.IX + d);
            return 19;
            break;
        case 0x4f:
            // LD C,A
            state.C = state.A;
            return 8;
            break;
        case 0x50:
            // LD D,B
            state.D = state.B;
            return 8;
            break;
        case 0x51:
            // LD D,C
            state.D = state.C;
            return 8;
            break;
        case 0x52:
            // LD D,D
            state.D = state.D;
            return 8;
            break;
        case 0x53:
            // LD D,E
            state.D = state.E;
            return 8;
            break;
        case 0x54:
            // LD D,IXH
            state.D = state.IXH;
            return 8;
            break;
        case 0x55:
            // LD D,IXL
            state.D = state.IXL;
            return 8;
            break;
        case 0x56:
            // LD D,(IX+d)
            d = mem.rs8(state.PC++);
            state.D = mem.r8(state.IX + d);
            return 19;
            break;
        case 0x57:
            // LD D,A
            state.D = state.A;
            return 8;
            break;
        case 0x58:
            // LD E,B
            state.E = state.B;
            return 8;
            break;
        case 0x59:
            // LD E,C
            state.E = state.C;
            return 8;
            break;
        case 0x5a:
            // LD E,D
            state.E = state.D;
            return 8;
            break;
        case 0x5b:
            // LD E,E
            state.E = state.E;
            return 8;
            break;
        case 0x5c:
            // LD E,IXH
            state.E = state.IXH;
            return 8;
            break;
        case 0x5d:
            // LD E,IXL
            state.E = state.IXL;
            return 8;
            break;
        case 0x5e:
            // LD E,(IX+d)
            d = mem.rs8(state.PC++);
            state.E = mem.r8(state.IX + d);
            return 19;
            break;
        case 0x5f:
            // LD E,A
            state.E = state.A;
            return 8;
            break;
        case 0x60:
            // LD IXH,B
            state.IXH = state.B;
            return 8;
            break;
        case 0x61:
            // LD IXH,C
            state.IXH = state.C;
            return 8;
            break;
        case 0x62:
            // LD IXH,D
            state.IXH = state.D;
            return 8;
            break;
        case 0x63:
            // LD IXH,E
            state.IXH = state.E;
            return 8;
            break;
        case 0x64:
            // LD IXH,IXH
            state.IXH = state.IXH;
            return 8;
            break;
        case 0x65:
            // LD IXH,IXL
            state.IXH = state.IXL;
            return 8;
            break;
        case 0x66:
            // LD H,(IX+d)
            d = mem.rs8(state.PC++);
            state.H = mem.r8(state.IX + d);
            return 19;
            break;
        case 0x67:
            // LD IXH,A
            state.IXH = state.A;
            return 8;
            break;
        case 0x68:
            // LD IXL,B
            state.IXL = state.B;
            return 8;
            break;
        case 0x69:
            // LD IXL,C
            state.IXL = state.C;
            return 8;
            break;
        case 0x6a:
            // LD IXL,D
            state.IXL = state.D;
            return 8;
            break;
        case 0x6b:
            // LD IXL,E
            state.IXL = state.E;
            return 8;
            break;
        case 0x6c:
            // LD IXL,IXH
            state.IXL = state.IXH;
            return 8;
            break;
        case 0x6d:
            // LD IXL,IXL
            state.IXL = state.IXL;
            return 8;
            break;
        case 0x6e:
            // LD L,(IX+d)
            d = mem.rs8(state.PC++);
            state.L = mem.r8(state.IX + d);
            return 19;
            break;
        case 0x6f:
            // LD IXL,A
            state.IXL = state.A;
            return 8;
            break;
        case 0x70:
            // LD (IX+d),B
            d = mem.rs8(state.PC++);
            mem.w8(state.IX + d, state.B);
            return 19;
            break;
        case 0x71:
            // LD (IX+d),C
            d = mem.rs8(state.PC++);
            mem.w8(state.IX + d, state.C);
            return 19;
            break;
        case 0x72:
            // LD (IX+d),D
            d = mem.rs8(state.PC++);
            mem.w8(state.IX + d, state.D);
            return 19;
            break;
        case 0x73:
            // LD (IX+d),E
            d = mem.rs8(state.PC++);
            mem.w8(state.IX + d, state.E);
            return 19;
            break;
        case 0x74:
            // LD (IX+d),H
            d = mem.rs8(state.PC++);
            mem.w8(state.IX + d, state.H);
            return 19;
            break;
        case 0x75:
            // LD (IX+d),L
            d = mem.rs8(state.PC++);
            mem.w8(state.IX + d, state.L);
            return 19;
            break;
        case 0x77:
            // LD (IX+d),A
            d = mem.rs8(state.PC++);
            mem.w8(state.IX + d, state.A);
            return 19;
            break;
        case 0x78:
            // LD A,B
            state.A = state.B;
            return 8;
            break;
        case 0x79:
            // LD A,C
            state.A = state.C;
            return 8;
            break;
        case 0x7a:
            // LD A,D
            state.A = state.D;
            return 8;
            break;
        case 0x7b:
            // LD A,E
            state.A = state.E;
            return 8;
            break;
        case 0x7c:
            // LD A,IXH
            state.A = state.IXH;
            return 8;
            break;
        case 0x7d:
            // LD A,IXL
            state.A = state.IXL;
            return 8;
            break;
        case 0x7e:
            // LD A,(IX+d)
            d = mem.rs8(state.PC++);
            state.A = mem.r8(state.IX + d);
            return 19;
            break;
        case 0x7f:
            // LD A,A
            state.A = state.A;
            return 8;
            break;
        case 0x84:
            // ADD IXH
            add8(state.IXH);
            return 8;
            break;
        case 0x85:
            // ADD IXL
            add8(state.IXL);
            return 8;
            break;
        case 0x86:
            // ADD(IX+d)
            d = mem.rs8(state.PC++);
            add8(mem.r8(state.IX + d));
            return 19;
            break;
        case 0x8c:
            // ADC IXH
            adc8(state.IXH);
            return 8;
            break;
        case 0x8d:
            // ADC IXL
            adc8(state.IXL);
            return 8;
            break;
        case 0x8e:
            // ADC(IX+d)
            d = mem.rs8(state.PC++);
            adc8(mem.r8(state.IX + d));
            return 19;
            break;
        case 0x94:
            // SUB IXH
            sub8(state.IXH);
            return 8;
            break;
        case 0x95:
            // SUB IXL
            sub8(state.IXL);
            return 8;
            break;
        case 0x96:
            // SUB(IX+d)
            d = mem.rs8(state.PC++);
            sub8(mem.r8(state.IX + d));
            return 19;
            break;
        case 0x9c:
            // SBC IXH
            sbc8(state.IXH);
            return 8;
            break;
        case 0x9d:
            // SBC IXL
            sbc8(state.IXL);
            return 8;
            break;
        case 0x9e:
            // SBC(IX+d)
            d = mem.rs8(state.PC++);
            sbc8(mem.r8(state.IX + d));
            return 19;
            break;
        case 0xa4:
            // AND IXH
            and8(state.IXH);
            return 8;
            break;
        case 0xa5:
            // AND IXL
            and8(state.IXL);
            return 8;
            break;
        case 0xa6:
            // AND(IX+d)
            d = mem.rs8(state.PC++);
            and8(mem.r8(state.IX + d));
            return 19;
            break;
        case 0xac:
            // XOR IXH
            xor8(state.IXH);
            return 8;
            break;
        case 0xad:
            // XOR IXL
            xor8(state.IXL);
            return 8;
            break;
        case 0xae:
            // XOR(IX+d)
            d = mem.rs8(state.PC++);
            xor8(mem.r8(state.IX + d));
            return 19;
            break;
        case 0xb4:
            // OR IXH
            or8(state.IXH);
            return 8;
            break;
        case 0xb5:
            // OR IXL
            or8(state.IXL);
            return 8;
            break;
        case 0xb6:
            // OR(IX+d)
            d = mem.rs8(state.PC++);
            or8(mem.r8(state.IX + d));
            return 19;
            break;
        case 0xbc:
            // CP IXH
            cp8(state.IXH);
            return 8;
            break;
        case 0xbd:
            // CP IXL
            cp8(state.IXL);
            return 8;
            break;
        case 0xbe:
            // CP(IX+d)
            d = mem.rs8(state.PC++);
            cp8(mem.r8(state.IX + d));
            return 19;
            break;
        case 0xcb:
            // RLC ([IX|IY]+d)
            // RRC ([IX|IY]+d)
            // RL ([IX|IY]+d)
            // RR ([IX|IY]+d)
            // SLA ([IX|IY]+d)
            // SRA ([IX|IY]+d)
            // SRL ([IX|IY]+d)
            // BIT b,([IX|IY]+d
            // SET b,([IX|IY]+d
            // RES b,([IX|IY]+d
            return dd_fd_cb(0xdd);
            break;
        case 0xe1:
            // POP IX
            state.IX = mem.r16(state.SP);
            state.SP += 2;
            return 14;
            break;
        case 0xe3:
            // EX (SP),IX
            u16tmp = mem.r16(state.SP);
            mem.w16(state.SP, state.IX);
            state.IX = u16tmp;
            return 23;
            break;
        case 0xe5:
            // PUSH IX
            state.SP -= 2;
            mem.w16(state.SP, state.IX);
            return 15;
            break;
        case 0xe9:
            // JP (IX)
            state.PC = state.IX;
            return 8;
            break;
        case 0xf9:
            // LD SP,IX
            state.SP = state.IX;
            return 10;
            break;
        default:
            return invalid_opcode(2);
            break;
        }
        break;
    case 0xde:
        // SBC n
        sbc8(mem.r8(state.PC++));
        return 7;
        break;
    case 0xe0:
        // RET PO
        if ((!(state.F & PF))) {
            state.PC = mem.r16(state.SP);
            state.SP += 2;
            return 11;
        }
        else {
            return 5;
        }
        break;
    case 0xe1:
        // POP HL
        state.HL = mem.r16(state.SP);
        state.SP += 2;
        return 10;
        break;
    case 0xe2:
        // JP PO,nn
        state.PC = (!(state.F & PF)) ? mem.r16(state.PC) : state.PC+2;
        return 10;
        break;
    case 0xe3:
        // EX (SP),HL
        u16tmp = mem.r16(state.SP);
        mem.w16(state.SP, state.HL);
        state.HL = u16tmp;
        return 19;
        break;
    case 0xe4:
        // CALL PO,nn
        if ((!(state.F & PF))) {
            state.SP -= 2;
            mem.w16(state.SP, state.PC+2);
            state.PC = mem.r16(state.PC);
            return 17;
        }
        else {
            state.PC += 2;
            return 10;
        }
        break;
    case 0xe5:
        // PUSH HL
        state.SP -= 2;
        mem.w16(state.SP, state.HL);
        return 11;
        break;
    case 0xe6:
        // AND n
        and8(mem.r8(state.PC++));
        return 7;
        break;
    case 0xe8:
        // RET PE
        if ((state.F & PF)) {
            state.PC = mem.r16(state.SP);
            state.SP += 2;
            return 11;
        }
        else {
            return 5;
        }
        break;
    case 0xe9:
        // JP (HL)
        state.PC = state.HL;
        return 4;
        break;
    case 0xea:
        // JP PE,nn
        state.PC = (state.F & PF) ? mem.r16(state.PC) : state.PC+2;
        return 10;
        break;
    case 0xeb:
        // EX DE,HL
        swap16(state.DE, state.HL);
        return 4;
        break;
    case 0xec:
        // CALL PE,nn
        if ((state.F & PF)) {
            state.SP -= 2;
            mem.w16(state.SP, state.PC+2);
            state.PC = mem.r16(state.PC);
            return 17;
        }
        else {
            state.PC += 2;
            return 10;
        }
        break;
    case 0xed:
        switch (fetch_op()) {
        case 0x40:
            // IN B,(C)
            state.B = in(state.BC);
            state.F = szp[state.B]|(state.F&CF);
            return 12;
            break;
        case 0x41:
            // OUT (C),B
            out(state.BC, state.B);
            return 12;
            break;
        case 0x42:
            // SBC HL,BC
            state.HL = sbc16(state.HL, state.BC);
            return 15;
            break;
        case 0x43:
            // LD (nn),BC
            mem.w16(mem.r16(state.PC), state.BC);
            state.PC += 2;
            return 20;
            break;
        case 0x44:
            // NEG
            neg8();
            return 8;
            break;
        case 0x46:
            // IM 0
            state.IM = 0;
            return 8;
            break;
        case 0x47:
            // LD I,A
            state.I = state.A;
            return 9;
            break;
        case 0x48:
            // IN C,(C)
            state.C = in(state.BC);
            state.F = szp[state.C]|(state.F&CF);
            return 12;
            break;
        case 0x49:
            // OUT (C),C
            out(state.BC, state.C);
            return 12;
            break;
        case 0x4a:
            // ADC HL,BC
            state.HL = adc16(state.HL, state.BC);
            return 15;
            break;
        case 0x4b:
            // LD BC,(nn)
            state.BC = mem.r16(mem.r16(state.PC));
            state.PC += 2;
            return 20;
            break;
        case 0x4d:
            // RETI
            reti();
            return 15;
            break;
        case 0x4f:
            // LD R,A
            state.R = state.A;
            return 9;
            break;
        case 0x50:
            // IN D,(C)
            state.D = in(state.BC);
            state.F = szp[state.D]|(state.F&CF);
            return 12;
            break;
        case 0x51:
            // OUT (C),D
            out(state.BC, state.D);
            return 12;
            break;
        case 0x52:
            // SBC HL,DE
            state.HL = sbc16(state.HL, state.DE);
            return 15;
            break;
        case 0x53:
            // LD (nn),DE
            mem.w16(mem.r16(state.PC), state.DE);
            state.PC += 2;
            return 20;
            break;
        case 0x56:
            // IM 1
            state.IM = 1;
            return 8;
            break;
        case 0x57:
            // LD A,I
            state.A = state.I;
            state.F = sziff2(state.I,state.IFF2)|(state.F&CF);
            return 9;
            break;
        case 0x58:
            // IN E,(C)
            state.E = in(state.BC);
            state.F = szp[state.E]|(state.F&CF);
            return 12;
            break;
        case 0x59:
            // OUT (C),E
            out(state.BC, state.E);
            return 12;
            break;
        case 0x5a:
            // ADC HL,DE
            state.HL = adc16(state.HL, state.DE);
            return 15;
            break;
        case 0x5b:
            // LD DE,(nn)
            state.DE = mem.r16(mem.r16(state.PC));
            state.PC += 2;
            return 20;
            break;
        case 0x5e:
            // IM 2
            state.IM = 2;
            return 8;
            break;
        case 0x5f:
            // LD A,R
            state.A = state.R;
            state.F = sziff2(state.R,state.IFF2)|(state.F&CF);
            return 9;
            break;
        case 0x60:
            // IN H,(C)
            state.H = in(state.BC);
            state.F = szp[state.H]|(state.F&CF);
            return 12;
            break;
        case 0x61:
            // OUT (C),H
            out(state.BC, state.H);
            return 12;
            break;
        case 0x62:
            // SBC HL,HL
            state.HL = sbc16(state.HL, state.HL);
            return 15;
            break;
        case 0x63:
            // LD (nn),HL
            mem.w16(mem.r16(state.PC), state.HL);
            state.PC += 2;
            return 20;
            break;
        case 0x67:
            // RRD
            rrd();
            return 18;
            break;
        case 0x68:
            // IN L,(C)
            state.L = in(state.BC);
            state.F = szp[state.L]|(state.F&CF);
            return 12;
            break;
        case 0x69:
            // OUT (C),L
            out(state.BC, state.L);
            return 12;
            break;
        case 0x6a:
            // ADC HL,HL
            state.HL = adc16(state.HL, state.HL);
            return 15;
            break;
        case 0x6b:
            // LD HL,(nn)
            state.HL = mem.r16(mem.r16(state.PC));
            state.PC += 2;
            return 20;
            break;
        case 0x6f:
            // RLD
            rld();
            return 18;
            break;
        case 0x72:
            // SBC HL,SP
            state.HL = sbc16(state.HL, state.SP);
            return 15;
            break;
        case 0x73:
            // LD (nn),SP
            mem.w16(mem.r16(state.PC), state.SP);
            state.PC += 2;
            return 20;
            break;
        case 0x78:
            // IN A,(C)
            state.A = in(state.BC);
            state.F = szp[state.A]|(state.F&CF);
            return 12;
            break;
        case 0x79:
            // OUT (C),A
            out(state.BC, state.A);
            return 12;
            break;
        case 0x7a:
            // ADC HL,SP
            state.HL = adc16(state.HL, state.SP);
            return 15;
            break;
        case 0x7b:
            // LD SP,(nn)
            state.SP = mem.r16(mem.r16(state.PC));
            state.PC += 2;
            return 20;
            break;
        case 0xa0:
            // LDI
            ldi();
            return 16;
            break;
        case 0xa1:
            // CPI
            cpi();
            return 16;
            break;
        case 0xa2:
            // INI
            ini();
            return 16;
            break;
        case 0xa3:
            // OUTI
            outi();
            return 16;
            break;
        case 0xa8:
            // LDD
            ldd();
            return 16;
            break;
        case 0xa9:
            // CPD
            cpd();
            return 16;
            break;
        case 0xaa:
            // IND
            ind();
            return 16;
            break;
        case 0xab:
            // OUTD
            outd();
            return 16;
            break;
        case 0xb0:
            // LDIR
            return ldir();
            break;
        case 0xb1:
            // CPIR
            return cpir();
            break;
        case 0xb2:
            // INIR
            return inir();
            break;
        case 0xb3:
            // OTIR
            return otir();
            break;
        case 0xb8:
            // LDDR
            return lddr();
            break;
        case 0xb9:
            // CPDR
            return cpdr();
            break;
        case 0xba:
            // INDR
            return indr();
            break;
        case 0xbb:
            // OTDR
            return otdr();
            break;
        default:
            return invalid_opcode(2);
            break;
        }
        break;
    case 0xee:
        // XOR n
        xor8(mem.r8(state.PC++));
        return 7;
        break;
    case 0xf0:
        // RET P
        if ((!(state.F & SF))) {
            state.PC = mem.r16(state.SP);
            state.SP += 2;
            return 11;
        }
        else {
            return 5;
        }
        break;
    case 0xf1:
        // POP AF
        state.AF = mem.r16(state.SP);
        state.SP += 2;
        return 10;
        break;
    case 0xf2:
        // JP P,nn
        state.PC = (!(state.F & SF)) ? mem.r16(state.PC) : state.PC+2;
        return 10;
        break;
    case 0xf3:
        // DI
        di();
        return 4;
        break;
    case 0xf4:
        // CALL P,nn
        if ((!(state.F & SF))) {
            state.SP -= 2;
            mem.w16(state.SP, state.PC+2);
            state.PC = mem.r16(state.PC);
            return 17;
        }
        else {
            state.PC += 2;
            return 10;
        }
        break;
    case 0xf5:
        // PUSH AF
        state.SP -= 2;
        mem.w16(state.SP, state.AF);
        return 11;
        break;
    case 0xf6:
        // OR n
        or8(mem.r8(state.PC++));
        return 7;
        break;
    case 0xf8:
        // RET M
        if ((state.F & SF)) {
            state.PC = mem.r16(state.SP);
            state.SP += 2;
            return 11;
        }
        else {
            return 5;
        }
        break;
    case 0xf9:
        // LD SP,HL
        state.SP = state.HL;
        return 6;
        break;
    case 0xfa:
        // JP M,nn
        state.PC = (state.F & SF) ? mem.r16(state.PC) : state.PC+2;
        return 10;
        break;
    case 0xfb:
        // EI
        ei();
        return 4;
        break;
    case 0xfc:
        // CALL M,nn
        if ((state.F & SF)) {
            state.SP -= 2;
            mem.w16(state.SP, state.PC+2);
            state.PC = mem.r16(state.PC);
            return 17;
        }
        else {
            state.PC += 2;
            return 10;
        }
        break;
    case 0xfd:
        switch (fetch_op()) {
        case 0x9:
            // ADD IY,BC
            state.IY = add16(state.IY, state.BC);
            return 15;
            break;
        case 0x19:
            // ADD IY,DE
            state.IY = add16(state.IY, state.DE);
            return 15;
            break;
        case 0x21:
            // LD IY,nn
            state.IY = mem.r16(state.PC);
            state.PC += 2;
            return 14;
            break;
        case 0x22:
            // LD (nn),IY
            mem.w16(mem.r16(state.PC), state.IY);
            state.PC += 2;
            return 20;
            break;
        case 0x23:
            // INC IY
            state.IY++;
            return 10;
            break;
        case 0x24:
            // INC IYH
            state.IYH = inc8(state.IYH);
            return 8;
            break;
        case 0x25:
            // DEC IYH
            state.IYH = dec8(state.IYH);
            return 8;
            break;
        case 0x26:
            // LD IYH,n
            state.IYH = mem.r8(state.PC++);
            return 11;
            break;
        case 0x29:
            // ADD IY,IY
            state.IY = add16(state.IY, state.IY);
            return 15;
            break;
        case 0x2a:
            // LD IY,(nn)
            state.IY = mem.r16(mem.r16(state.PC));
            state.PC += 2;
            return 20;
            break;
        case 0x2b:
            // DEC IY
            state.IY--;
            return 10;
            break;
        case 0x2c:
            // INC IYL
            state.IYL = inc8(state.IYL);
            return 8;
            break;
        case 0x2d:
            // DEC IYL
            state.IYL = dec8(state.IYL);
            return 8;
            break;
        case 0x2e:
            // LD IYL,n
            state.IYL = mem.r8(state.PC++);
            return 11;
            break;
        case 0x34:
            // INC (IY+d)
            d = mem.rs8(state.PC++);
            mem.w8(state.IY+d, inc8(mem.r8(state.IY+d)));
            return 23;
            break;
        case 0x35:
            // DEC (IY+d)
            d = mem.rs8(state.PC++);
            mem.w8(state.IY+d, dec8(mem.r8(state.IY+d)));
            return 23;
            break;
        case 0x36:
            // LD (IY+d),n
            d = mem.rs8(state.PC++);
            mem.w8(state.IY + d, mem.r8(state.PC++));
            return 19;
            break;
        case 0x39:
            // ADD IY,SP
            state.IY = add16(state.IY, state.SP);
            return 15;
            break;
        case 0x40:
            // LD B,B
            state.B = state.B;
            return 8;
            break;
        case 0x41:
            // LD B,C
            state.B = state.C;
            return 8;
            break;
        case 0x42:
            // LD B,D
            state.B = state.D;
            return 8;
            break;
        case 0x43:
            // LD B,E
            state.B = state.E;
            return 8;
            break;
        case 0x44:
            // LD B,IYH
            state.B = state.IYH;
            return 8;
            break;
        case 0x45:
            // LD B,IYL
            state.B = state.IYL;
            return 8;
            break;
        case 0x46:
            // LD B,(IY+d)
            d = mem.rs8(state.PC++);
            state.B = mem.r8(state.IY + d);
            return 19;
            break;
        case 0x47:
            // LD B,A
            state.B = state.A;
            return 8;
            break;
        case 0x48:
            // LD C,B
            state.C = state.B;
            return 8;
            break;
        case 0x49:
            // LD C,C
            state.C = state.C;
            return 8;
            break;
        case 0x4a:
            // LD C,D
            state.C = state.D;
            return 8;
            break;
        case 0x4b:
            // LD C,E
            state.C = state.E;
            return 8;
            break;
        case 0x4c:
            // LD C,IYH
            state.C = state.IYH;
            return 8;
            break;
        case 0x4d:
            // LD C,IYL
            state.C = state.IYL;
            return 8;
            break;
        case 0x4e:
            // LD C,(IY+d)
            d = mem.rs8(state.PC++);
            state.C = mem.r8(state.IY + d);
            return 19;
            break;
        case 0x4f:
            // LD C,A
            state.C = state.A;
            return 8;
            break;
        case 0x50:
            // LD D,B
            state.D = state.B;
            return 8;
            break;
        case 0x51:
            // LD D,C
            state.D = state.C;
            return 8;
            break;
        case 0x52:
            // LD D,D
            state.D = state.D;
            return 8;
            break;
        case 0x53:
            // LD D,E
            state.D = state.E;
            return 8;
            break;
        case 0x54:
            // LD D,IYH
            state.D = state.IYH;
            return 8;
            break;
        case 0x55:
            // LD D,IYL
            state.D = state.IYL;
            return 8;
            break;
        case 0x56:
            // LD D,(IY+d)
            d = mem.rs8(state.PC++);
            state.D = mem.r8(state.IY + d);
            return 19;
            break;
        case 0x57:
            // LD D,A
            state.D = state.A;
            return 8;
            break;
        case 0x58:
            // LD E,B
            state.E = state.B;
            return 8;
            break;
        case 0x59:
            // LD E,C
            state.E = state.C;
            return 8;
            break;
        case 0x5a:
            // LD E,D
            state.E = state.D;
            return 8;
            break;
        case 0x5b:
            // LD E,E
            state.E = state.E;
            return 8;
            break;
        case 0x5c:
            // LD E,IYH
            state.E = state.IYH;
            return 8;
            break;
        case 0x5d:
            // LD E,IYL
            state.E = state.IYL;
            return 8;
            break;
        case 0x5e:
            // LD E,(IY+d)
            d = mem.rs8(state.PC++);
            state.E = mem.r8(state.IY + d);
            return 19;
            break;
        case 0x5f:
            // LD E,A
            state.E = state.A;
            return 8;
            break;
        case 0x60:
            // LD IYH,B
            state.IYH = state.B;
            return 8;
            break;
        case 0x61:
            // LD IYH,C
            state.IYH = state.C;
            return 8;
            break;
        case 0x62:
            // LD IYH,D
            state.IYH = state.D;
            return 8;
            break;
        case 0x63:
            // LD IYH,E
            state.IYH = state.E;
            return 8;
            break;
        case 0x64:
            // LD IYH,IYH
            state.IYH = state.IYH;
            return 8;
            break;
        case 0x65:
            // LD IYH,IYL
            state.IYH = state.IYL;
            return 8;
            break;
        case 0x66:
            // LD H,(IY+d)
            d = mem.rs8(state.PC++);
            state.H = mem.r8(state.IY + d);
            return 19;
            break;
        case 0x67:
            // LD IYH,A
            state.IYH = state.A;
            return 8;
            break;
        case 0x68:
            // LD IYL,B
            state.IYL = state.B;
            return 8;
            break;
        case 0x69:
            // LD IYL,C
            state.IYL = state.C;
            return 8;
            break;
        case 0x6a:
            // LD IYL,D
            state.IYL = state.D;
            return 8;
            break;
        case 0x6b:
            // LD IYL,E
            state.IYL = state.E;
            return 8;
            break;
        case 0x6c:
            // LD IYL,IYH
            state.IYL = state.IYH;
            return 8;
            break;
        case 0x6d:
            // LD IYL,IYL
            state.IYL = state.IYL;
            return 8;
            break;
        case 0x6e:
            // LD L,(IY+d)
            d = mem.rs8(state.PC++);
            state.L = mem.r8(state.IY + d);
            return 19;
            break;
        case 0x6f:
            // LD IYL,A
            state.IYL = state.A;
            return 8;
            break;
        case 0x70:
            // LD (IY+d),B
            d = mem.rs8(state.PC++);
            mem.w8(state.IY + d, state.B);
            return 19;
            break;
        case 0x71:
            // LD (IY+d),C
            d = mem.rs8(state.PC++);
            mem.w8(state.IY + d, state.C);
            return 19;
            break;
        case 0x72:
            // LD (IY+d),D
            d = mem.rs8(state.PC++);
            mem.w8(state.IY + d, state.D);
            return 19;
            break;
        case 0x73:
            // LD (IY+d),E
            d = mem.rs8(state.PC++);
            mem.w8(state.IY + d, state.E);
            return 19;
            break;
        case 0x74:
            // LD (IY+d),H
            d = mem.rs8(state.PC++);
            mem.w8(state.IY + d, state.H);
            return 19;
            break;
        case 0x75:
            // LD (IY+d),L
            d = mem.rs8(state.PC++);
            mem.w8(state.IY + d, state.L);
            return 19;
            break;
        case 0x77:
            // LD (IY+d),A
            d = mem.rs8(state.PC++);
            mem.w8(state.IY + d, state.A);
            return 19;
            break;
        case 0x78:
            // LD A,B
            state.A = state.B;
            return 8;
            break;
        case 0x79:
            // LD A,C
            state.A = state.C;
            return 8;
            break;
        case 0x7a:
            // LD A,D
            state.A = state.D;
            return 8;
            break;
        case 0x7b:
            // LD A,E
            state.A = state.E;
            return 8;
            break;
        case 0x7c:
            // LD A,IYH
            state.A = state.IYH;
            return 8;
            break;
        case 0x7d:
            // LD A,IYL
            state.A = state.IYL;
            return 8;
            break;
        case 0x7e:
            // LD A,(IY+d)
            d = mem.rs8(state.PC++);
            state.A = mem.r8(state.IY + d);
            return 19;
            break;
        case 0x7f:
            // LD A,A
            state.A = state.A;
            return 8;
            break;
        case 0x84:
            // ADD IYH
            add8(state.IYH);
            return 8;
            break;
        case 0x85:
            // ADD IYL
            add8(state.IYL);
            return 8;
            break;
        case 0x86:
            // ADD(IY+d)
            d = mem.rs8(state.PC++);
            add8(mem.r8(state.IY + d));
            return 19;
            break;
        case 0x8c:
            // ADC IYH
            adc8(state.IYH);
            return 8;
            break;
        case 0x8d:
            // ADC IYL
            adc8(state.IYL);
            return 8;
            break;
        case 0x8e:
            // ADC(IY+d)
            d = mem.rs8(state.PC++);
            adc8(mem.r8(state.IY + d));
            return 19;
            break;
        case 0x94:
            // SUB IYH
            sub8(state.IYH);
            return 8;
            break;
        case 0x95:
            // SUB IYL
            sub8(state.IYL);
            return 8;
            break;
        case 0x96:
            // SUB(IY+d)
            d = mem.rs8(state.PC++);
            sub8(mem.r8(state.IY + d));
            return 19;
            break;
        case 0x9c:
            // SBC IYH
            sbc8(state.IYH);
            return 8;
            break;
        case 0x9d:
            // SBC IYL
            sbc8(state.IYL);
            return 8;
            break;
        case 0x9e:
            // SBC(IY+d)
            d = mem.rs8(state.PC++);
            sbc8(mem.r8(state.IY + d));
            return 19;
            break;
        case 0xa4:
            // AND IYH
            and8(state.IYH);
            return 8;
            break;
        case 0xa5:
            // AND IYL
            and8(state.IYL);
            return 8;
            break;
        case 0xa6:
            // AND(IY+d)
            d = mem.rs8(state.PC++);
            and8(mem.r8(state.IY + d));
            return 19;
            break;
        case 0xac:
            // XOR IYH
            xor8(state.IYH);
            return 8;
            break;
        case 0xad:
            // XOR IYL
            xor8(state.IYL);
            return 8;
            break;
        case 0xae:
            // XOR(IY+d)
            d = mem.rs8(state.PC++);
            xor8(mem.r8(state.IY + d));
            return 19;
            break;
        case 0xb4:
            // OR IYH
            or8(state.IYH);
            return 8;
            break;
        case 0xb5:
            // OR IYL
            or8(state.IYL);
            return 8;
            break;
        case 0xb6:
            // OR(IY+d)
            d = mem.rs8(state.PC++);
            or8(mem.r8(state.IY + d));
            return 19;
            break;
        case 0xbc:
            // CP IYH
            cp8(state.IYH);
            return 8;
            break;
        case 0xbd:
            // CP IYL
            cp8(state.IYL);
            return 8;
            break;
        case 0xbe:
            // CP(IY+d)
            d = mem.rs8(state.PC++);
            cp8(mem.r8(state.IY + d));
            return 19;
            break;
        case 0xcb:
            // RLC ([IX|IY]+d)
            // RRC ([IX|IY]+d)
            // RL ([IX|IY]+d)
            // RR ([IX|IY]+d)
            // SLA ([IX|IY]+d)
            // SRA ([IX|IY]+d)
            // SRL ([IX|IY]+d)
            // BIT b,([IX|IY]+d
            // SET b,([IX|IY]+d
            // RES b,([IX|IY]+d
            return dd_fd_cb(0xfd);
            break;
        case 0xe1:
            // POP IY
            state.IY = mem.r16(state.SP);
            state.SP += 2;
            return 14;
            break;
        case 0xe3:
            // EX (SP),IY
            u16tmp = mem.r16(state.SP);
            mem.w16(state.SP, state.IY);
            state.IY = u16tmp;
            return 23;
            break;
        case 0xe5:
            // PUSH IY
            state.SP -= 2;
            mem.w16(state.SP, state.IY);
            return 15;
            break;
        case 0xe9:
            // JP (IY)
            state.PC = state.IY;
            return 8;
            break;
        case 0xf9:
            // LD SP,IY
            state.SP = state.IY;
            return 10;
            break;
        default:
            return invalid_opcode(2);
            break;
        }
        break;
    case 0xfe:
        // CP n
        cp8(mem.r8(state.PC++));
        return 7;
        break;
    default:
       return invalid_opcode(1);
       break;
    }
#ifdef _DEBUG
    YAKC_ASSERT(false);
#endif
    return 0;
}
} // namespace yakc
