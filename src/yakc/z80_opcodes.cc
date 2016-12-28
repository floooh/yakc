// #version:2#
// machine generated, do not edit!
#include "z80.h"
namespace YAKC {
uint32_t z80::do_op(system_bus* bus) {
  switch (fetch_op()) {
    case 0x0: return 4; // NOP
    case 0x1: BC=mem.r16(PC); PC+=2; return 10; // LD BC,nn
    case 0x2: mem.w8(BC,A); Z=C+1; W=A; return 7; // LD (BC),A
    case 0x3: BC++; return 6; // INC BC
    case 0x4: B=inc8(B); return 4; // INC B
    case 0x5: B=dec8(B); return 4; // DEC B
    case 0x6: B=mem.r8(PC++); return 7; // LD B,n
    case 0x7: rlca8(); return 4; // RLCA
    case 0x8: swap16(AF,AF_); return 4; // EX AF,AF'
    case 0x9: HL=add16(HL,BC); return 11; // ADD HL,BC
    case 0xa: A=mem.r8(BC); WZ=BC+1; return 7; // LD A,(BC)
    case 0xb: BC--; return 6; // DEC BC
    case 0xc: C=inc8(C); return 4; // INC C
    case 0xd: C=dec8(C); return 4; // DEC C
    case 0xe: C=mem.r8(PC++); return 7; // LD C,n
    case 0xf: rrca8(); return 4; // RRCA
    case 0x10: if (--B>0) { WZ=PC=PC+mem.rs8(PC)+1; return 13; } else { PC++; return 8; } // DJNZ
    case 0x11: DE=mem.r16(PC); PC+=2; return 10; // LD DE,nn
    case 0x12: mem.w8(DE,A); Z=E+1; W=A; return 7; // LD (DE),A
    case 0x13: DE++; return 6; // INC DE
    case 0x14: D=inc8(D); return 4; // INC D
    case 0x15: D=dec8(D); return 4; // DEC D
    case 0x16: D=mem.r8(PC++); return 7; // LD D,n
    case 0x17: rla8(); return 4; // RLA
    case 0x18: WZ=PC=PC+mem.rs8(PC)+1; return 12; // JR d
    case 0x19: HL=add16(HL,DE); return 11; // ADD HL,DE
    case 0x1a: A=mem.r8(DE); WZ=DE+1; return 7; // LD A,(DE)
    case 0x1b: DE--; return 6; // DEC DE
    case 0x1c: E=inc8(E); return 4; // INC E
    case 0x1d: E=dec8(E); return 4; // DEC E
    case 0x1e: E=mem.r8(PC++); return 7; // LD E,n
    case 0x1f: rra8(); return 4; // RRA
    case 0x20: if (!(F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return 12; } else { PC++; return 7; } // JR NZ,d
    case 0x21: HL=mem.r16(PC); PC+=2; return 10; // LD HL,nn
    case 0x22: WZ=mem.r16(PC); mem.w16(WZ++,HL); PC+=2; return 16; // LD (nn),HL
    case 0x23: HL++; return 6; // INC HL
    case 0x24: H=inc8(H); return 4; // INC H
    case 0x25: H=dec8(H); return 4; // DEC H
    case 0x26: H=mem.r8(PC++); return 7; // LD H,n
    case 0x27: daa(); return 4; // DAA
    case 0x28: if ((F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return 12; } else { PC++; return 7; } // JR Z,d
    case 0x29: HL=add16(HL,HL); return 11; // ADD HL,HL
    case 0x2a: WZ=mem.r16(PC); HL=mem.r16(WZ++); PC+=2; return 16; // LD HL,(nn)
    case 0x2b: HL--; return 6; // DEC HL
    case 0x2c: L=inc8(L); return 4; // INC L
    case 0x2d: L=dec8(L); return 4; // DEC L
    case 0x2e: L=mem.r8(PC++); return 7; // LD L,n
    case 0x2f: A^=0xFF; F=(F&(SF|ZF|PF|CF))|HF|NF|(A&(YF|XF)); return 4; // CPL
    case 0x30: if (!(F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return 12; } else { PC++; return 7; } // JR NC,d
    case 0x31: SP=mem.r16(PC); PC+=2; return 10; // LD SP,nn
    case 0x32: WZ=mem.r16(PC); mem.w8(WZ++,A); W=A; PC+=2; return 13; // LD (nn),A
    case 0x33: SP++; return 6; // INC SP
    case 0x34: { uword a=HL; mem.w8(a,inc8(mem.r8(a))); } return 11; // INC (HL)
    case 0x35: { uword a=HL; mem.w8(a,dec8(mem.r8(a))); } return 11; // DEC (HL)
    case 0x36: { uword a=HL; mem.w8(a,mem.r8(PC++)); } return 10; // LD (HL),n
    case 0x37: F=(F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF)); return 4; // SCF
    case 0x38: if ((F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return 12; } else { PC++; return 7; } // JR C,d
    case 0x39: HL=add16(HL,SP); return 11; // ADD HL,SP
    case 0x3a: WZ=mem.r16(PC); A=mem.r8(WZ++); PC+=2; return 13; // LD A,(nn)
    case 0x3b: SP--; return 6; // DEC SP
    case 0x3c: A=inc8(A); return 4; // INC A
    case 0x3d: A=dec8(A); return 4; // DEC A
    case 0x3e: A=mem.r8(PC++); return 7; // LD A,n
    case 0x3f: F=((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF; return 4; // CCF
    case 0x40: B=B; return 4; // LD B,B
    case 0x41: B=C; return 4; // LD B,C
    case 0x42: B=D; return 4; // LD B,D
    case 0x43: B=E; return 4; // LD B,E
    case 0x44: B=H; return 4; // LD B,H
    case 0x45: B=L; return 4; // LD B,L
    case 0x46: { uword a=HL; B=mem.r8(a); } return 7; // LD B,(HL)
    case 0x47: B=A; return 4; // LD B,A
    case 0x48: C=B; return 4; // LD C,B
    case 0x49: C=C; return 4; // LD C,C
    case 0x4a: C=D; return 4; // LD C,D
    case 0x4b: C=E; return 4; // LD C,E
    case 0x4c: C=H; return 4; // LD C,H
    case 0x4d: C=L; return 4; // LD C,L
    case 0x4e: { uword a=HL; C=mem.r8(a); } return 7; // LD C,(HL)
    case 0x4f: C=A; return 4; // LD C,A
    case 0x50: D=B; return 4; // LD D,B
    case 0x51: D=C; return 4; // LD D,C
    case 0x52: D=D; return 4; // LD D,D
    case 0x53: D=E; return 4; // LD D,E
    case 0x54: D=H; return 4; // LD D,H
    case 0x55: D=L; return 4; // LD D,L
    case 0x56: { uword a=HL; D=mem.r8(a); } return 7; // LD D,(HL)
    case 0x57: D=A; return 4; // LD D,A
    case 0x58: E=B; return 4; // LD E,B
    case 0x59: E=C; return 4; // LD E,C
    case 0x5a: E=D; return 4; // LD E,D
    case 0x5b: E=E; return 4; // LD E,E
    case 0x5c: E=H; return 4; // LD E,H
    case 0x5d: E=L; return 4; // LD E,L
    case 0x5e: { uword a=HL; E=mem.r8(a); } return 7; // LD E,(HL)
    case 0x5f: E=A; return 4; // LD E,A
    case 0x60: H=B; return 4; // LD H,B
    case 0x61: H=C; return 4; // LD H,C
    case 0x62: H=D; return 4; // LD H,D
    case 0x63: H=E; return 4; // LD H,E
    case 0x64: H=H; return 4; // LD H,H
    case 0x65: H=L; return 4; // LD H,L
    case 0x66: { uword a=HL; H=mem.r8(a); } return 7; // LD H,(HL)
    case 0x67: H=A; return 4; // LD H,A
    case 0x68: L=B; return 4; // LD L,B
    case 0x69: L=C; return 4; // LD L,C
    case 0x6a: L=D; return 4; // LD L,D
    case 0x6b: L=E; return 4; // LD L,E
    case 0x6c: L=H; return 4; // LD L,H
    case 0x6d: L=L; return 4; // LD L,L
    case 0x6e: { uword a=HL; L=mem.r8(a); } return 7; // LD L,(HL)
    case 0x6f: L=A; return 4; // LD L,A
    case 0x70: { uword a=HL; mem.w8(a,B); } return 7; // LD (HL),B
    case 0x71: { uword a=HL; mem.w8(a,C); } return 7; // LD (HL),C
    case 0x72: { uword a=HL; mem.w8(a,D); } return 7; // LD (HL),D
    case 0x73: { uword a=HL; mem.w8(a,E); } return 7; // LD (HL),E
    case 0x74: { uword a=HL; mem.w8(a,H); } return 7; // LD (HL),H
    case 0x75: { uword a=HL; mem.w8(a,L); } return 7; // LD (HL),L
    case 0x76: halt(); return 4; // HALT
    case 0x77: { uword a=HL; mem.w8(a,A); } return 7; // LD (HL),A
    case 0x78: A=B; return 4; // LD A,B
    case 0x79: A=C; return 4; // LD A,C
    case 0x7a: A=D; return 4; // LD A,D
    case 0x7b: A=E; return 4; // LD A,E
    case 0x7c: A=H; return 4; // LD A,H
    case 0x7d: A=L; return 4; // LD A,L
    case 0x7e: { uword a=HL; A=mem.r8(a); } return 7; // LD A,(HL)
    case 0x7f: A=A; return 4; // LD A,A
    case 0x80: add8(B); return 4; // ADD B
    case 0x81: add8(C); return 4; // ADD C
    case 0x82: add8(D); return 4; // ADD D
    case 0x83: add8(E); return 4; // ADD E
    case 0x84: add8(H); return 4; // ADD H
    case 0x85: add8(L); return 4; // ADD L
    case 0x86: { uword a=HL; add8(mem.r8(a)); } return 7; // ADD (HL)
    case 0x87: add8(A); return 4; // ADD A
    case 0x88: adc8(B); return 4; // ADC B
    case 0x89: adc8(C); return 4; // ADC C
    case 0x8a: adc8(D); return 4; // ADC D
    case 0x8b: adc8(E); return 4; // ADC E
    case 0x8c: adc8(H); return 4; // ADC H
    case 0x8d: adc8(L); return 4; // ADC L
    case 0x8e: { uword a=HL; adc8(mem.r8(a)); } return 7; // ADC (HL)
    case 0x8f: adc8(A); return 4; // ADC A
    case 0x90: sub8(B); return 4; // SUB B
    case 0x91: sub8(C); return 4; // SUB C
    case 0x92: sub8(D); return 4; // SUB D
    case 0x93: sub8(E); return 4; // SUB E
    case 0x94: sub8(H); return 4; // SUB H
    case 0x95: sub8(L); return 4; // SUB L
    case 0x96: { uword a=HL; sub8(mem.r8(a)); } return 7; // SUB (HL)
    case 0x97: sub8(A); return 4; // SUB A
    case 0x98: sbc8(B); return 4; // SBC B
    case 0x99: sbc8(C); return 4; // SBC C
    case 0x9a: sbc8(D); return 4; // SBC D
    case 0x9b: sbc8(E); return 4; // SBC E
    case 0x9c: sbc8(H); return 4; // SBC H
    case 0x9d: sbc8(L); return 4; // SBC L
    case 0x9e: { uword a=HL; sbc8(mem.r8(a)); } return 7; // SBC (HL)
    case 0x9f: sbc8(A); return 4; // SBC A
    case 0xa0: and8(B); return 4; // AND B
    case 0xa1: and8(C); return 4; // AND C
    case 0xa2: and8(D); return 4; // AND D
    case 0xa3: and8(E); return 4; // AND E
    case 0xa4: and8(H); return 4; // AND H
    case 0xa5: and8(L); return 4; // AND L
    case 0xa6: { uword a=HL; and8(mem.r8(a)); } return 7; // AND (HL)
    case 0xa7: and8(A); return 4; // AND A
    case 0xa8: xor8(B); return 4; // XOR B
    case 0xa9: xor8(C); return 4; // XOR C
    case 0xaa: xor8(D); return 4; // XOR D
    case 0xab: xor8(E); return 4; // XOR E
    case 0xac: xor8(H); return 4; // XOR H
    case 0xad: xor8(L); return 4; // XOR L
    case 0xae: { uword a=HL; xor8(mem.r8(a)); } return 7; // XOR (HL)
    case 0xaf: xor8(A); return 4; // XOR A
    case 0xb0: or8(B); return 4; // OR B
    case 0xb1: or8(C); return 4; // OR C
    case 0xb2: or8(D); return 4; // OR D
    case 0xb3: or8(E); return 4; // OR E
    case 0xb4: or8(H); return 4; // OR H
    case 0xb5: or8(L); return 4; // OR L
    case 0xb6: { uword a=HL; or8(mem.r8(a)); } return 7; // OR (HL)
    case 0xb7: or8(A); return 4; // OR A
    case 0xb8: cp8(B); return 4; // CP B
    case 0xb9: cp8(C); return 4; // CP C
    case 0xba: cp8(D); return 4; // CP D
    case 0xbb: cp8(E); return 4; // CP E
    case 0xbc: cp8(H); return 4; // CP H
    case 0xbd: cp8(L); return 4; // CP L
    case 0xbe: { uword a=HL; cp8(mem.r8(a)); } return 7; // CP (HL)
    case 0xbf: cp8(A); return 4; // CP A
    case 0xc0: if (!(F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return 11; } else return 5; // RET NZ
    case 0xc1: BC=mem.r16(SP); SP+=2; return 10; // POP BC
    case 0xc2: WZ=mem.r16(PC); if (!(F&ZF)) { PC=WZ; } else { PC+=2; }; return 10; // JP NZ,nn
    case 0xc3: WZ=PC=mem.r16(PC); return 10; // JP nn
    case 0xc4: WZ=mem.r16(PC); PC+=2; if (!(F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 17; } else { return 10; } // CALL NZ,nn
    case 0xc5: SP-=2; mem.w16(SP,BC); return 11; // PUSH BC
    case 0xc6: add8(mem.r8(PC++)); return 7; // ADD n
    case 0xc7: rst(0x0); return 11; // RST 0x0
    case 0xc8: if ((F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return 11; } else return 5; // RET Z
    case 0xc9: WZ=PC=mem.r16(SP); SP+=2; return 10; // RET
    case 0xca: WZ=mem.r16(PC); if ((F&ZF)) { PC=WZ; } else { PC+=2; }; return 10; // JP Z,nn
    case 0xcb:
      switch (fetch_op()) {
        case 0x0: B=rlc8(B); return 8; // RLC B
        case 0x1: C=rlc8(C); return 8; // RLC C
        case 0x2: D=rlc8(D); return 8; // RLC D
        case 0x3: E=rlc8(E); return 8; // RLC E
        case 0x4: H=rlc8(H); return 8; // RLC H
        case 0x5: L=rlc8(L); return 8; // RLC L
        case 0x6: { uword a=HL; mem.w8(a,rlc8(mem.r8(a))); } return 15; // RLC (HL)
        case 0x7: A=rlc8(A); return 8; // RLC A
        case 0x8: B=rrc8(B); return 8; // RRC B
        case 0x9: C=rrc8(C); return 8; // RRC C
        case 0xa: D=rrc8(D); return 8; // RRC D
        case 0xb: E=rrc8(E); return 8; // RRC E
        case 0xc: H=rrc8(H); return 8; // RRC H
        case 0xd: L=rrc8(L); return 8; // RRC L
        case 0xe: { uword a=HL; mem.w8(a,rrc8(mem.r8(a))); } return 15; // RRC (HL)
        case 0xf: A=rrc8(A); return 8; // RRC A
        case 0x10: B=rl8(B); return 8; // RL B
        case 0x11: C=rl8(C); return 8; // RL C
        case 0x12: D=rl8(D); return 8; // RL D
        case 0x13: E=rl8(E); return 8; // RL E
        case 0x14: H=rl8(H); return 8; // RL H
        case 0x15: L=rl8(L); return 8; // RL L
        case 0x16: { uword a=HL; mem.w8(a,rl8(mem.r8(a))); } return 15; // RL (HL)
        case 0x17: A=rl8(A); return 8; // RL A
        case 0x18: B=rr8(B); return 8; // RR B
        case 0x19: C=rr8(C); return 8; // RR C
        case 0x1a: D=rr8(D); return 8; // RR D
        case 0x1b: E=rr8(E); return 8; // RR E
        case 0x1c: H=rr8(H); return 8; // RR H
        case 0x1d: L=rr8(L); return 8; // RR L
        case 0x1e: { uword a=HL; mem.w8(a,rr8(mem.r8(a))); } return 15; // RR (HL)
        case 0x1f: A=rr8(A); return 8; // RR A
        case 0x20: B=sla8(B); return 8; // SLA B
        case 0x21: C=sla8(C); return 8; // SLA C
        case 0x22: D=sla8(D); return 8; // SLA D
        case 0x23: E=sla8(E); return 8; // SLA E
        case 0x24: H=sla8(H); return 8; // SLA H
        case 0x25: L=sla8(L); return 8; // SLA L
        case 0x26: { uword a=HL; mem.w8(a,sla8(mem.r8(a))); } return 15; // SLA (HL)
        case 0x27: A=sla8(A); return 8; // SLA A
        case 0x28: B=sra8(B); return 8; // SRA B
        case 0x29: C=sra8(C); return 8; // SRA C
        case 0x2a: D=sra8(D); return 8; // SRA D
        case 0x2b: E=sra8(E); return 8; // SRA E
        case 0x2c: H=sra8(H); return 8; // SRA H
        case 0x2d: L=sra8(L); return 8; // SRA L
        case 0x2e: { uword a=HL; mem.w8(a,sra8(mem.r8(a))); } return 15; // SRA (HL)
        case 0x2f: A=sra8(A); return 8; // SRA A
        case 0x30: B=sll8(B); return 8; // SLL B
        case 0x31: C=sll8(C); return 8; // SLL C
        case 0x32: D=sll8(D); return 8; // SLL D
        case 0x33: E=sll8(E); return 8; // SLL E
        case 0x34: H=sll8(H); return 8; // SLL H
        case 0x35: L=sll8(L); return 8; // SLL L
        case 0x36: { uword a=HL; mem.w8(a,sll8(mem.r8(a))); } return 15; // SLL (HL)
        case 0x37: A=sll8(A); return 8; // SLL A
        case 0x38: B=srl8(B); return 8; // SRL B
        case 0x39: C=srl8(C); return 8; // SRL C
        case 0x3a: D=srl8(D); return 8; // SRL D
        case 0x3b: E=srl8(E); return 8; // SRL E
        case 0x3c: H=srl8(H); return 8; // SRL H
        case 0x3d: L=srl8(L); return 8; // SRL L
        case 0x3e: { uword a=HL; mem.w8(a,srl8(mem.r8(a))); } return 15; // SRL (HL)
        case 0x3f: A=srl8(A); return 8; // SRL A
        case 0x40: bit(B,0x1); return 8; // BIT 0,B
        case 0x41: bit(C,0x1); return 8; // BIT 0,C
        case 0x42: bit(D,0x1); return 8; // BIT 0,D
        case 0x43: bit(E,0x1); return 8; // BIT 0,E
        case 0x44: bit(H,0x1); return 8; // BIT 0,H
        case 0x45: bit(L,0x1); return 8; // BIT 0,L
        case 0x46: { uword a=HL; ibit(mem.r8(a),0x1); } return 12; // BIT 0,(HL)
        case 0x47: bit(A,0x1); return 8; // BIT 0,A
        case 0x48: bit(B,0x2); return 8; // BIT 1,B
        case 0x49: bit(C,0x2); return 8; // BIT 1,C
        case 0x4a: bit(D,0x2); return 8; // BIT 1,D
        case 0x4b: bit(E,0x2); return 8; // BIT 1,E
        case 0x4c: bit(H,0x2); return 8; // BIT 1,H
        case 0x4d: bit(L,0x2); return 8; // BIT 1,L
        case 0x4e: { uword a=HL; ibit(mem.r8(a),0x2); } return 12; // BIT 1,(HL)
        case 0x4f: bit(A,0x2); return 8; // BIT 1,A
        case 0x50: bit(B,0x4); return 8; // BIT 2,B
        case 0x51: bit(C,0x4); return 8; // BIT 2,C
        case 0x52: bit(D,0x4); return 8; // BIT 2,D
        case 0x53: bit(E,0x4); return 8; // BIT 2,E
        case 0x54: bit(H,0x4); return 8; // BIT 2,H
        case 0x55: bit(L,0x4); return 8; // BIT 2,L
        case 0x56: { uword a=HL; ibit(mem.r8(a),0x4); } return 12; // BIT 2,(HL)
        case 0x57: bit(A,0x4); return 8; // BIT 2,A
        case 0x58: bit(B,0x8); return 8; // BIT 3,B
        case 0x59: bit(C,0x8); return 8; // BIT 3,C
        case 0x5a: bit(D,0x8); return 8; // BIT 3,D
        case 0x5b: bit(E,0x8); return 8; // BIT 3,E
        case 0x5c: bit(H,0x8); return 8; // BIT 3,H
        case 0x5d: bit(L,0x8); return 8; // BIT 3,L
        case 0x5e: { uword a=HL; ibit(mem.r8(a),0x8); } return 12; // BIT 3,(HL)
        case 0x5f: bit(A,0x8); return 8; // BIT 3,A
        case 0x60: bit(B,0x10); return 8; // BIT 4,B
        case 0x61: bit(C,0x10); return 8; // BIT 4,C
        case 0x62: bit(D,0x10); return 8; // BIT 4,D
        case 0x63: bit(E,0x10); return 8; // BIT 4,E
        case 0x64: bit(H,0x10); return 8; // BIT 4,H
        case 0x65: bit(L,0x10); return 8; // BIT 4,L
        case 0x66: { uword a=HL; ibit(mem.r8(a),0x10); } return 12; // BIT 4,(HL)
        case 0x67: bit(A,0x10); return 8; // BIT 4,A
        case 0x68: bit(B,0x20); return 8; // BIT 5,B
        case 0x69: bit(C,0x20); return 8; // BIT 5,C
        case 0x6a: bit(D,0x20); return 8; // BIT 5,D
        case 0x6b: bit(E,0x20); return 8; // BIT 5,E
        case 0x6c: bit(H,0x20); return 8; // BIT 5,H
        case 0x6d: bit(L,0x20); return 8; // BIT 5,L
        case 0x6e: { uword a=HL; ibit(mem.r8(a),0x20); } return 12; // BIT 5,(HL)
        case 0x6f: bit(A,0x20); return 8; // BIT 5,A
        case 0x70: bit(B,0x40); return 8; // BIT 6,B
        case 0x71: bit(C,0x40); return 8; // BIT 6,C
        case 0x72: bit(D,0x40); return 8; // BIT 6,D
        case 0x73: bit(E,0x40); return 8; // BIT 6,E
        case 0x74: bit(H,0x40); return 8; // BIT 6,H
        case 0x75: bit(L,0x40); return 8; // BIT 6,L
        case 0x76: { uword a=HL; ibit(mem.r8(a),0x40); } return 12; // BIT 6,(HL)
        case 0x77: bit(A,0x40); return 8; // BIT 6,A
        case 0x78: bit(B,0x80); return 8; // BIT 7,B
        case 0x79: bit(C,0x80); return 8; // BIT 7,C
        case 0x7a: bit(D,0x80); return 8; // BIT 7,D
        case 0x7b: bit(E,0x80); return 8; // BIT 7,E
        case 0x7c: bit(H,0x80); return 8; // BIT 7,H
        case 0x7d: bit(L,0x80); return 8; // BIT 7,L
        case 0x7e: { uword a=HL; ibit(mem.r8(a),0x80); } return 12; // BIT 7,(HL)
        case 0x7f: bit(A,0x80); return 8; // BIT 7,A
        case 0x80: B&=~0x1; return 8; // RES 0,B
        case 0x81: C&=~0x1; return 8; // RES 0,C
        case 0x82: D&=~0x1; return 8; // RES 0,D
        case 0x83: E&=~0x1; return 8; // RES 0,E
        case 0x84: H&=~0x1; return 8; // RES 0,H
        case 0x85: L&=~0x1; return 8; // RES 0,L
        case 0x86: { uword a=HL; mem.w8(a,mem.r8(a)&~0x1); } return 15; // RES 0,(HL)
        case 0x87: A&=~0x1; return 8; // RES 0,A
        case 0x88: B&=~0x2; return 8; // RES 1,B
        case 0x89: C&=~0x2; return 8; // RES 1,C
        case 0x8a: D&=~0x2; return 8; // RES 1,D
        case 0x8b: E&=~0x2; return 8; // RES 1,E
        case 0x8c: H&=~0x2; return 8; // RES 1,H
        case 0x8d: L&=~0x2; return 8; // RES 1,L
        case 0x8e: { uword a=HL; mem.w8(a,mem.r8(a)&~0x2); } return 15; // RES 1,(HL)
        case 0x8f: A&=~0x2; return 8; // RES 1,A
        case 0x90: B&=~0x4; return 8; // RES 2,B
        case 0x91: C&=~0x4; return 8; // RES 2,C
        case 0x92: D&=~0x4; return 8; // RES 2,D
        case 0x93: E&=~0x4; return 8; // RES 2,E
        case 0x94: H&=~0x4; return 8; // RES 2,H
        case 0x95: L&=~0x4; return 8; // RES 2,L
        case 0x96: { uword a=HL; mem.w8(a,mem.r8(a)&~0x4); } return 15; // RES 2,(HL)
        case 0x97: A&=~0x4; return 8; // RES 2,A
        case 0x98: B&=~0x8; return 8; // RES 3,B
        case 0x99: C&=~0x8; return 8; // RES 3,C
        case 0x9a: D&=~0x8; return 8; // RES 3,D
        case 0x9b: E&=~0x8; return 8; // RES 3,E
        case 0x9c: H&=~0x8; return 8; // RES 3,H
        case 0x9d: L&=~0x8; return 8; // RES 3,L
        case 0x9e: { uword a=HL; mem.w8(a,mem.r8(a)&~0x8); } return 15; // RES 3,(HL)
        case 0x9f: A&=~0x8; return 8; // RES 3,A
        case 0xa0: B&=~0x10; return 8; // RES 4,B
        case 0xa1: C&=~0x10; return 8; // RES 4,C
        case 0xa2: D&=~0x10; return 8; // RES 4,D
        case 0xa3: E&=~0x10; return 8; // RES 4,E
        case 0xa4: H&=~0x10; return 8; // RES 4,H
        case 0xa5: L&=~0x10; return 8; // RES 4,L
        case 0xa6: { uword a=HL; mem.w8(a,mem.r8(a)&~0x10); } return 15; // RES 4,(HL)
        case 0xa7: A&=~0x10; return 8; // RES 4,A
        case 0xa8: B&=~0x20; return 8; // RES 5,B
        case 0xa9: C&=~0x20; return 8; // RES 5,C
        case 0xaa: D&=~0x20; return 8; // RES 5,D
        case 0xab: E&=~0x20; return 8; // RES 5,E
        case 0xac: H&=~0x20; return 8; // RES 5,H
        case 0xad: L&=~0x20; return 8; // RES 5,L
        case 0xae: { uword a=HL; mem.w8(a,mem.r8(a)&~0x20); } return 15; // RES 5,(HL)
        case 0xaf: A&=~0x20; return 8; // RES 5,A
        case 0xb0: B&=~0x40; return 8; // RES 6,B
        case 0xb1: C&=~0x40; return 8; // RES 6,C
        case 0xb2: D&=~0x40; return 8; // RES 6,D
        case 0xb3: E&=~0x40; return 8; // RES 6,E
        case 0xb4: H&=~0x40; return 8; // RES 6,H
        case 0xb5: L&=~0x40; return 8; // RES 6,L
        case 0xb6: { uword a=HL; mem.w8(a,mem.r8(a)&~0x40); } return 15; // RES 6,(HL)
        case 0xb7: A&=~0x40; return 8; // RES 6,A
        case 0xb8: B&=~0x80; return 8; // RES 7,B
        case 0xb9: C&=~0x80; return 8; // RES 7,C
        case 0xba: D&=~0x80; return 8; // RES 7,D
        case 0xbb: E&=~0x80; return 8; // RES 7,E
        case 0xbc: H&=~0x80; return 8; // RES 7,H
        case 0xbd: L&=~0x80; return 8; // RES 7,L
        case 0xbe: { uword a=HL; mem.w8(a,mem.r8(a)&~0x80); } return 15; // RES 7,(HL)
        case 0xbf: A&=~0x80; return 8; // RES 7,A
        case 0xc0: B|=0x1; return 8; // SET 0,B
        case 0xc1: C|=0x1; return 8; // SET 0,C
        case 0xc2: D|=0x1; return 8; // SET 0,D
        case 0xc3: E|=0x1; return 8; // SET 0,E
        case 0xc4: H|=0x1; return 8; // SET 0,H
        case 0xc5: L|=0x1; return 8; // SET 0,L
        case 0xc6: { uword a=HL; mem.w8(a,mem.r8(a)|0x1);} return 15; // SET 0,(HL)
        case 0xc7: A|=0x1; return 8; // SET 0,A
        case 0xc8: B|=0x2; return 8; // SET 1,B
        case 0xc9: C|=0x2; return 8; // SET 1,C
        case 0xca: D|=0x2; return 8; // SET 1,D
        case 0xcb: E|=0x2; return 8; // SET 1,E
        case 0xcc: H|=0x2; return 8; // SET 1,H
        case 0xcd: L|=0x2; return 8; // SET 1,L
        case 0xce: { uword a=HL; mem.w8(a,mem.r8(a)|0x2);} return 15; // SET 1,(HL)
        case 0xcf: A|=0x2; return 8; // SET 1,A
        case 0xd0: B|=0x4; return 8; // SET 2,B
        case 0xd1: C|=0x4; return 8; // SET 2,C
        case 0xd2: D|=0x4; return 8; // SET 2,D
        case 0xd3: E|=0x4; return 8; // SET 2,E
        case 0xd4: H|=0x4; return 8; // SET 2,H
        case 0xd5: L|=0x4; return 8; // SET 2,L
        case 0xd6: { uword a=HL; mem.w8(a,mem.r8(a)|0x4);} return 15; // SET 2,(HL)
        case 0xd7: A|=0x4; return 8; // SET 2,A
        case 0xd8: B|=0x8; return 8; // SET 3,B
        case 0xd9: C|=0x8; return 8; // SET 3,C
        case 0xda: D|=0x8; return 8; // SET 3,D
        case 0xdb: E|=0x8; return 8; // SET 3,E
        case 0xdc: H|=0x8; return 8; // SET 3,H
        case 0xdd: L|=0x8; return 8; // SET 3,L
        case 0xde: { uword a=HL; mem.w8(a,mem.r8(a)|0x8);} return 15; // SET 3,(HL)
        case 0xdf: A|=0x8; return 8; // SET 3,A
        case 0xe0: B|=0x10; return 8; // SET 4,B
        case 0xe1: C|=0x10; return 8; // SET 4,C
        case 0xe2: D|=0x10; return 8; // SET 4,D
        case 0xe3: E|=0x10; return 8; // SET 4,E
        case 0xe4: H|=0x10; return 8; // SET 4,H
        case 0xe5: L|=0x10; return 8; // SET 4,L
        case 0xe6: { uword a=HL; mem.w8(a,mem.r8(a)|0x10);} return 15; // SET 4,(HL)
        case 0xe7: A|=0x10; return 8; // SET 4,A
        case 0xe8: B|=0x20; return 8; // SET 5,B
        case 0xe9: C|=0x20; return 8; // SET 5,C
        case 0xea: D|=0x20; return 8; // SET 5,D
        case 0xeb: E|=0x20; return 8; // SET 5,E
        case 0xec: H|=0x20; return 8; // SET 5,H
        case 0xed: L|=0x20; return 8; // SET 5,L
        case 0xee: { uword a=HL; mem.w8(a,mem.r8(a)|0x20);} return 15; // SET 5,(HL)
        case 0xef: A|=0x20; return 8; // SET 5,A
        case 0xf0: B|=0x40; return 8; // SET 6,B
        case 0xf1: C|=0x40; return 8; // SET 6,C
        case 0xf2: D|=0x40; return 8; // SET 6,D
        case 0xf3: E|=0x40; return 8; // SET 6,E
        case 0xf4: H|=0x40; return 8; // SET 6,H
        case 0xf5: L|=0x40; return 8; // SET 6,L
        case 0xf6: { uword a=HL; mem.w8(a,mem.r8(a)|0x40);} return 15; // SET 6,(HL)
        case 0xf7: A|=0x40; return 8; // SET 6,A
        case 0xf8: B|=0x80; return 8; // SET 7,B
        case 0xf9: C|=0x80; return 8; // SET 7,C
        case 0xfa: D|=0x80; return 8; // SET 7,D
        case 0xfb: E|=0x80; return 8; // SET 7,E
        case 0xfc: H|=0x80; return 8; // SET 7,H
        case 0xfd: L|=0x80; return 8; // SET 7,L
        case 0xfe: { uword a=HL; mem.w8(a,mem.r8(a)|0x80);} return 15; // SET 7,(HL)
        case 0xff: A|=0x80; return 8; // SET 7,A
        default: return invalid_opcode(2);
      }
      break;
    case 0xcc: WZ=mem.r16(PC); PC+=2; if ((F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 17; } else { return 10; } // CALL Z,nn
    case 0xcd: SP-=2; mem.w16(SP,PC+2); WZ=PC=mem.r16(PC); return 17; // CALL nn
    case 0xce: adc8(mem.r8(PC++)); return 7; // ADC n
    case 0xcf: rst(0x8); return 11; // RST 0x8
    case 0xd0: if (!(F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return 11; } else return 5; // RET NC
    case 0xd1: DE=mem.r16(SP); SP+=2; return 10; // POP DE
    case 0xd2: WZ=mem.r16(PC); if (!(F&CF)) { PC=WZ; } else { PC+=2; }; return 10; // JP NC,nn
    case 0xd3: out(bus, (A<<8)|mem.r8(PC++),A); return 11; // OUT (n),A
    case 0xd4: WZ=mem.r16(PC); PC+=2; if (!(F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 17; } else { return 10; } // CALL NC,nn
    case 0xd5: SP-=2; mem.w16(SP,DE); return 11; // PUSH DE
    case 0xd6: sub8(mem.r8(PC++)); return 7; // SUB n
    case 0xd7: rst(0x10); return 11; // RST 0x10
    case 0xd8: if ((F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return 11; } else return 5; // RET C
    case 0xd9: swap16(BC,BC_); swap16(DE,DE_); swap16(HL,HL_); swap16(WZ,WZ_); return 4; // EXX
    case 0xda: WZ=mem.r16(PC); if ((F&CF)) { PC=WZ; } else { PC+=2; }; return 10; // JP C,nn
    case 0xdb: A=in(bus, (A<<8)|mem.r8(PC++)); return 11; // IN A,(n)
    case 0xdc: WZ=mem.r16(PC); PC+=2; if ((F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 17; } else { return 10; } // CALL C,nn
    case 0xdd:
      switch (fetch_op()) {
        case 0x0: return 8; // NOP
        case 0x1: BC=mem.r16(PC); PC+=2; return 14; // LD BC,nn
        case 0x2: mem.w8(BC,A); Z=C+1; W=A; return 11; // LD (BC),A
        case 0x3: BC++; return 10; // INC BC
        case 0x4: B=inc8(B); return 8; // INC B
        case 0x5: B=dec8(B); return 8; // DEC B
        case 0x6: B=mem.r8(PC++); return 11; // LD B,n
        case 0x7: rlca8(); return 8; // RLCA
        case 0x8: swap16(AF,AF_); return 8; // EX AF,AF'
        case 0x9: IX=add16(IX,BC); return 15; // ADD IX,BC
        case 0xa: A=mem.r8(BC); WZ=BC+1; return 11; // LD A,(BC)
        case 0xb: BC--; return 10; // DEC BC
        case 0xc: C=inc8(C); return 8; // INC C
        case 0xd: C=dec8(C); return 8; // DEC C
        case 0xe: C=mem.r8(PC++); return 11; // LD C,n
        case 0xf: rrca8(); return 8; // RRCA
        case 0x10: if (--B>0) { WZ=PC=PC+mem.rs8(PC)+1; return 17; } else { PC++; return 12; } // DJNZ
        case 0x11: DE=mem.r16(PC); PC+=2; return 14; // LD DE,nn
        case 0x12: mem.w8(DE,A); Z=E+1; W=A; return 11; // LD (DE),A
        case 0x13: DE++; return 10; // INC DE
        case 0x14: D=inc8(D); return 8; // INC D
        case 0x15: D=dec8(D); return 8; // DEC D
        case 0x16: D=mem.r8(PC++); return 11; // LD D,n
        case 0x17: rla8(); return 8; // RLA
        case 0x18: WZ=PC=PC+mem.rs8(PC)+1; return 16; // JR d
        case 0x19: IX=add16(IX,DE); return 15; // ADD IX,DE
        case 0x1a: A=mem.r8(DE); WZ=DE+1; return 11; // LD A,(DE)
        case 0x1b: DE--; return 10; // DEC DE
        case 0x1c: E=inc8(E); return 8; // INC E
        case 0x1d: E=dec8(E); return 8; // DEC E
        case 0x1e: E=mem.r8(PC++); return 11; // LD E,n
        case 0x1f: rra8(); return 8; // RRA
        case 0x20: if (!(F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return 16; } else { PC++; return 11; } // JR NZ,d
        case 0x21: IX=mem.r16(PC); PC+=2; return 14; // LD IX,nn
        case 0x22: WZ=mem.r16(PC); mem.w16(WZ++,IX); PC+=2; return 20; // LD (nn),IX
        case 0x23: IX++; return 10; // INC IX
        case 0x24: IXH=inc8(IXH); return 8; // INC IXH
        case 0x25: IXH=dec8(IXH); return 8; // DEC IXH
        case 0x26: IXH=mem.r8(PC++); return 11; // LD IXH,n
        case 0x27: daa(); return 8; // DAA
        case 0x28: if ((F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return 16; } else { PC++; return 11; } // JR Z,d
        case 0x29: IX=add16(IX,IX); return 15; // ADD IX,IX
        case 0x2a: WZ=mem.r16(PC); IX=mem.r16(WZ++); PC+=2; return 20; // LD IX,(nn)
        case 0x2b: IX--; return 10; // DEC IX
        case 0x2c: IXL=inc8(IXL); return 8; // INC IXL
        case 0x2d: IXL=dec8(IXL); return 8; // DEC IXL
        case 0x2e: IXL=mem.r8(PC++); return 11; // LD IXL,n
        case 0x2f: A^=0xFF; F=(F&(SF|ZF|PF|CF))|HF|NF|(A&(YF|XF)); return 8; // CPL
        case 0x30: if (!(F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return 16; } else { PC++; return 11; } // JR NC,d
        case 0x31: SP=mem.r16(PC); PC+=2; return 14; // LD SP,nn
        case 0x32: WZ=mem.r16(PC); mem.w8(WZ++,A); W=A; PC+=2; return 17; // LD (nn),A
        case 0x33: SP++; return 10; // INC SP
        case 0x34: { uword a=WZ=IX+mem.rs8(PC++); mem.w8(a,inc8(mem.r8(a))); } return 23; // INC (IX+d)
        case 0x35: { uword a=WZ=IX+mem.rs8(PC++); mem.w8(a,dec8(mem.r8(a))); } return 23; // DEC (IX+d)
        case 0x36: { uword a=WZ=IX+mem.rs8(PC++); mem.w8(a,mem.r8(PC++)); } return 19; // LD (IX+d),n
        case 0x37: F=(F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF)); return 8; // SCF
        case 0x38: if ((F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return 16; } else { PC++; return 11; } // JR C,d
        case 0x39: IX=add16(IX,SP); return 15; // ADD IX,SP
        case 0x3a: WZ=mem.r16(PC); A=mem.r8(WZ++); PC+=2; return 17; // LD A,(nn)
        case 0x3b: SP--; return 10; // DEC SP
        case 0x3c: A=inc8(A); return 8; // INC A
        case 0x3d: A=dec8(A); return 8; // DEC A
        case 0x3e: A=mem.r8(PC++); return 11; // LD A,n
        case 0x3f: F=((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF; return 8; // CCF
        case 0x40: B=B; return 8; // LD B,B
        case 0x41: B=C; return 8; // LD B,C
        case 0x42: B=D; return 8; // LD B,D
        case 0x43: B=E; return 8; // LD B,E
        case 0x44: B=IXH; return 8; // LD B,IXH
        case 0x45: B=IXL; return 8; // LD B,IXL
        case 0x46: { uword a=WZ=IX+mem.rs8(PC++); B=mem.r8(a); } return 19; // LD B,(IX+d)
        case 0x47: B=A; return 8; // LD B,A
        case 0x48: C=B; return 8; // LD C,B
        case 0x49: C=C; return 8; // LD C,C
        case 0x4a: C=D; return 8; // LD C,D
        case 0x4b: C=E; return 8; // LD C,E
        case 0x4c: C=IXH; return 8; // LD C,IXH
        case 0x4d: C=IXL; return 8; // LD C,IXL
        case 0x4e: { uword a=WZ=IX+mem.rs8(PC++); C=mem.r8(a); } return 19; // LD C,(IX+d)
        case 0x4f: C=A; return 8; // LD C,A
        case 0x50: D=B; return 8; // LD D,B
        case 0x51: D=C; return 8; // LD D,C
        case 0x52: D=D; return 8; // LD D,D
        case 0x53: D=E; return 8; // LD D,E
        case 0x54: D=IXH; return 8; // LD D,IXH
        case 0x55: D=IXL; return 8; // LD D,IXL
        case 0x56: { uword a=WZ=IX+mem.rs8(PC++); D=mem.r8(a); } return 19; // LD D,(IX+d)
        case 0x57: D=A; return 8; // LD D,A
        case 0x58: E=B; return 8; // LD E,B
        case 0x59: E=C; return 8; // LD E,C
        case 0x5a: E=D; return 8; // LD E,D
        case 0x5b: E=E; return 8; // LD E,E
        case 0x5c: E=IXH; return 8; // LD E,IXH
        case 0x5d: E=IXL; return 8; // LD E,IXL
        case 0x5e: { uword a=WZ=IX+mem.rs8(PC++); E=mem.r8(a); } return 19; // LD E,(IX+d)
        case 0x5f: E=A; return 8; // LD E,A
        case 0x60: IXH=B; return 8; // LD IXH,B
        case 0x61: IXH=C; return 8; // LD IXH,C
        case 0x62: IXH=D; return 8; // LD IXH,D
        case 0x63: IXH=E; return 8; // LD IXH,E
        case 0x64: IXH=IXH; return 8; // LD IXH,IXH
        case 0x65: IXH=IXL; return 8; // LD IXH,IXL
        case 0x66: { uword a=WZ=IX+mem.rs8(PC++); H=mem.r8(a); } return 19; // LD H,(IX+d)
        case 0x67: IXH=A; return 8; // LD IXH,A
        case 0x68: IXL=B; return 8; // LD IXL,B
        case 0x69: IXL=C; return 8; // LD IXL,C
        case 0x6a: IXL=D; return 8; // LD IXL,D
        case 0x6b: IXL=E; return 8; // LD IXL,E
        case 0x6c: IXL=IXH; return 8; // LD IXL,IXH
        case 0x6d: IXL=IXL; return 8; // LD IXL,IXL
        case 0x6e: { uword a=WZ=IX+mem.rs8(PC++); L=mem.r8(a); } return 19; // LD L,(IX+d)
        case 0x6f: IXL=A; return 8; // LD IXL,A
        case 0x70: { uword a=WZ=IX+mem.rs8(PC++); mem.w8(a,B); } return 19; // LD (IX+d),B
        case 0x71: { uword a=WZ=IX+mem.rs8(PC++); mem.w8(a,C); } return 19; // LD (IX+d),C
        case 0x72: { uword a=WZ=IX+mem.rs8(PC++); mem.w8(a,D); } return 19; // LD (IX+d),D
        case 0x73: { uword a=WZ=IX+mem.rs8(PC++); mem.w8(a,E); } return 19; // LD (IX+d),E
        case 0x74: { uword a=WZ=IX+mem.rs8(PC++); mem.w8(a,H); } return 19; // LD (IX+d),H
        case 0x75: { uword a=WZ=IX+mem.rs8(PC++); mem.w8(a,L); } return 19; // LD (IX+d),L
        case 0x76: halt(); return 8; // HALT
        case 0x77: { uword a=WZ=IX+mem.rs8(PC++); mem.w8(a,A); } return 19; // LD (IX+d),A
        case 0x78: A=B; return 8; // LD A,B
        case 0x79: A=C; return 8; // LD A,C
        case 0x7a: A=D; return 8; // LD A,D
        case 0x7b: A=E; return 8; // LD A,E
        case 0x7c: A=IXH; return 8; // LD A,IXH
        case 0x7d: A=IXL; return 8; // LD A,IXL
        case 0x7e: { uword a=WZ=IX+mem.rs8(PC++); A=mem.r8(a); } return 19; // LD A,(IX+d)
        case 0x7f: A=A; return 8; // LD A,A
        case 0x80: add8(B); return 8; // ADD B
        case 0x81: add8(C); return 8; // ADD C
        case 0x82: add8(D); return 8; // ADD D
        case 0x83: add8(E); return 8; // ADD E
        case 0x84: add8(IXH); return 8; // ADD IXH
        case 0x85: add8(IXL); return 8; // ADD IXL
        case 0x86: { uword a=WZ=IX+mem.rs8(PC++); add8(mem.r8(a)); } return 19; // ADD (IX+d)
        case 0x87: add8(A); return 8; // ADD A
        case 0x88: adc8(B); return 8; // ADC B
        case 0x89: adc8(C); return 8; // ADC C
        case 0x8a: adc8(D); return 8; // ADC D
        case 0x8b: adc8(E); return 8; // ADC E
        case 0x8c: adc8(IXH); return 8; // ADC IXH
        case 0x8d: adc8(IXL); return 8; // ADC IXL
        case 0x8e: { uword a=WZ=IX+mem.rs8(PC++); adc8(mem.r8(a)); } return 19; // ADC (IX+d)
        case 0x8f: adc8(A); return 8; // ADC A
        case 0x90: sub8(B); return 8; // SUB B
        case 0x91: sub8(C); return 8; // SUB C
        case 0x92: sub8(D); return 8; // SUB D
        case 0x93: sub8(E); return 8; // SUB E
        case 0x94: sub8(IXH); return 8; // SUB IXH
        case 0x95: sub8(IXL); return 8; // SUB IXL
        case 0x96: { uword a=WZ=IX+mem.rs8(PC++); sub8(mem.r8(a)); } return 19; // SUB (IX+d)
        case 0x97: sub8(A); return 8; // SUB A
        case 0x98: sbc8(B); return 8; // SBC B
        case 0x99: sbc8(C); return 8; // SBC C
        case 0x9a: sbc8(D); return 8; // SBC D
        case 0x9b: sbc8(E); return 8; // SBC E
        case 0x9c: sbc8(IXH); return 8; // SBC IXH
        case 0x9d: sbc8(IXL); return 8; // SBC IXL
        case 0x9e: { uword a=WZ=IX+mem.rs8(PC++); sbc8(mem.r8(a)); } return 19; // SBC (IX+d)
        case 0x9f: sbc8(A); return 8; // SBC A
        case 0xa0: and8(B); return 8; // AND B
        case 0xa1: and8(C); return 8; // AND C
        case 0xa2: and8(D); return 8; // AND D
        case 0xa3: and8(E); return 8; // AND E
        case 0xa4: and8(IXH); return 8; // AND IXH
        case 0xa5: and8(IXL); return 8; // AND IXL
        case 0xa6: { uword a=WZ=IX+mem.rs8(PC++); and8(mem.r8(a)); } return 19; // AND (IX+d)
        case 0xa7: and8(A); return 8; // AND A
        case 0xa8: xor8(B); return 8; // XOR B
        case 0xa9: xor8(C); return 8; // XOR C
        case 0xaa: xor8(D); return 8; // XOR D
        case 0xab: xor8(E); return 8; // XOR E
        case 0xac: xor8(IXH); return 8; // XOR IXH
        case 0xad: xor8(IXL); return 8; // XOR IXL
        case 0xae: { uword a=WZ=IX+mem.rs8(PC++); xor8(mem.r8(a)); } return 19; // XOR (IX+d)
        case 0xaf: xor8(A); return 8; // XOR A
        case 0xb0: or8(B); return 8; // OR B
        case 0xb1: or8(C); return 8; // OR C
        case 0xb2: or8(D); return 8; // OR D
        case 0xb3: or8(E); return 8; // OR E
        case 0xb4: or8(IXH); return 8; // OR IXH
        case 0xb5: or8(IXL); return 8; // OR IXL
        case 0xb6: { uword a=WZ=IX+mem.rs8(PC++); or8(mem.r8(a)); } return 19; // OR (IX+d)
        case 0xb7: or8(A); return 8; // OR A
        case 0xb8: cp8(B); return 8; // CP B
        case 0xb9: cp8(C); return 8; // CP C
        case 0xba: cp8(D); return 8; // CP D
        case 0xbb: cp8(E); return 8; // CP E
        case 0xbc: cp8(IXH); return 8; // CP IXH
        case 0xbd: cp8(IXL); return 8; // CP IXL
        case 0xbe: { uword a=WZ=IX+mem.rs8(PC++); cp8(mem.r8(a)); } return 19; // CP (IX+d)
        case 0xbf: cp8(A); return 8; // CP A
        case 0xc0: if (!(F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET NZ
        case 0xc1: BC=mem.r16(SP); SP+=2; return 14; // POP BC
        case 0xc2: WZ=mem.r16(PC); if (!(F&ZF)) { PC=WZ; } else { PC+=2; }; return 14; // JP NZ,nn
        case 0xc3: WZ=PC=mem.r16(PC); return 14; // JP nn
        case 0xc4: WZ=mem.r16(PC); PC+=2; if (!(F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL NZ,nn
        case 0xc5: SP-=2; mem.w16(SP,BC); return 15; // PUSH BC
        case 0xc6: add8(mem.r8(PC++)); return 11; // ADD n
        case 0xc7: rst(0x0); return 15; // RST 0x0
        case 0xc8: if ((F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET Z
        case 0xc9: WZ=PC=mem.r16(SP); SP+=2; return 14; // RET
        case 0xca: WZ=mem.r16(PC); if ((F&ZF)) { PC=WZ; } else { PC+=2; }; return 14; // JP Z,nn
        case 0xcb:
          { const int d = mem.rs8(PC++);
          switch (fetch_op()) {
            case 0x0: { uword a=WZ=IX+d;; B=rlc8(mem.r8(a)); mem.w8(a,B); } return 23; // RLC (IX+d),B
            case 0x1: { uword a=WZ=IX+d;; C=rlc8(mem.r8(a)); mem.w8(a,C); } return 23; // RLC (IX+d),C
            case 0x2: { uword a=WZ=IX+d;; D=rlc8(mem.r8(a)); mem.w8(a,D); } return 23; // RLC (IX+d),D
            case 0x3: { uword a=WZ=IX+d;; E=rlc8(mem.r8(a)); mem.w8(a,E); } return 23; // RLC (IX+d),E
            case 0x4: { uword a=WZ=IX+d;; H=rlc8(mem.r8(a)); mem.w8(a,H); } return 23; // RLC (IX+d),H
            case 0x5: { uword a=WZ=IX+d;; L=rlc8(mem.r8(a)); mem.w8(a,L); } return 23; // RLC (IX+d),L
            case 0x6: { uword a=WZ=IX+d;; mem.w8(a,rlc8(mem.r8(a))); } return 23; // RLC (IX+d)
            case 0x7: { uword a=WZ=IX+d;; A=rlc8(mem.r8(a)); mem.w8(a,A); } return 23; // RLC (IX+d),A
            case 0x8: { uword a=WZ=IX+d;; B=rrc8(mem.r8(a)); mem.w8(a,B); } return 23; // RRC (IX+d),B
            case 0x9: { uword a=WZ=IX+d;; C=rrc8(mem.r8(a)); mem.w8(a,C); } return 23; // RRC (IX+d),C
            case 0xa: { uword a=WZ=IX+d;; D=rrc8(mem.r8(a)); mem.w8(a,D); } return 23; // RRC (IX+d),D
            case 0xb: { uword a=WZ=IX+d;; E=rrc8(mem.r8(a)); mem.w8(a,E); } return 23; // RRC (IX+d),E
            case 0xc: { uword a=WZ=IX+d;; H=rrc8(mem.r8(a)); mem.w8(a,H); } return 23; // RRC (IX+d),H
            case 0xd: { uword a=WZ=IX+d;; L=rrc8(mem.r8(a)); mem.w8(a,L); } return 23; // RRC (IX+d),L
            case 0xe: { uword a=WZ=IX+d;; mem.w8(a,rrc8(mem.r8(a))); } return 23; // RRC (IX+d)
            case 0xf: { uword a=WZ=IX+d;; A=rrc8(mem.r8(a)); mem.w8(a,A); } return 23; // RRC (IX+d),A
            case 0x10: { uword a=WZ=IX+d;; B=rl8(mem.r8(a)); mem.w8(a,B); } return 23; // RL (IX+d),B
            case 0x11: { uword a=WZ=IX+d;; C=rl8(mem.r8(a)); mem.w8(a,C); } return 23; // RL (IX+d),C
            case 0x12: { uword a=WZ=IX+d;; D=rl8(mem.r8(a)); mem.w8(a,D); } return 23; // RL (IX+d),D
            case 0x13: { uword a=WZ=IX+d;; E=rl8(mem.r8(a)); mem.w8(a,E); } return 23; // RL (IX+d),E
            case 0x14: { uword a=WZ=IX+d;; H=rl8(mem.r8(a)); mem.w8(a,H); } return 23; // RL (IX+d),H
            case 0x15: { uword a=WZ=IX+d;; L=rl8(mem.r8(a)); mem.w8(a,L); } return 23; // RL (IX+d),L
            case 0x16: { uword a=WZ=IX+d;; mem.w8(a,rl8(mem.r8(a))); } return 23; // RL (IX+d)
            case 0x17: { uword a=WZ=IX+d;; A=rl8(mem.r8(a)); mem.w8(a,A); } return 23; // RL (IX+d),A
            case 0x18: { uword a=WZ=IX+d;; B=rr8(mem.r8(a)); mem.w8(a,B); } return 23; // RR (IX+d),B
            case 0x19: { uword a=WZ=IX+d;; C=rr8(mem.r8(a)); mem.w8(a,C); } return 23; // RR (IX+d),C
            case 0x1a: { uword a=WZ=IX+d;; D=rr8(mem.r8(a)); mem.w8(a,D); } return 23; // RR (IX+d),D
            case 0x1b: { uword a=WZ=IX+d;; E=rr8(mem.r8(a)); mem.w8(a,E); } return 23; // RR (IX+d),E
            case 0x1c: { uword a=WZ=IX+d;; H=rr8(mem.r8(a)); mem.w8(a,H); } return 23; // RR (IX+d),H
            case 0x1d: { uword a=WZ=IX+d;; L=rr8(mem.r8(a)); mem.w8(a,L); } return 23; // RR (IX+d),L
            case 0x1e: { uword a=WZ=IX+d;; mem.w8(a,rr8(mem.r8(a))); } return 23; // RR (IX+d)
            case 0x1f: { uword a=WZ=IX+d;; A=rr8(mem.r8(a)); mem.w8(a,A); } return 23; // RR (IX+d),A
            case 0x20: { uword a=WZ=IX+d;; B=sla8(mem.r8(a)); mem.w8(a,B); } return 23; // SLA (IX+d),B
            case 0x21: { uword a=WZ=IX+d;; C=sla8(mem.r8(a)); mem.w8(a,C); } return 23; // SLA (IX+d),C
            case 0x22: { uword a=WZ=IX+d;; D=sla8(mem.r8(a)); mem.w8(a,D); } return 23; // SLA (IX+d),D
            case 0x23: { uword a=WZ=IX+d;; E=sla8(mem.r8(a)); mem.w8(a,E); } return 23; // SLA (IX+d),E
            case 0x24: { uword a=WZ=IX+d;; H=sla8(mem.r8(a)); mem.w8(a,H); } return 23; // SLA (IX+d),H
            case 0x25: { uword a=WZ=IX+d;; L=sla8(mem.r8(a)); mem.w8(a,L); } return 23; // SLA (IX+d),L
            case 0x26: { uword a=WZ=IX+d;; mem.w8(a,sla8(mem.r8(a))); } return 23; // SLA (IX+d)
            case 0x27: { uword a=WZ=IX+d;; A=sla8(mem.r8(a)); mem.w8(a,A); } return 23; // SLA (IX+d),A
            case 0x28: { uword a=WZ=IX+d;; B=sra8(mem.r8(a)); mem.w8(a,B); } return 23; // SRA (IX+d),B
            case 0x29: { uword a=WZ=IX+d;; C=sra8(mem.r8(a)); mem.w8(a,C); } return 23; // SRA (IX+d),C
            case 0x2a: { uword a=WZ=IX+d;; D=sra8(mem.r8(a)); mem.w8(a,D); } return 23; // SRA (IX+d),D
            case 0x2b: { uword a=WZ=IX+d;; E=sra8(mem.r8(a)); mem.w8(a,E); } return 23; // SRA (IX+d),E
            case 0x2c: { uword a=WZ=IX+d;; H=sra8(mem.r8(a)); mem.w8(a,H); } return 23; // SRA (IX+d),H
            case 0x2d: { uword a=WZ=IX+d;; L=sra8(mem.r8(a)); mem.w8(a,L); } return 23; // SRA (IX+d),L
            case 0x2e: { uword a=WZ=IX+d;; mem.w8(a,sra8(mem.r8(a))); } return 23; // SRA (IX+d)
            case 0x2f: { uword a=WZ=IX+d;; A=sra8(mem.r8(a)); mem.w8(a,A); } return 23; // SRA (IX+d),A
            case 0x30: { uword a=WZ=IX+d;; B=sll8(mem.r8(a)); mem.w8(a,B); } return 23; // SLL (IX+d),B
            case 0x31: { uword a=WZ=IX+d;; C=sll8(mem.r8(a)); mem.w8(a,C); } return 23; // SLL (IX+d),C
            case 0x32: { uword a=WZ=IX+d;; D=sll8(mem.r8(a)); mem.w8(a,D); } return 23; // SLL (IX+d),D
            case 0x33: { uword a=WZ=IX+d;; E=sll8(mem.r8(a)); mem.w8(a,E); } return 23; // SLL (IX+d),E
            case 0x34: { uword a=WZ=IX+d;; H=sll8(mem.r8(a)); mem.w8(a,H); } return 23; // SLL (IX+d),H
            case 0x35: { uword a=WZ=IX+d;; L=sll8(mem.r8(a)); mem.w8(a,L); } return 23; // SLL (IX+d),L
            case 0x36: { uword a=WZ=IX+d;; mem.w8(a,sll8(mem.r8(a))); } return 23; // SLL (IX+d)
            case 0x37: { uword a=WZ=IX+d;; A=sll8(mem.r8(a)); mem.w8(a,A); } return 23; // SLL (IX+d),A
            case 0x38: { uword a=WZ=IX+d;; B=srl8(mem.r8(a)); mem.w8(a,B); } return 23; // SRL (IX+d),B
            case 0x39: { uword a=WZ=IX+d;; C=srl8(mem.r8(a)); mem.w8(a,C); } return 23; // SRL (IX+d),C
            case 0x3a: { uword a=WZ=IX+d;; D=srl8(mem.r8(a)); mem.w8(a,D); } return 23; // SRL (IX+d),D
            case 0x3b: { uword a=WZ=IX+d;; E=srl8(mem.r8(a)); mem.w8(a,E); } return 23; // SRL (IX+d),E
            case 0x3c: { uword a=WZ=IX+d;; H=srl8(mem.r8(a)); mem.w8(a,H); } return 23; // SRL (IX+d),H
            case 0x3d: { uword a=WZ=IX+d;; L=srl8(mem.r8(a)); mem.w8(a,L); } return 23; // SRL (IX+d),L
            case 0x3e: { uword a=WZ=IX+d;; mem.w8(a,srl8(mem.r8(a))); } return 23; // SRL (IX+d)
            case 0x3f: { uword a=WZ=IX+d;; A=srl8(mem.r8(a)); mem.w8(a,A); } return 23; // SRL (IX+d),A
            case 0x40: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IX+d)
            case 0x41: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IX+d)
            case 0x42: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IX+d)
            case 0x43: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IX+d)
            case 0x44: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IX+d)
            case 0x45: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IX+d)
            case 0x46: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IX+d)
            case 0x47: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IX+d)
            case 0x48: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IX+d)
            case 0x49: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IX+d)
            case 0x4a: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IX+d)
            case 0x4b: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IX+d)
            case 0x4c: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IX+d)
            case 0x4d: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IX+d)
            case 0x4e: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IX+d)
            case 0x4f: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IX+d)
            case 0x50: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IX+d)
            case 0x51: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IX+d)
            case 0x52: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IX+d)
            case 0x53: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IX+d)
            case 0x54: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IX+d)
            case 0x55: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IX+d)
            case 0x56: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IX+d)
            case 0x57: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IX+d)
            case 0x58: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IX+d)
            case 0x59: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IX+d)
            case 0x5a: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IX+d)
            case 0x5b: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IX+d)
            case 0x5c: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IX+d)
            case 0x5d: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IX+d)
            case 0x5e: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IX+d)
            case 0x5f: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IX+d)
            case 0x60: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IX+d)
            case 0x61: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IX+d)
            case 0x62: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IX+d)
            case 0x63: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IX+d)
            case 0x64: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IX+d)
            case 0x65: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IX+d)
            case 0x66: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IX+d)
            case 0x67: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IX+d)
            case 0x68: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IX+d)
            case 0x69: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IX+d)
            case 0x6a: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IX+d)
            case 0x6b: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IX+d)
            case 0x6c: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IX+d)
            case 0x6d: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IX+d)
            case 0x6e: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IX+d)
            case 0x6f: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IX+d)
            case 0x70: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IX+d)
            case 0x71: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IX+d)
            case 0x72: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IX+d)
            case 0x73: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IX+d)
            case 0x74: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IX+d)
            case 0x75: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IX+d)
            case 0x76: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IX+d)
            case 0x77: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IX+d)
            case 0x78: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IX+d)
            case 0x79: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IX+d)
            case 0x7a: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IX+d)
            case 0x7b: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IX+d)
            case 0x7c: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IX+d)
            case 0x7d: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IX+d)
            case 0x7e: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IX+d)
            case 0x7f: { uword a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IX+d)
            case 0x80: { uword a=WZ=IX+d;; B=mem.r8(a)&~0x1; mem.w8(a,B); } return 23; // RES 0,(IX+d),B
            case 0x81: { uword a=WZ=IX+d;; C=mem.r8(a)&~0x1; mem.w8(a,C); } return 23; // RES 0,(IX+d),C
            case 0x82: { uword a=WZ=IX+d;; D=mem.r8(a)&~0x1; mem.w8(a,D); } return 23; // RES 0,(IX+d),D
            case 0x83: { uword a=WZ=IX+d;; E=mem.r8(a)&~0x1; mem.w8(a,E); } return 23; // RES 0,(IX+d),E
            case 0x84: { uword a=WZ=IX+d;; H=mem.r8(a)&~0x1; mem.w8(a,H); } return 23; // RES 0,(IX+d),H
            case 0x85: { uword a=WZ=IX+d;; L=mem.r8(a)&~0x1; mem.w8(a,L); } return 23; // RES 0,(IX+d),L
            case 0x86: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x1); } return 23; // RES 0,(IX+d)
            case 0x87: { uword a=WZ=IX+d;; A=mem.r8(a)&~0x1; mem.w8(a,A); } return 23; // RES 0,(IX+d),A
            case 0x88: { uword a=WZ=IX+d;; B=mem.r8(a)&~0x2; mem.w8(a,B); } return 23; // RES 1,(IX+d),B
            case 0x89: { uword a=WZ=IX+d;; C=mem.r8(a)&~0x2; mem.w8(a,C); } return 23; // RES 1,(IX+d),C
            case 0x8a: { uword a=WZ=IX+d;; D=mem.r8(a)&~0x2; mem.w8(a,D); } return 23; // RES 1,(IX+d),D
            case 0x8b: { uword a=WZ=IX+d;; E=mem.r8(a)&~0x2; mem.w8(a,E); } return 23; // RES 1,(IX+d),E
            case 0x8c: { uword a=WZ=IX+d;; H=mem.r8(a)&~0x2; mem.w8(a,H); } return 23; // RES 1,(IX+d),H
            case 0x8d: { uword a=WZ=IX+d;; L=mem.r8(a)&~0x2; mem.w8(a,L); } return 23; // RES 1,(IX+d),L
            case 0x8e: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x2); } return 23; // RES 1,(IX+d)
            case 0x8f: { uword a=WZ=IX+d;; A=mem.r8(a)&~0x2; mem.w8(a,A); } return 23; // RES 1,(IX+d),A
            case 0x90: { uword a=WZ=IX+d;; B=mem.r8(a)&~0x4; mem.w8(a,B); } return 23; // RES 2,(IX+d),B
            case 0x91: { uword a=WZ=IX+d;; C=mem.r8(a)&~0x4; mem.w8(a,C); } return 23; // RES 2,(IX+d),C
            case 0x92: { uword a=WZ=IX+d;; D=mem.r8(a)&~0x4; mem.w8(a,D); } return 23; // RES 2,(IX+d),D
            case 0x93: { uword a=WZ=IX+d;; E=mem.r8(a)&~0x4; mem.w8(a,E); } return 23; // RES 2,(IX+d),E
            case 0x94: { uword a=WZ=IX+d;; H=mem.r8(a)&~0x4; mem.w8(a,H); } return 23; // RES 2,(IX+d),H
            case 0x95: { uword a=WZ=IX+d;; L=mem.r8(a)&~0x4; mem.w8(a,L); } return 23; // RES 2,(IX+d),L
            case 0x96: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x4); } return 23; // RES 2,(IX+d)
            case 0x97: { uword a=WZ=IX+d;; A=mem.r8(a)&~0x4; mem.w8(a,A); } return 23; // RES 2,(IX+d),A
            case 0x98: { uword a=WZ=IX+d;; B=mem.r8(a)&~0x8; mem.w8(a,B); } return 23; // RES 3,(IX+d),B
            case 0x99: { uword a=WZ=IX+d;; C=mem.r8(a)&~0x8; mem.w8(a,C); } return 23; // RES 3,(IX+d),C
            case 0x9a: { uword a=WZ=IX+d;; D=mem.r8(a)&~0x8; mem.w8(a,D); } return 23; // RES 3,(IX+d),D
            case 0x9b: { uword a=WZ=IX+d;; E=mem.r8(a)&~0x8; mem.w8(a,E); } return 23; // RES 3,(IX+d),E
            case 0x9c: { uword a=WZ=IX+d;; H=mem.r8(a)&~0x8; mem.w8(a,H); } return 23; // RES 3,(IX+d),H
            case 0x9d: { uword a=WZ=IX+d;; L=mem.r8(a)&~0x8; mem.w8(a,L); } return 23; // RES 3,(IX+d),L
            case 0x9e: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x8); } return 23; // RES 3,(IX+d)
            case 0x9f: { uword a=WZ=IX+d;; A=mem.r8(a)&~0x8; mem.w8(a,A); } return 23; // RES 3,(IX+d),A
            case 0xa0: { uword a=WZ=IX+d;; B=mem.r8(a)&~0x10; mem.w8(a,B); } return 23; // RES 4,(IX+d),B
            case 0xa1: { uword a=WZ=IX+d;; C=mem.r8(a)&~0x10; mem.w8(a,C); } return 23; // RES 4,(IX+d),C
            case 0xa2: { uword a=WZ=IX+d;; D=mem.r8(a)&~0x10; mem.w8(a,D); } return 23; // RES 4,(IX+d),D
            case 0xa3: { uword a=WZ=IX+d;; E=mem.r8(a)&~0x10; mem.w8(a,E); } return 23; // RES 4,(IX+d),E
            case 0xa4: { uword a=WZ=IX+d;; H=mem.r8(a)&~0x10; mem.w8(a,H); } return 23; // RES 4,(IX+d),H
            case 0xa5: { uword a=WZ=IX+d;; L=mem.r8(a)&~0x10; mem.w8(a,L); } return 23; // RES 4,(IX+d),L
            case 0xa6: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x10); } return 23; // RES 4,(IX+d)
            case 0xa7: { uword a=WZ=IX+d;; A=mem.r8(a)&~0x10; mem.w8(a,A); } return 23; // RES 4,(IX+d),A
            case 0xa8: { uword a=WZ=IX+d;; B=mem.r8(a)&~0x20; mem.w8(a,B); } return 23; // RES 5,(IX+d),B
            case 0xa9: { uword a=WZ=IX+d;; C=mem.r8(a)&~0x20; mem.w8(a,C); } return 23; // RES 5,(IX+d),C
            case 0xaa: { uword a=WZ=IX+d;; D=mem.r8(a)&~0x20; mem.w8(a,D); } return 23; // RES 5,(IX+d),D
            case 0xab: { uword a=WZ=IX+d;; E=mem.r8(a)&~0x20; mem.w8(a,E); } return 23; // RES 5,(IX+d),E
            case 0xac: { uword a=WZ=IX+d;; H=mem.r8(a)&~0x20; mem.w8(a,H); } return 23; // RES 5,(IX+d),H
            case 0xad: { uword a=WZ=IX+d;; L=mem.r8(a)&~0x20; mem.w8(a,L); } return 23; // RES 5,(IX+d),L
            case 0xae: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x20); } return 23; // RES 5,(IX+d)
            case 0xaf: { uword a=WZ=IX+d;; A=mem.r8(a)&~0x20; mem.w8(a,A); } return 23; // RES 5,(IX+d),A
            case 0xb0: { uword a=WZ=IX+d;; B=mem.r8(a)&~0x40; mem.w8(a,B); } return 23; // RES 6,(IX+d),B
            case 0xb1: { uword a=WZ=IX+d;; C=mem.r8(a)&~0x40; mem.w8(a,C); } return 23; // RES 6,(IX+d),C
            case 0xb2: { uword a=WZ=IX+d;; D=mem.r8(a)&~0x40; mem.w8(a,D); } return 23; // RES 6,(IX+d),D
            case 0xb3: { uword a=WZ=IX+d;; E=mem.r8(a)&~0x40; mem.w8(a,E); } return 23; // RES 6,(IX+d),E
            case 0xb4: { uword a=WZ=IX+d;; H=mem.r8(a)&~0x40; mem.w8(a,H); } return 23; // RES 6,(IX+d),H
            case 0xb5: { uword a=WZ=IX+d;; L=mem.r8(a)&~0x40; mem.w8(a,L); } return 23; // RES 6,(IX+d),L
            case 0xb6: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x40); } return 23; // RES 6,(IX+d)
            case 0xb7: { uword a=WZ=IX+d;; A=mem.r8(a)&~0x40; mem.w8(a,A); } return 23; // RES 6,(IX+d),A
            case 0xb8: { uword a=WZ=IX+d;; B=mem.r8(a)&~0x80; mem.w8(a,B); } return 23; // RES 7,(IX+d),B
            case 0xb9: { uword a=WZ=IX+d;; C=mem.r8(a)&~0x80; mem.w8(a,C); } return 23; // RES 7,(IX+d),C
            case 0xba: { uword a=WZ=IX+d;; D=mem.r8(a)&~0x80; mem.w8(a,D); } return 23; // RES 7,(IX+d),D
            case 0xbb: { uword a=WZ=IX+d;; E=mem.r8(a)&~0x80; mem.w8(a,E); } return 23; // RES 7,(IX+d),E
            case 0xbc: { uword a=WZ=IX+d;; H=mem.r8(a)&~0x80; mem.w8(a,H); } return 23; // RES 7,(IX+d),H
            case 0xbd: { uword a=WZ=IX+d;; L=mem.r8(a)&~0x80; mem.w8(a,L); } return 23; // RES 7,(IX+d),L
            case 0xbe: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x80); } return 23; // RES 7,(IX+d)
            case 0xbf: { uword a=WZ=IX+d;; A=mem.r8(a)&~0x80; mem.w8(a,A); } return 23; // RES 7,(IX+d),A
            case 0xc0: { uword a=WZ=IX+d;; B=mem.r8(a)|0x1; mem.w8(a,B);} return 23; // SET 0,(IX+d),B
            case 0xc1: { uword a=WZ=IX+d;; C=mem.r8(a)|0x1; mem.w8(a,C);} return 23; // SET 0,(IX+d),C
            case 0xc2: { uword a=WZ=IX+d;; D=mem.r8(a)|0x1; mem.w8(a,D);} return 23; // SET 0,(IX+d),D
            case 0xc3: { uword a=WZ=IX+d;; E=mem.r8(a)|0x1; mem.w8(a,E);} return 23; // SET 0,(IX+d),E
            case 0xc4: { uword a=WZ=IX+d;; H=mem.r8(a)|0x1; mem.w8(a,IXH);} return 23; // SET 0,(IX+d),H
            case 0xc5: { uword a=WZ=IX+d;; L=mem.r8(a)|0x1; mem.w8(a,IXL);} return 23; // SET 0,(IX+d),L
            case 0xc6: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x1);} return 23; // SET 0,(IX+d)
            case 0xc7: { uword a=WZ=IX+d;; A=mem.r8(a)|0x1; mem.w8(a,A);} return 23; // SET 0,(IX+d),A
            case 0xc8: { uword a=WZ=IX+d;; B=mem.r8(a)|0x2; mem.w8(a,B);} return 23; // SET 1,(IX+d),B
            case 0xc9: { uword a=WZ=IX+d;; C=mem.r8(a)|0x2; mem.w8(a,C);} return 23; // SET 1,(IX+d),C
            case 0xca: { uword a=WZ=IX+d;; D=mem.r8(a)|0x2; mem.w8(a,D);} return 23; // SET 1,(IX+d),D
            case 0xcb: { uword a=WZ=IX+d;; E=mem.r8(a)|0x2; mem.w8(a,E);} return 23; // SET 1,(IX+d),E
            case 0xcc: { uword a=WZ=IX+d;; H=mem.r8(a)|0x2; mem.w8(a,IXH);} return 23; // SET 1,(IX+d),H
            case 0xcd: { uword a=WZ=IX+d;; L=mem.r8(a)|0x2; mem.w8(a,IXL);} return 23; // SET 1,(IX+d),L
            case 0xce: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x2);} return 23; // SET 1,(IX+d)
            case 0xcf: { uword a=WZ=IX+d;; A=mem.r8(a)|0x2; mem.w8(a,A);} return 23; // SET 1,(IX+d),A
            case 0xd0: { uword a=WZ=IX+d;; B=mem.r8(a)|0x4; mem.w8(a,B);} return 23; // SET 2,(IX+d),B
            case 0xd1: { uword a=WZ=IX+d;; C=mem.r8(a)|0x4; mem.w8(a,C);} return 23; // SET 2,(IX+d),C
            case 0xd2: { uword a=WZ=IX+d;; D=mem.r8(a)|0x4; mem.w8(a,D);} return 23; // SET 2,(IX+d),D
            case 0xd3: { uword a=WZ=IX+d;; E=mem.r8(a)|0x4; mem.w8(a,E);} return 23; // SET 2,(IX+d),E
            case 0xd4: { uword a=WZ=IX+d;; H=mem.r8(a)|0x4; mem.w8(a,IXH);} return 23; // SET 2,(IX+d),H
            case 0xd5: { uword a=WZ=IX+d;; L=mem.r8(a)|0x4; mem.w8(a,IXL);} return 23; // SET 2,(IX+d),L
            case 0xd6: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x4);} return 23; // SET 2,(IX+d)
            case 0xd7: { uword a=WZ=IX+d;; A=mem.r8(a)|0x4; mem.w8(a,A);} return 23; // SET 2,(IX+d),A
            case 0xd8: { uword a=WZ=IX+d;; B=mem.r8(a)|0x8; mem.w8(a,B);} return 23; // SET 3,(IX+d),B
            case 0xd9: { uword a=WZ=IX+d;; C=mem.r8(a)|0x8; mem.w8(a,C);} return 23; // SET 3,(IX+d),C
            case 0xda: { uword a=WZ=IX+d;; D=mem.r8(a)|0x8; mem.w8(a,D);} return 23; // SET 3,(IX+d),D
            case 0xdb: { uword a=WZ=IX+d;; E=mem.r8(a)|0x8; mem.w8(a,E);} return 23; // SET 3,(IX+d),E
            case 0xdc: { uword a=WZ=IX+d;; H=mem.r8(a)|0x8; mem.w8(a,IXH);} return 23; // SET 3,(IX+d),H
            case 0xdd: { uword a=WZ=IX+d;; L=mem.r8(a)|0x8; mem.w8(a,IXL);} return 23; // SET 3,(IX+d),L
            case 0xde: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x8);} return 23; // SET 3,(IX+d)
            case 0xdf: { uword a=WZ=IX+d;; A=mem.r8(a)|0x8; mem.w8(a,A);} return 23; // SET 3,(IX+d),A
            case 0xe0: { uword a=WZ=IX+d;; B=mem.r8(a)|0x10; mem.w8(a,B);} return 23; // SET 4,(IX+d),B
            case 0xe1: { uword a=WZ=IX+d;; C=mem.r8(a)|0x10; mem.w8(a,C);} return 23; // SET 4,(IX+d),C
            case 0xe2: { uword a=WZ=IX+d;; D=mem.r8(a)|0x10; mem.w8(a,D);} return 23; // SET 4,(IX+d),D
            case 0xe3: { uword a=WZ=IX+d;; E=mem.r8(a)|0x10; mem.w8(a,E);} return 23; // SET 4,(IX+d),E
            case 0xe4: { uword a=WZ=IX+d;; H=mem.r8(a)|0x10; mem.w8(a,IXH);} return 23; // SET 4,(IX+d),H
            case 0xe5: { uword a=WZ=IX+d;; L=mem.r8(a)|0x10; mem.w8(a,IXL);} return 23; // SET 4,(IX+d),L
            case 0xe6: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x10);} return 23; // SET 4,(IX+d)
            case 0xe7: { uword a=WZ=IX+d;; A=mem.r8(a)|0x10; mem.w8(a,A);} return 23; // SET 4,(IX+d),A
            case 0xe8: { uword a=WZ=IX+d;; B=mem.r8(a)|0x20; mem.w8(a,B);} return 23; // SET 5,(IX+d),B
            case 0xe9: { uword a=WZ=IX+d;; C=mem.r8(a)|0x20; mem.w8(a,C);} return 23; // SET 5,(IX+d),C
            case 0xea: { uword a=WZ=IX+d;; D=mem.r8(a)|0x20; mem.w8(a,D);} return 23; // SET 5,(IX+d),D
            case 0xeb: { uword a=WZ=IX+d;; E=mem.r8(a)|0x20; mem.w8(a,E);} return 23; // SET 5,(IX+d),E
            case 0xec: { uword a=WZ=IX+d;; H=mem.r8(a)|0x20; mem.w8(a,IXH);} return 23; // SET 5,(IX+d),H
            case 0xed: { uword a=WZ=IX+d;; L=mem.r8(a)|0x20; mem.w8(a,IXL);} return 23; // SET 5,(IX+d),L
            case 0xee: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x20);} return 23; // SET 5,(IX+d)
            case 0xef: { uword a=WZ=IX+d;; A=mem.r8(a)|0x20; mem.w8(a,A);} return 23; // SET 5,(IX+d),A
            case 0xf0: { uword a=WZ=IX+d;; B=mem.r8(a)|0x40; mem.w8(a,B);} return 23; // SET 6,(IX+d),B
            case 0xf1: { uword a=WZ=IX+d;; C=mem.r8(a)|0x40; mem.w8(a,C);} return 23; // SET 6,(IX+d),C
            case 0xf2: { uword a=WZ=IX+d;; D=mem.r8(a)|0x40; mem.w8(a,D);} return 23; // SET 6,(IX+d),D
            case 0xf3: { uword a=WZ=IX+d;; E=mem.r8(a)|0x40; mem.w8(a,E);} return 23; // SET 6,(IX+d),E
            case 0xf4: { uword a=WZ=IX+d;; H=mem.r8(a)|0x40; mem.w8(a,IXH);} return 23; // SET 6,(IX+d),H
            case 0xf5: { uword a=WZ=IX+d;; L=mem.r8(a)|0x40; mem.w8(a,IXL);} return 23; // SET 6,(IX+d),L
            case 0xf6: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x40);} return 23; // SET 6,(IX+d)
            case 0xf7: { uword a=WZ=IX+d;; A=mem.r8(a)|0x40; mem.w8(a,A);} return 23; // SET 6,(IX+d),A
            case 0xf8: { uword a=WZ=IX+d;; B=mem.r8(a)|0x80; mem.w8(a,B);} return 23; // SET 7,(IX+d),B
            case 0xf9: { uword a=WZ=IX+d;; C=mem.r8(a)|0x80; mem.w8(a,C);} return 23; // SET 7,(IX+d),C
            case 0xfa: { uword a=WZ=IX+d;; D=mem.r8(a)|0x80; mem.w8(a,D);} return 23; // SET 7,(IX+d),D
            case 0xfb: { uword a=WZ=IX+d;; E=mem.r8(a)|0x80; mem.w8(a,E);} return 23; // SET 7,(IX+d),E
            case 0xfc: { uword a=WZ=IX+d;; H=mem.r8(a)|0x80; mem.w8(a,IXH);} return 23; // SET 7,(IX+d),H
            case 0xfd: { uword a=WZ=IX+d;; L=mem.r8(a)|0x80; mem.w8(a,IXL);} return 23; // SET 7,(IX+d),L
            case 0xfe: { uword a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x80);} return 23; // SET 7,(IX+d)
            case 0xff: { uword a=WZ=IX+d;; A=mem.r8(a)|0x80; mem.w8(a,A);} return 23; // SET 7,(IX+d),A
            default: return invalid_opcode(4);
          }
          break;
          }
        case 0xcc: WZ=mem.r16(PC); PC+=2; if ((F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL Z,nn
        case 0xcd: SP-=2; mem.w16(SP,PC+2); WZ=PC=mem.r16(PC); return 21; // CALL nn
        case 0xce: adc8(mem.r8(PC++)); return 11; // ADC n
        case 0xcf: rst(0x8); return 15; // RST 0x8
        case 0xd0: if (!(F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET NC
        case 0xd1: DE=mem.r16(SP); SP+=2; return 14; // POP DE
        case 0xd2: WZ=mem.r16(PC); if (!(F&CF)) { PC=WZ; } else { PC+=2; }; return 14; // JP NC,nn
        case 0xd3: out(bus, (A<<8)|mem.r8(PC++),A); return 15; // OUT (n),A
        case 0xd4: WZ=mem.r16(PC); PC+=2; if (!(F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL NC,nn
        case 0xd5: SP-=2; mem.w16(SP,DE); return 15; // PUSH DE
        case 0xd6: sub8(mem.r8(PC++)); return 11; // SUB n
        case 0xd7: rst(0x10); return 15; // RST 0x10
        case 0xd8: if ((F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET C
        case 0xd9: swap16(BC,BC_); swap16(DE,DE_); swap16(HL,HL_); swap16(WZ,WZ_); return 8; // EXX
        case 0xda: WZ=mem.r16(PC); if ((F&CF)) { PC=WZ; } else { PC+=2; }; return 14; // JP C,nn
        case 0xdb: A=in(bus, (A<<8)|mem.r8(PC++)); return 15; // IN A,(n)
        case 0xdc: WZ=mem.r16(PC); PC+=2; if ((F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL C,nn
        case 0xde: sbc8(mem.r8(PC++)); return 11; // SBC n
        case 0xdf: rst(0x18); return 15; // RST 0x18
        case 0xe0: if (!(F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET PO
        case 0xe1: IX=mem.r16(SP); SP+=2; return 14; // POP IX
        case 0xe2: WZ=mem.r16(PC); if (!(F&PF)) { PC=WZ; } else { PC+=2; }; return 14; // JP PO,nn
        case 0xe3: {uword swp=mem.r16(SP); mem.w16(SP,IX); IX=WZ=swp;} return 23; // EX (SP),IX
        case 0xe4: WZ=mem.r16(PC); PC+=2; if (!(F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL PO,nn
        case 0xe5: SP-=2; mem.w16(SP,IX); return 15; // PUSH IX
        case 0xe6: and8(mem.r8(PC++)); return 11; // AND n
        case 0xe7: rst(0x20); return 15; // RST 0x20
        case 0xe8: if ((F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET PE
        case 0xe9: PC=IX; return 8; // JP IX
        case 0xea: WZ=mem.r16(PC); if ((F&PF)) { PC=WZ; } else { PC+=2; }; return 14; // JP PE,nn
        case 0xeb: swap16(DE,HL); return 8; // EX DE,HL
        case 0xec: WZ=mem.r16(PC); PC+=2; if ((F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL PE,nn
        case 0xee: xor8(mem.r8(PC++)); return 11; // XOR n
        case 0xef: rst(0x28); return 15; // RST 0x28
        case 0xf0: if (!(F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET P
        case 0xf1: AF=mem.r16(SP); SP+=2; return 14; // POP AF
        case 0xf2: WZ=mem.r16(PC); if (!(F&SF)) { PC=WZ; } else { PC+=2; }; return 14; // JP P,nn
        case 0xf3: di(); return 8; // DI
        case 0xf4: WZ=mem.r16(PC); PC+=2; if (!(F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL P,nn
        case 0xf5: SP-=2; mem.w16(SP,AF); return 15; // PUSH AF
        case 0xf6: or8(mem.r8(PC++)); return 11; // OR n
        case 0xf7: rst(0x30); return 15; // RST 0x30
        case 0xf8: if ((F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET M
        case 0xf9: SP=IX; return 10; // LD SP,IX
        case 0xfa: WZ=mem.r16(PC); if ((F&SF)) { PC=WZ; } else { PC+=2; }; return 14; // JP M,nn
        case 0xfb: ei(); return 8; // EI
        case 0xfc: WZ=mem.r16(PC); PC+=2; if ((F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL M,nn
        case 0xfe: cp8(mem.r8(PC++)); return 11; // CP n
        case 0xff: rst(0x38); return 15; // RST 0x38
        default: return invalid_opcode(2);
      }
      break;
    case 0xde: sbc8(mem.r8(PC++)); return 7; // SBC n
    case 0xdf: rst(0x18); return 11; // RST 0x18
    case 0xe0: if (!(F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return 11; } else return 5; // RET PO
    case 0xe1: HL=mem.r16(SP); SP+=2; return 10; // POP HL
    case 0xe2: WZ=mem.r16(PC); if (!(F&PF)) { PC=WZ; } else { PC+=2; }; return 10; // JP PO,nn
    case 0xe3: {uword swp=mem.r16(SP); mem.w16(SP,HL); HL=WZ=swp;} return 19; // EX (SP),HL
    case 0xe4: WZ=mem.r16(PC); PC+=2; if (!(F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 17; } else { return 10; } // CALL PO,nn
    case 0xe5: SP-=2; mem.w16(SP,HL); return 11; // PUSH HL
    case 0xe6: and8(mem.r8(PC++)); return 7; // AND n
    case 0xe7: rst(0x20); return 11; // RST 0x20
    case 0xe8: if ((F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return 11; } else return 5; // RET PE
    case 0xe9: PC=HL; return 4; // JP HL
    case 0xea: WZ=mem.r16(PC); if ((F&PF)) { PC=WZ; } else { PC+=2; }; return 10; // JP PE,nn
    case 0xeb: swap16(DE,HL); return 4; // EX DE,HL
    case 0xec: WZ=mem.r16(PC); PC+=2; if ((F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 17; } else { return 10; } // CALL PE,nn
    case 0xed:
      switch (fetch_op()) {
        case 0x40: B=in(bus, BC); F=szp[B]|(F&CF); return 12; // IN B,(C)
        case 0x41: out(bus, BC,B); return 12; // OUT (C),B
        case 0x42: HL=sbc16(HL,BC); return 15; // SBC HL,BC
        case 0x43: WZ=mem.r16(PC); mem.w16(WZ++,BC); PC+=2; return 20; // LD (nn),BC
        case 0x44: neg8(); return 8; // NEG
        case 0x46: IM=0; return 8; // IM 0
        case 0x47: I=A; return 9; // LD I,A
        case 0x48: C=in(bus, BC); F=szp[C]|(F&CF); return 12; // IN C,(C)
        case 0x49: out(bus, BC,C); return 12; // OUT (C),C
        case 0x4a: HL=adc16(HL,BC); return 15; // ADC HL,BC
        case 0x4b: WZ=mem.r16(PC); BC=mem.r16(WZ++); PC+=2; return 20; // LD BC,(nn)
        case 0x4c: neg8(); return 8; // NEG
        case 0x4d: reti(); return 15; // RETI
        case 0x4e: IM=0; return 8; // IM 0
        case 0x4f: R=A; return 9; // LD R,A
        case 0x50: D=in(bus, BC); F=szp[D]|(F&CF); return 12; // IN D,(C)
        case 0x51: out(bus, BC,D); return 12; // OUT (C),D
        case 0x52: HL=sbc16(HL,DE); return 15; // SBC HL,DE
        case 0x53: WZ=mem.r16(PC); mem.w16(WZ++,DE); PC+=2; return 20; // LD (nn),DE
        case 0x54: neg8(); return 8; // NEG
        case 0x56: IM=1; return 8; // IM 1
        case 0x57: A=I; F=sziff2(I,IFF2)|(F&CF); return 9; // LD A,I
        case 0x58: E=in(bus, BC); F=szp[E]|(F&CF); return 12; // IN E,(C)
        case 0x59: out(bus, BC,E); return 12; // OUT (C),E
        case 0x5a: HL=adc16(HL,DE); return 15; // ADC HL,DE
        case 0x5b: WZ=mem.r16(PC); DE=mem.r16(WZ++); PC+=2; return 20; // LD DE,(nn)
        case 0x5c: neg8(); return 8; // NEG
        case 0x5e: IM=2; return 8; // IM 2
        case 0x5f: A=R; F=sziff2(R,IFF2)|(F&CF); return 9; // LD A,R
        case 0x60: H=in(bus, BC); F=szp[H]|(F&CF); return 12; // IN H,(C)
        case 0x61: out(bus, BC,H); return 12; // OUT (C),H
        case 0x62: HL=sbc16(HL,HL); return 15; // SBC HL,HL
        case 0x63: WZ=mem.r16(PC); mem.w16(WZ++,HL); PC+=2; return 20; // LD (nn),HL
        case 0x64: neg8(); return 8; // NEG
        case 0x66: IM=0; return 8; // IM 0
        case 0x67: rrd(); return 18; // RRD
        case 0x68: L=in(bus, BC); F=szp[L]|(F&CF); return 12; // IN L,(C)
        case 0x69: out(bus, BC,L); return 12; // OUT (C),L
        case 0x6a: HL=adc16(HL,HL); return 15; // ADC HL,HL
        case 0x6b: WZ=mem.r16(PC); HL=mem.r16(WZ++); PC+=2; return 20; // LD HL,(nn)
        case 0x6c: neg8(); return 8; // NEG
        case 0x6e: IM=0; return 8; // IM 0
        case 0x6f: rld(); return 18; // RLD
        case 0x70: F=szp[in(bus, BC)]|(F&CF); return 12; // IN (C)
        case 0x71: out(bus, BC,0); return 12; // None
        case 0x72: HL=sbc16(HL,SP); return 15; // SBC HL,SP
        case 0x73: WZ=mem.r16(PC); mem.w16(WZ++,SP); PC+=2; return 20; // LD (nn),SP
        case 0x74: neg8(); return 8; // NEG
        case 0x76: IM=1; return 8; // IM 1
        case 0x77: return 9; // NOP (ED)
        case 0x78: A=in(bus, BC); F=szp[A]|(F&CF); return 12; // IN A,(C)
        case 0x79: out(bus, BC,A); return 12; // OUT (C),A
        case 0x7a: HL=adc16(HL,SP); return 15; // ADC HL,SP
        case 0x7b: WZ=mem.r16(PC); SP=mem.r16(WZ++); PC+=2; return 20; // LD SP,(nn)
        case 0x7c: neg8(); return 8; // NEG
        case 0x7e: IM=2; return 8; // IM 2
        case 0x7f: return 9; // NOP (ED)
        case 0xa0: ldi(); return 16; // LDI
        case 0xa1: cpi(); return 16; // CPI
        case 0xa2: ini(bus); return 16; // INI
        case 0xa3: outi(bus); return 16; // OUTI
        case 0xa8: ldd(); return 16; // LDD
        case 0xa9: cpd(); return 16; // CPD
        case 0xaa: ind(bus); return 16; // IND
        case 0xab: outd(bus); return 16; // OUTD
        case 0xb0: return ldir(); // LDIR
        case 0xb1: return cpir(); // CPIR
        case 0xb2: return inir(bus); // INIR
        case 0xb3: return otir(bus); // OTID
        case 0xb8: return lddr(); // LDDR
        case 0xb9: return cpdr(); // CPDR
        case 0xba: return indr(bus); // INDR
        case 0xbb: return otdr(bus); // OTDR
        default: return invalid_opcode(2);
      }
      break;
    case 0xee: xor8(mem.r8(PC++)); return 7; // XOR n
    case 0xef: rst(0x28); return 11; // RST 0x28
    case 0xf0: if (!(F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return 11; } else return 5; // RET P
    case 0xf1: AF=mem.r16(SP); SP+=2; return 10; // POP AF
    case 0xf2: WZ=mem.r16(PC); if (!(F&SF)) { PC=WZ; } else { PC+=2; }; return 10; // JP P,nn
    case 0xf3: di(); return 4; // DI
    case 0xf4: WZ=mem.r16(PC); PC+=2; if (!(F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 17; } else { return 10; } // CALL P,nn
    case 0xf5: SP-=2; mem.w16(SP,AF); return 11; // PUSH AF
    case 0xf6: or8(mem.r8(PC++)); return 7; // OR n
    case 0xf7: rst(0x30); return 11; // RST 0x30
    case 0xf8: if ((F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return 11; } else return 5; // RET M
    case 0xf9: SP=HL; return 6; // LD SP,HL
    case 0xfa: WZ=mem.r16(PC); if ((F&SF)) { PC=WZ; } else { PC+=2; }; return 10; // JP M,nn
    case 0xfb: ei(); return 4; // EI
    case 0xfc: WZ=mem.r16(PC); PC+=2; if ((F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 17; } else { return 10; } // CALL M,nn
    case 0xfd:
      switch (fetch_op()) {
        case 0x0: return 8; // NOP
        case 0x1: BC=mem.r16(PC); PC+=2; return 14; // LD BC,nn
        case 0x2: mem.w8(BC,A); Z=C+1; W=A; return 11; // LD (BC),A
        case 0x3: BC++; return 10; // INC BC
        case 0x4: B=inc8(B); return 8; // INC B
        case 0x5: B=dec8(B); return 8; // DEC B
        case 0x6: B=mem.r8(PC++); return 11; // LD B,n
        case 0x7: rlca8(); return 8; // RLCA
        case 0x8: swap16(AF,AF_); return 8; // EX AF,AF'
        case 0x9: IY=add16(IY,BC); return 15; // ADD IY,BC
        case 0xa: A=mem.r8(BC); WZ=BC+1; return 11; // LD A,(BC)
        case 0xb: BC--; return 10; // DEC BC
        case 0xc: C=inc8(C); return 8; // INC C
        case 0xd: C=dec8(C); return 8; // DEC C
        case 0xe: C=mem.r8(PC++); return 11; // LD C,n
        case 0xf: rrca8(); return 8; // RRCA
        case 0x10: if (--B>0) { WZ=PC=PC+mem.rs8(PC)+1; return 17; } else { PC++; return 12; } // DJNZ
        case 0x11: DE=mem.r16(PC); PC+=2; return 14; // LD DE,nn
        case 0x12: mem.w8(DE,A); Z=E+1; W=A; return 11; // LD (DE),A
        case 0x13: DE++; return 10; // INC DE
        case 0x14: D=inc8(D); return 8; // INC D
        case 0x15: D=dec8(D); return 8; // DEC D
        case 0x16: D=mem.r8(PC++); return 11; // LD D,n
        case 0x17: rla8(); return 8; // RLA
        case 0x18: WZ=PC=PC+mem.rs8(PC)+1; return 16; // JR d
        case 0x19: IY=add16(IY,DE); return 15; // ADD IY,DE
        case 0x1a: A=mem.r8(DE); WZ=DE+1; return 11; // LD A,(DE)
        case 0x1b: DE--; return 10; // DEC DE
        case 0x1c: E=inc8(E); return 8; // INC E
        case 0x1d: E=dec8(E); return 8; // DEC E
        case 0x1e: E=mem.r8(PC++); return 11; // LD E,n
        case 0x1f: rra8(); return 8; // RRA
        case 0x20: if (!(F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return 16; } else { PC++; return 11; } // JR NZ,d
        case 0x21: IY=mem.r16(PC); PC+=2; return 14; // LD IY,nn
        case 0x22: WZ=mem.r16(PC); mem.w16(WZ++,IY); PC+=2; return 20; // LD (nn),IY
        case 0x23: IY++; return 10; // INC IY
        case 0x24: IYH=inc8(IYH); return 8; // INC IYH
        case 0x25: IYH=dec8(IYH); return 8; // DEC IYH
        case 0x26: IYH=mem.r8(PC++); return 11; // LD IYH,n
        case 0x27: daa(); return 8; // DAA
        case 0x28: if ((F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return 16; } else { PC++; return 11; } // JR Z,d
        case 0x29: IY=add16(IY,IY); return 15; // ADD IY,IY
        case 0x2a: WZ=mem.r16(PC); IY=mem.r16(WZ++); PC+=2; return 20; // LD IY,(nn)
        case 0x2b: IY--; return 10; // DEC IY
        case 0x2c: IYL=inc8(IYL); return 8; // INC IYL
        case 0x2d: IYL=dec8(IYL); return 8; // DEC IYL
        case 0x2e: IYL=mem.r8(PC++); return 11; // LD IYL,n
        case 0x2f: A^=0xFF; F=(F&(SF|ZF|PF|CF))|HF|NF|(A&(YF|XF)); return 8; // CPL
        case 0x30: if (!(F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return 16; } else { PC++; return 11; } // JR NC,d
        case 0x31: SP=mem.r16(PC); PC+=2; return 14; // LD SP,nn
        case 0x32: WZ=mem.r16(PC); mem.w8(WZ++,A); W=A; PC+=2; return 17; // LD (nn),A
        case 0x33: SP++; return 10; // INC SP
        case 0x34: { uword a=WZ=IY+mem.rs8(PC++); mem.w8(a,inc8(mem.r8(a))); } return 23; // INC (IY+d)
        case 0x35: { uword a=WZ=IY+mem.rs8(PC++); mem.w8(a,dec8(mem.r8(a))); } return 23; // DEC (IY+d)
        case 0x36: { uword a=WZ=IY+mem.rs8(PC++); mem.w8(a,mem.r8(PC++)); } return 19; // LD (IY+d),n
        case 0x37: F=(F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF)); return 8; // SCF
        case 0x38: if ((F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return 16; } else { PC++; return 11; } // JR C,d
        case 0x39: IY=add16(IY,SP); return 15; // ADD IY,SP
        case 0x3a: WZ=mem.r16(PC); A=mem.r8(WZ++); PC+=2; return 17; // LD A,(nn)
        case 0x3b: SP--; return 10; // DEC SP
        case 0x3c: A=inc8(A); return 8; // INC A
        case 0x3d: A=dec8(A); return 8; // DEC A
        case 0x3e: A=mem.r8(PC++); return 11; // LD A,n
        case 0x3f: F=((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF; return 8; // CCF
        case 0x40: B=B; return 8; // LD B,B
        case 0x41: B=C; return 8; // LD B,C
        case 0x42: B=D; return 8; // LD B,D
        case 0x43: B=E; return 8; // LD B,E
        case 0x44: B=IYH; return 8; // LD B,IYH
        case 0x45: B=IYL; return 8; // LD B,IYL
        case 0x46: { uword a=WZ=IY+mem.rs8(PC++); B=mem.r8(a); } return 19; // LD B,(IY+d)
        case 0x47: B=A; return 8; // LD B,A
        case 0x48: C=B; return 8; // LD C,B
        case 0x49: C=C; return 8; // LD C,C
        case 0x4a: C=D; return 8; // LD C,D
        case 0x4b: C=E; return 8; // LD C,E
        case 0x4c: C=IYH; return 8; // LD C,IYH
        case 0x4d: C=IYL; return 8; // LD C,IYL
        case 0x4e: { uword a=WZ=IY+mem.rs8(PC++); C=mem.r8(a); } return 19; // LD C,(IY+d)
        case 0x4f: C=A; return 8; // LD C,A
        case 0x50: D=B; return 8; // LD D,B
        case 0x51: D=C; return 8; // LD D,C
        case 0x52: D=D; return 8; // LD D,D
        case 0x53: D=E; return 8; // LD D,E
        case 0x54: D=IYH; return 8; // LD D,IYH
        case 0x55: D=IYL; return 8; // LD D,IYL
        case 0x56: { uword a=WZ=IY+mem.rs8(PC++); D=mem.r8(a); } return 19; // LD D,(IY+d)
        case 0x57: D=A; return 8; // LD D,A
        case 0x58: E=B; return 8; // LD E,B
        case 0x59: E=C; return 8; // LD E,C
        case 0x5a: E=D; return 8; // LD E,D
        case 0x5b: E=E; return 8; // LD E,E
        case 0x5c: E=IYH; return 8; // LD E,IYH
        case 0x5d: E=IYL; return 8; // LD E,IYL
        case 0x5e: { uword a=WZ=IY+mem.rs8(PC++); E=mem.r8(a); } return 19; // LD E,(IY+d)
        case 0x5f: E=A; return 8; // LD E,A
        case 0x60: IYH=B; return 8; // LD IYH,B
        case 0x61: IYH=C; return 8; // LD IYH,C
        case 0x62: IYH=D; return 8; // LD IYH,D
        case 0x63: IYH=E; return 8; // LD IYH,E
        case 0x64: IYH=IYH; return 8; // LD IYH,IYH
        case 0x65: IYH=IYL; return 8; // LD IYH,IYL
        case 0x66: { uword a=WZ=IY+mem.rs8(PC++); H=mem.r8(a); } return 19; // LD H,(IY+d)
        case 0x67: IYH=A; return 8; // LD IYH,A
        case 0x68: IYL=B; return 8; // LD IYL,B
        case 0x69: IYL=C; return 8; // LD IYL,C
        case 0x6a: IYL=D; return 8; // LD IYL,D
        case 0x6b: IYL=E; return 8; // LD IYL,E
        case 0x6c: IYL=IYH; return 8; // LD IYL,IYH
        case 0x6d: IYL=IYL; return 8; // LD IYL,IYL
        case 0x6e: { uword a=WZ=IY+mem.rs8(PC++); L=mem.r8(a); } return 19; // LD L,(IY+d)
        case 0x6f: IYL=A; return 8; // LD IYL,A
        case 0x70: { uword a=WZ=IY+mem.rs8(PC++); mem.w8(a,B); } return 19; // LD (IY+d),B
        case 0x71: { uword a=WZ=IY+mem.rs8(PC++); mem.w8(a,C); } return 19; // LD (IY+d),C
        case 0x72: { uword a=WZ=IY+mem.rs8(PC++); mem.w8(a,D); } return 19; // LD (IY+d),D
        case 0x73: { uword a=WZ=IY+mem.rs8(PC++); mem.w8(a,E); } return 19; // LD (IY+d),E
        case 0x74: { uword a=WZ=IY+mem.rs8(PC++); mem.w8(a,H); } return 19; // LD (IY+d),H
        case 0x75: { uword a=WZ=IY+mem.rs8(PC++); mem.w8(a,L); } return 19; // LD (IY+d),L
        case 0x76: halt(); return 8; // HALT
        case 0x77: { uword a=WZ=IY+mem.rs8(PC++); mem.w8(a,A); } return 19; // LD (IY+d),A
        case 0x78: A=B; return 8; // LD A,B
        case 0x79: A=C; return 8; // LD A,C
        case 0x7a: A=D; return 8; // LD A,D
        case 0x7b: A=E; return 8; // LD A,E
        case 0x7c: A=IYH; return 8; // LD A,IYH
        case 0x7d: A=IYL; return 8; // LD A,IYL
        case 0x7e: { uword a=WZ=IY+mem.rs8(PC++); A=mem.r8(a); } return 19; // LD A,(IY+d)
        case 0x7f: A=A; return 8; // LD A,A
        case 0x80: add8(B); return 8; // ADD B
        case 0x81: add8(C); return 8; // ADD C
        case 0x82: add8(D); return 8; // ADD D
        case 0x83: add8(E); return 8; // ADD E
        case 0x84: add8(IYH); return 8; // ADD IYH
        case 0x85: add8(IYL); return 8; // ADD IYL
        case 0x86: { uword a=WZ=IY+mem.rs8(PC++); add8(mem.r8(a)); } return 19; // ADD (IY+d)
        case 0x87: add8(A); return 8; // ADD A
        case 0x88: adc8(B); return 8; // ADC B
        case 0x89: adc8(C); return 8; // ADC C
        case 0x8a: adc8(D); return 8; // ADC D
        case 0x8b: adc8(E); return 8; // ADC E
        case 0x8c: adc8(IYH); return 8; // ADC IYH
        case 0x8d: adc8(IYL); return 8; // ADC IYL
        case 0x8e: { uword a=WZ=IY+mem.rs8(PC++); adc8(mem.r8(a)); } return 19; // ADC (IY+d)
        case 0x8f: adc8(A); return 8; // ADC A
        case 0x90: sub8(B); return 8; // SUB B
        case 0x91: sub8(C); return 8; // SUB C
        case 0x92: sub8(D); return 8; // SUB D
        case 0x93: sub8(E); return 8; // SUB E
        case 0x94: sub8(IYH); return 8; // SUB IYH
        case 0x95: sub8(IYL); return 8; // SUB IYL
        case 0x96: { uword a=WZ=IY+mem.rs8(PC++); sub8(mem.r8(a)); } return 19; // SUB (IY+d)
        case 0x97: sub8(A); return 8; // SUB A
        case 0x98: sbc8(B); return 8; // SBC B
        case 0x99: sbc8(C); return 8; // SBC C
        case 0x9a: sbc8(D); return 8; // SBC D
        case 0x9b: sbc8(E); return 8; // SBC E
        case 0x9c: sbc8(IYH); return 8; // SBC IYH
        case 0x9d: sbc8(IYL); return 8; // SBC IYL
        case 0x9e: { uword a=WZ=IY+mem.rs8(PC++); sbc8(mem.r8(a)); } return 19; // SBC (IY+d)
        case 0x9f: sbc8(A); return 8; // SBC A
        case 0xa0: and8(B); return 8; // AND B
        case 0xa1: and8(C); return 8; // AND C
        case 0xa2: and8(D); return 8; // AND D
        case 0xa3: and8(E); return 8; // AND E
        case 0xa4: and8(IYH); return 8; // AND IYH
        case 0xa5: and8(IYL); return 8; // AND IYL
        case 0xa6: { uword a=WZ=IY+mem.rs8(PC++); and8(mem.r8(a)); } return 19; // AND (IY+d)
        case 0xa7: and8(A); return 8; // AND A
        case 0xa8: xor8(B); return 8; // XOR B
        case 0xa9: xor8(C); return 8; // XOR C
        case 0xaa: xor8(D); return 8; // XOR D
        case 0xab: xor8(E); return 8; // XOR E
        case 0xac: xor8(IYH); return 8; // XOR IYH
        case 0xad: xor8(IYL); return 8; // XOR IYL
        case 0xae: { uword a=WZ=IY+mem.rs8(PC++); xor8(mem.r8(a)); } return 19; // XOR (IY+d)
        case 0xaf: xor8(A); return 8; // XOR A
        case 0xb0: or8(B); return 8; // OR B
        case 0xb1: or8(C); return 8; // OR C
        case 0xb2: or8(D); return 8; // OR D
        case 0xb3: or8(E); return 8; // OR E
        case 0xb4: or8(IYH); return 8; // OR IYH
        case 0xb5: or8(IYL); return 8; // OR IYL
        case 0xb6: { uword a=WZ=IY+mem.rs8(PC++); or8(mem.r8(a)); } return 19; // OR (IY+d)
        case 0xb7: or8(A); return 8; // OR A
        case 0xb8: cp8(B); return 8; // CP B
        case 0xb9: cp8(C); return 8; // CP C
        case 0xba: cp8(D); return 8; // CP D
        case 0xbb: cp8(E); return 8; // CP E
        case 0xbc: cp8(IYH); return 8; // CP IYH
        case 0xbd: cp8(IYL); return 8; // CP IYL
        case 0xbe: { uword a=WZ=IY+mem.rs8(PC++); cp8(mem.r8(a)); } return 19; // CP (IY+d)
        case 0xbf: cp8(A); return 8; // CP A
        case 0xc0: if (!(F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET NZ
        case 0xc1: BC=mem.r16(SP); SP+=2; return 14; // POP BC
        case 0xc2: WZ=mem.r16(PC); if (!(F&ZF)) { PC=WZ; } else { PC+=2; }; return 14; // JP NZ,nn
        case 0xc3: WZ=PC=mem.r16(PC); return 14; // JP nn
        case 0xc4: WZ=mem.r16(PC); PC+=2; if (!(F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL NZ,nn
        case 0xc5: SP-=2; mem.w16(SP,BC); return 15; // PUSH BC
        case 0xc6: add8(mem.r8(PC++)); return 11; // ADD n
        case 0xc7: rst(0x0); return 15; // RST 0x0
        case 0xc8: if ((F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET Z
        case 0xc9: WZ=PC=mem.r16(SP); SP+=2; return 14; // RET
        case 0xca: WZ=mem.r16(PC); if ((F&ZF)) { PC=WZ; } else { PC+=2; }; return 14; // JP Z,nn
        case 0xcb:
          { const int d = mem.rs8(PC++);
          switch (fetch_op()) {
            case 0x0: { uword a=WZ=IY+d;; B=rlc8(mem.r8(a)); mem.w8(a,B); } return 23; // RLC (IY+d),B
            case 0x1: { uword a=WZ=IY+d;; C=rlc8(mem.r8(a)); mem.w8(a,C); } return 23; // RLC (IY+d),C
            case 0x2: { uword a=WZ=IY+d;; D=rlc8(mem.r8(a)); mem.w8(a,D); } return 23; // RLC (IY+d),D
            case 0x3: { uword a=WZ=IY+d;; E=rlc8(mem.r8(a)); mem.w8(a,E); } return 23; // RLC (IY+d),E
            case 0x4: { uword a=WZ=IY+d;; H=rlc8(mem.r8(a)); mem.w8(a,H); } return 23; // RLC (IY+d),H
            case 0x5: { uword a=WZ=IY+d;; L=rlc8(mem.r8(a)); mem.w8(a,L); } return 23; // RLC (IY+d),L
            case 0x6: { uword a=WZ=IY+d;; mem.w8(a,rlc8(mem.r8(a))); } return 23; // RLC (IY+d)
            case 0x7: { uword a=WZ=IY+d;; A=rlc8(mem.r8(a)); mem.w8(a,A); } return 23; // RLC (IY+d),A
            case 0x8: { uword a=WZ=IY+d;; B=rrc8(mem.r8(a)); mem.w8(a,B); } return 23; // RRC (IY+d),B
            case 0x9: { uword a=WZ=IY+d;; C=rrc8(mem.r8(a)); mem.w8(a,C); } return 23; // RRC (IY+d),C
            case 0xa: { uword a=WZ=IY+d;; D=rrc8(mem.r8(a)); mem.w8(a,D); } return 23; // RRC (IY+d),D
            case 0xb: { uword a=WZ=IY+d;; E=rrc8(mem.r8(a)); mem.w8(a,E); } return 23; // RRC (IY+d),E
            case 0xc: { uword a=WZ=IY+d;; H=rrc8(mem.r8(a)); mem.w8(a,H); } return 23; // RRC (IY+d),H
            case 0xd: { uword a=WZ=IY+d;; L=rrc8(mem.r8(a)); mem.w8(a,L); } return 23; // RRC (IY+d),L
            case 0xe: { uword a=WZ=IY+d;; mem.w8(a,rrc8(mem.r8(a))); } return 23; // RRC (IY+d)
            case 0xf: { uword a=WZ=IY+d;; A=rrc8(mem.r8(a)); mem.w8(a,A); } return 23; // RRC (IY+d),A
            case 0x10: { uword a=WZ=IY+d;; B=rl8(mem.r8(a)); mem.w8(a,B); } return 23; // RL (IY+d),B
            case 0x11: { uword a=WZ=IY+d;; C=rl8(mem.r8(a)); mem.w8(a,C); } return 23; // RL (IY+d),C
            case 0x12: { uword a=WZ=IY+d;; D=rl8(mem.r8(a)); mem.w8(a,D); } return 23; // RL (IY+d),D
            case 0x13: { uword a=WZ=IY+d;; E=rl8(mem.r8(a)); mem.w8(a,E); } return 23; // RL (IY+d),E
            case 0x14: { uword a=WZ=IY+d;; H=rl8(mem.r8(a)); mem.w8(a,H); } return 23; // RL (IY+d),H
            case 0x15: { uword a=WZ=IY+d;; L=rl8(mem.r8(a)); mem.w8(a,L); } return 23; // RL (IY+d),L
            case 0x16: { uword a=WZ=IY+d;; mem.w8(a,rl8(mem.r8(a))); } return 23; // RL (IY+d)
            case 0x17: { uword a=WZ=IY+d;; A=rl8(mem.r8(a)); mem.w8(a,A); } return 23; // RL (IY+d),A
            case 0x18: { uword a=WZ=IY+d;; B=rr8(mem.r8(a)); mem.w8(a,B); } return 23; // RR (IY+d),B
            case 0x19: { uword a=WZ=IY+d;; C=rr8(mem.r8(a)); mem.w8(a,C); } return 23; // RR (IY+d),C
            case 0x1a: { uword a=WZ=IY+d;; D=rr8(mem.r8(a)); mem.w8(a,D); } return 23; // RR (IY+d),D
            case 0x1b: { uword a=WZ=IY+d;; E=rr8(mem.r8(a)); mem.w8(a,E); } return 23; // RR (IY+d),E
            case 0x1c: { uword a=WZ=IY+d;; H=rr8(mem.r8(a)); mem.w8(a,H); } return 23; // RR (IY+d),H
            case 0x1d: { uword a=WZ=IY+d;; L=rr8(mem.r8(a)); mem.w8(a,L); } return 23; // RR (IY+d),L
            case 0x1e: { uword a=WZ=IY+d;; mem.w8(a,rr8(mem.r8(a))); } return 23; // RR (IY+d)
            case 0x1f: { uword a=WZ=IY+d;; A=rr8(mem.r8(a)); mem.w8(a,A); } return 23; // RR (IY+d),A
            case 0x20: { uword a=WZ=IY+d;; B=sla8(mem.r8(a)); mem.w8(a,B); } return 23; // SLA (IY+d),B
            case 0x21: { uword a=WZ=IY+d;; C=sla8(mem.r8(a)); mem.w8(a,C); } return 23; // SLA (IY+d),C
            case 0x22: { uword a=WZ=IY+d;; D=sla8(mem.r8(a)); mem.w8(a,D); } return 23; // SLA (IY+d),D
            case 0x23: { uword a=WZ=IY+d;; E=sla8(mem.r8(a)); mem.w8(a,E); } return 23; // SLA (IY+d),E
            case 0x24: { uword a=WZ=IY+d;; H=sla8(mem.r8(a)); mem.w8(a,H); } return 23; // SLA (IY+d),H
            case 0x25: { uword a=WZ=IY+d;; L=sla8(mem.r8(a)); mem.w8(a,L); } return 23; // SLA (IY+d),L
            case 0x26: { uword a=WZ=IY+d;; mem.w8(a,sla8(mem.r8(a))); } return 23; // SLA (IY+d)
            case 0x27: { uword a=WZ=IY+d;; A=sla8(mem.r8(a)); mem.w8(a,A); } return 23; // SLA (IY+d),A
            case 0x28: { uword a=WZ=IY+d;; B=sra8(mem.r8(a)); mem.w8(a,B); } return 23; // SRA (IY+d),B
            case 0x29: { uword a=WZ=IY+d;; C=sra8(mem.r8(a)); mem.w8(a,C); } return 23; // SRA (IY+d),C
            case 0x2a: { uword a=WZ=IY+d;; D=sra8(mem.r8(a)); mem.w8(a,D); } return 23; // SRA (IY+d),D
            case 0x2b: { uword a=WZ=IY+d;; E=sra8(mem.r8(a)); mem.w8(a,E); } return 23; // SRA (IY+d),E
            case 0x2c: { uword a=WZ=IY+d;; H=sra8(mem.r8(a)); mem.w8(a,H); } return 23; // SRA (IY+d),H
            case 0x2d: { uword a=WZ=IY+d;; L=sra8(mem.r8(a)); mem.w8(a,L); } return 23; // SRA (IY+d),L
            case 0x2e: { uword a=WZ=IY+d;; mem.w8(a,sra8(mem.r8(a))); } return 23; // SRA (IY+d)
            case 0x2f: { uword a=WZ=IY+d;; A=sra8(mem.r8(a)); mem.w8(a,A); } return 23; // SRA (IY+d),A
            case 0x30: { uword a=WZ=IY+d;; B=sll8(mem.r8(a)); mem.w8(a,B); } return 23; // SLL (IY+d),B
            case 0x31: { uword a=WZ=IY+d;; C=sll8(mem.r8(a)); mem.w8(a,C); } return 23; // SLL (IY+d),C
            case 0x32: { uword a=WZ=IY+d;; D=sll8(mem.r8(a)); mem.w8(a,D); } return 23; // SLL (IY+d),D
            case 0x33: { uword a=WZ=IY+d;; E=sll8(mem.r8(a)); mem.w8(a,E); } return 23; // SLL (IY+d),E
            case 0x34: { uword a=WZ=IY+d;; H=sll8(mem.r8(a)); mem.w8(a,H); } return 23; // SLL (IY+d),H
            case 0x35: { uword a=WZ=IY+d;; L=sll8(mem.r8(a)); mem.w8(a,L); } return 23; // SLL (IY+d),L
            case 0x36: { uword a=WZ=IY+d;; mem.w8(a,sll8(mem.r8(a))); } return 23; // SLL (IY+d)
            case 0x37: { uword a=WZ=IY+d;; A=sll8(mem.r8(a)); mem.w8(a,A); } return 23; // SLL (IY+d),A
            case 0x38: { uword a=WZ=IY+d;; B=srl8(mem.r8(a)); mem.w8(a,B); } return 23; // SRL (IY+d),B
            case 0x39: { uword a=WZ=IY+d;; C=srl8(mem.r8(a)); mem.w8(a,C); } return 23; // SRL (IY+d),C
            case 0x3a: { uword a=WZ=IY+d;; D=srl8(mem.r8(a)); mem.w8(a,D); } return 23; // SRL (IY+d),D
            case 0x3b: { uword a=WZ=IY+d;; E=srl8(mem.r8(a)); mem.w8(a,E); } return 23; // SRL (IY+d),E
            case 0x3c: { uword a=WZ=IY+d;; H=srl8(mem.r8(a)); mem.w8(a,H); } return 23; // SRL (IY+d),H
            case 0x3d: { uword a=WZ=IY+d;; L=srl8(mem.r8(a)); mem.w8(a,L); } return 23; // SRL (IY+d),L
            case 0x3e: { uword a=WZ=IY+d;; mem.w8(a,srl8(mem.r8(a))); } return 23; // SRL (IY+d)
            case 0x3f: { uword a=WZ=IY+d;; A=srl8(mem.r8(a)); mem.w8(a,A); } return 23; // SRL (IY+d),A
            case 0x40: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IY+d)
            case 0x41: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IY+d)
            case 0x42: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IY+d)
            case 0x43: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IY+d)
            case 0x44: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IY+d)
            case 0x45: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IY+d)
            case 0x46: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IY+d)
            case 0x47: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return 20; // BIT 0,(IY+d)
            case 0x48: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IY+d)
            case 0x49: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IY+d)
            case 0x4a: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IY+d)
            case 0x4b: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IY+d)
            case 0x4c: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IY+d)
            case 0x4d: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IY+d)
            case 0x4e: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IY+d)
            case 0x4f: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return 20; // BIT 1,(IY+d)
            case 0x50: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IY+d)
            case 0x51: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IY+d)
            case 0x52: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IY+d)
            case 0x53: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IY+d)
            case 0x54: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IY+d)
            case 0x55: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IY+d)
            case 0x56: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IY+d)
            case 0x57: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return 20; // BIT 2,(IY+d)
            case 0x58: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IY+d)
            case 0x59: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IY+d)
            case 0x5a: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IY+d)
            case 0x5b: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IY+d)
            case 0x5c: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IY+d)
            case 0x5d: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IY+d)
            case 0x5e: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IY+d)
            case 0x5f: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return 20; // BIT 3,(IY+d)
            case 0x60: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IY+d)
            case 0x61: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IY+d)
            case 0x62: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IY+d)
            case 0x63: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IY+d)
            case 0x64: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IY+d)
            case 0x65: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IY+d)
            case 0x66: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IY+d)
            case 0x67: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return 20; // BIT 4,(IY+d)
            case 0x68: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IY+d)
            case 0x69: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IY+d)
            case 0x6a: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IY+d)
            case 0x6b: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IY+d)
            case 0x6c: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IY+d)
            case 0x6d: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IY+d)
            case 0x6e: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IY+d)
            case 0x6f: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return 20; // BIT 5,(IY+d)
            case 0x70: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IY+d)
            case 0x71: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IY+d)
            case 0x72: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IY+d)
            case 0x73: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IY+d)
            case 0x74: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IY+d)
            case 0x75: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IY+d)
            case 0x76: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IY+d)
            case 0x77: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return 20; // BIT 6,(IY+d)
            case 0x78: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IY+d)
            case 0x79: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IY+d)
            case 0x7a: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IY+d)
            case 0x7b: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IY+d)
            case 0x7c: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IY+d)
            case 0x7d: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IY+d)
            case 0x7e: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IY+d)
            case 0x7f: { uword a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return 20; // BIT 7,(IY+d)
            case 0x80: { uword a=WZ=IY+d;; B=mem.r8(a)&~0x1; mem.w8(a,B); } return 23; // RES 0,(IY+d),B
            case 0x81: { uword a=WZ=IY+d;; C=mem.r8(a)&~0x1; mem.w8(a,C); } return 23; // RES 0,(IY+d),C
            case 0x82: { uword a=WZ=IY+d;; D=mem.r8(a)&~0x1; mem.w8(a,D); } return 23; // RES 0,(IY+d),D
            case 0x83: { uword a=WZ=IY+d;; E=mem.r8(a)&~0x1; mem.w8(a,E); } return 23; // RES 0,(IY+d),E
            case 0x84: { uword a=WZ=IY+d;; H=mem.r8(a)&~0x1; mem.w8(a,H); } return 23; // RES 0,(IY+d),H
            case 0x85: { uword a=WZ=IY+d;; L=mem.r8(a)&~0x1; mem.w8(a,L); } return 23; // RES 0,(IY+d),L
            case 0x86: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x1); } return 23; // RES 0,(IY+d)
            case 0x87: { uword a=WZ=IY+d;; A=mem.r8(a)&~0x1; mem.w8(a,A); } return 23; // RES 0,(IY+d),A
            case 0x88: { uword a=WZ=IY+d;; B=mem.r8(a)&~0x2; mem.w8(a,B); } return 23; // RES 1,(IY+d),B
            case 0x89: { uword a=WZ=IY+d;; C=mem.r8(a)&~0x2; mem.w8(a,C); } return 23; // RES 1,(IY+d),C
            case 0x8a: { uword a=WZ=IY+d;; D=mem.r8(a)&~0x2; mem.w8(a,D); } return 23; // RES 1,(IY+d),D
            case 0x8b: { uword a=WZ=IY+d;; E=mem.r8(a)&~0x2; mem.w8(a,E); } return 23; // RES 1,(IY+d),E
            case 0x8c: { uword a=WZ=IY+d;; H=mem.r8(a)&~0x2; mem.w8(a,H); } return 23; // RES 1,(IY+d),H
            case 0x8d: { uword a=WZ=IY+d;; L=mem.r8(a)&~0x2; mem.w8(a,L); } return 23; // RES 1,(IY+d),L
            case 0x8e: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x2); } return 23; // RES 1,(IY+d)
            case 0x8f: { uword a=WZ=IY+d;; A=mem.r8(a)&~0x2; mem.w8(a,A); } return 23; // RES 1,(IY+d),A
            case 0x90: { uword a=WZ=IY+d;; B=mem.r8(a)&~0x4; mem.w8(a,B); } return 23; // RES 2,(IY+d),B
            case 0x91: { uword a=WZ=IY+d;; C=mem.r8(a)&~0x4; mem.w8(a,C); } return 23; // RES 2,(IY+d),C
            case 0x92: { uword a=WZ=IY+d;; D=mem.r8(a)&~0x4; mem.w8(a,D); } return 23; // RES 2,(IY+d),D
            case 0x93: { uword a=WZ=IY+d;; E=mem.r8(a)&~0x4; mem.w8(a,E); } return 23; // RES 2,(IY+d),E
            case 0x94: { uword a=WZ=IY+d;; H=mem.r8(a)&~0x4; mem.w8(a,H); } return 23; // RES 2,(IY+d),H
            case 0x95: { uword a=WZ=IY+d;; L=mem.r8(a)&~0x4; mem.w8(a,L); } return 23; // RES 2,(IY+d),L
            case 0x96: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x4); } return 23; // RES 2,(IY+d)
            case 0x97: { uword a=WZ=IY+d;; A=mem.r8(a)&~0x4; mem.w8(a,A); } return 23; // RES 2,(IY+d),A
            case 0x98: { uword a=WZ=IY+d;; B=mem.r8(a)&~0x8; mem.w8(a,B); } return 23; // RES 3,(IY+d),B
            case 0x99: { uword a=WZ=IY+d;; C=mem.r8(a)&~0x8; mem.w8(a,C); } return 23; // RES 3,(IY+d),C
            case 0x9a: { uword a=WZ=IY+d;; D=mem.r8(a)&~0x8; mem.w8(a,D); } return 23; // RES 3,(IY+d),D
            case 0x9b: { uword a=WZ=IY+d;; E=mem.r8(a)&~0x8; mem.w8(a,E); } return 23; // RES 3,(IY+d),E
            case 0x9c: { uword a=WZ=IY+d;; H=mem.r8(a)&~0x8; mem.w8(a,H); } return 23; // RES 3,(IY+d),H
            case 0x9d: { uword a=WZ=IY+d;; L=mem.r8(a)&~0x8; mem.w8(a,L); } return 23; // RES 3,(IY+d),L
            case 0x9e: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x8); } return 23; // RES 3,(IY+d)
            case 0x9f: { uword a=WZ=IY+d;; A=mem.r8(a)&~0x8; mem.w8(a,A); } return 23; // RES 3,(IY+d),A
            case 0xa0: { uword a=WZ=IY+d;; B=mem.r8(a)&~0x10; mem.w8(a,B); } return 23; // RES 4,(IY+d),B
            case 0xa1: { uword a=WZ=IY+d;; C=mem.r8(a)&~0x10; mem.w8(a,C); } return 23; // RES 4,(IY+d),C
            case 0xa2: { uword a=WZ=IY+d;; D=mem.r8(a)&~0x10; mem.w8(a,D); } return 23; // RES 4,(IY+d),D
            case 0xa3: { uword a=WZ=IY+d;; E=mem.r8(a)&~0x10; mem.w8(a,E); } return 23; // RES 4,(IY+d),E
            case 0xa4: { uword a=WZ=IY+d;; H=mem.r8(a)&~0x10; mem.w8(a,H); } return 23; // RES 4,(IY+d),H
            case 0xa5: { uword a=WZ=IY+d;; L=mem.r8(a)&~0x10; mem.w8(a,L); } return 23; // RES 4,(IY+d),L
            case 0xa6: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x10); } return 23; // RES 4,(IY+d)
            case 0xa7: { uword a=WZ=IY+d;; A=mem.r8(a)&~0x10; mem.w8(a,A); } return 23; // RES 4,(IY+d),A
            case 0xa8: { uword a=WZ=IY+d;; B=mem.r8(a)&~0x20; mem.w8(a,B); } return 23; // RES 5,(IY+d),B
            case 0xa9: { uword a=WZ=IY+d;; C=mem.r8(a)&~0x20; mem.w8(a,C); } return 23; // RES 5,(IY+d),C
            case 0xaa: { uword a=WZ=IY+d;; D=mem.r8(a)&~0x20; mem.w8(a,D); } return 23; // RES 5,(IY+d),D
            case 0xab: { uword a=WZ=IY+d;; E=mem.r8(a)&~0x20; mem.w8(a,E); } return 23; // RES 5,(IY+d),E
            case 0xac: { uword a=WZ=IY+d;; H=mem.r8(a)&~0x20; mem.w8(a,H); } return 23; // RES 5,(IY+d),H
            case 0xad: { uword a=WZ=IY+d;; L=mem.r8(a)&~0x20; mem.w8(a,L); } return 23; // RES 5,(IY+d),L
            case 0xae: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x20); } return 23; // RES 5,(IY+d)
            case 0xaf: { uword a=WZ=IY+d;; A=mem.r8(a)&~0x20; mem.w8(a,A); } return 23; // RES 5,(IY+d),A
            case 0xb0: { uword a=WZ=IY+d;; B=mem.r8(a)&~0x40; mem.w8(a,B); } return 23; // RES 6,(IY+d),B
            case 0xb1: { uword a=WZ=IY+d;; C=mem.r8(a)&~0x40; mem.w8(a,C); } return 23; // RES 6,(IY+d),C
            case 0xb2: { uword a=WZ=IY+d;; D=mem.r8(a)&~0x40; mem.w8(a,D); } return 23; // RES 6,(IY+d),D
            case 0xb3: { uword a=WZ=IY+d;; E=mem.r8(a)&~0x40; mem.w8(a,E); } return 23; // RES 6,(IY+d),E
            case 0xb4: { uword a=WZ=IY+d;; H=mem.r8(a)&~0x40; mem.w8(a,H); } return 23; // RES 6,(IY+d),H
            case 0xb5: { uword a=WZ=IY+d;; L=mem.r8(a)&~0x40; mem.w8(a,L); } return 23; // RES 6,(IY+d),L
            case 0xb6: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x40); } return 23; // RES 6,(IY+d)
            case 0xb7: { uword a=WZ=IY+d;; A=mem.r8(a)&~0x40; mem.w8(a,A); } return 23; // RES 6,(IY+d),A
            case 0xb8: { uword a=WZ=IY+d;; B=mem.r8(a)&~0x80; mem.w8(a,B); } return 23; // RES 7,(IY+d),B
            case 0xb9: { uword a=WZ=IY+d;; C=mem.r8(a)&~0x80; mem.w8(a,C); } return 23; // RES 7,(IY+d),C
            case 0xba: { uword a=WZ=IY+d;; D=mem.r8(a)&~0x80; mem.w8(a,D); } return 23; // RES 7,(IY+d),D
            case 0xbb: { uword a=WZ=IY+d;; E=mem.r8(a)&~0x80; mem.w8(a,E); } return 23; // RES 7,(IY+d),E
            case 0xbc: { uword a=WZ=IY+d;; H=mem.r8(a)&~0x80; mem.w8(a,H); } return 23; // RES 7,(IY+d),H
            case 0xbd: { uword a=WZ=IY+d;; L=mem.r8(a)&~0x80; mem.w8(a,L); } return 23; // RES 7,(IY+d),L
            case 0xbe: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x80); } return 23; // RES 7,(IY+d)
            case 0xbf: { uword a=WZ=IY+d;; A=mem.r8(a)&~0x80; mem.w8(a,A); } return 23; // RES 7,(IY+d),A
            case 0xc0: { uword a=WZ=IY+d;; B=mem.r8(a)|0x1; mem.w8(a,B);} return 23; // SET 0,(IY+d),B
            case 0xc1: { uword a=WZ=IY+d;; C=mem.r8(a)|0x1; mem.w8(a,C);} return 23; // SET 0,(IY+d),C
            case 0xc2: { uword a=WZ=IY+d;; D=mem.r8(a)|0x1; mem.w8(a,D);} return 23; // SET 0,(IY+d),D
            case 0xc3: { uword a=WZ=IY+d;; E=mem.r8(a)|0x1; mem.w8(a,E);} return 23; // SET 0,(IY+d),E
            case 0xc4: { uword a=WZ=IY+d;; H=mem.r8(a)|0x1; mem.w8(a,IYH);} return 23; // SET 0,(IY+d),H
            case 0xc5: { uword a=WZ=IY+d;; L=mem.r8(a)|0x1; mem.w8(a,IYL);} return 23; // SET 0,(IY+d),L
            case 0xc6: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x1);} return 23; // SET 0,(IY+d)
            case 0xc7: { uword a=WZ=IY+d;; A=mem.r8(a)|0x1; mem.w8(a,A);} return 23; // SET 0,(IY+d),A
            case 0xc8: { uword a=WZ=IY+d;; B=mem.r8(a)|0x2; mem.w8(a,B);} return 23; // SET 1,(IY+d),B
            case 0xc9: { uword a=WZ=IY+d;; C=mem.r8(a)|0x2; mem.w8(a,C);} return 23; // SET 1,(IY+d),C
            case 0xca: { uword a=WZ=IY+d;; D=mem.r8(a)|0x2; mem.w8(a,D);} return 23; // SET 1,(IY+d),D
            case 0xcb: { uword a=WZ=IY+d;; E=mem.r8(a)|0x2; mem.w8(a,E);} return 23; // SET 1,(IY+d),E
            case 0xcc: { uword a=WZ=IY+d;; H=mem.r8(a)|0x2; mem.w8(a,IYH);} return 23; // SET 1,(IY+d),H
            case 0xcd: { uword a=WZ=IY+d;; L=mem.r8(a)|0x2; mem.w8(a,IYL);} return 23; // SET 1,(IY+d),L
            case 0xce: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x2);} return 23; // SET 1,(IY+d)
            case 0xcf: { uword a=WZ=IY+d;; A=mem.r8(a)|0x2; mem.w8(a,A);} return 23; // SET 1,(IY+d),A
            case 0xd0: { uword a=WZ=IY+d;; B=mem.r8(a)|0x4; mem.w8(a,B);} return 23; // SET 2,(IY+d),B
            case 0xd1: { uword a=WZ=IY+d;; C=mem.r8(a)|0x4; mem.w8(a,C);} return 23; // SET 2,(IY+d),C
            case 0xd2: { uword a=WZ=IY+d;; D=mem.r8(a)|0x4; mem.w8(a,D);} return 23; // SET 2,(IY+d),D
            case 0xd3: { uword a=WZ=IY+d;; E=mem.r8(a)|0x4; mem.w8(a,E);} return 23; // SET 2,(IY+d),E
            case 0xd4: { uword a=WZ=IY+d;; H=mem.r8(a)|0x4; mem.w8(a,IYH);} return 23; // SET 2,(IY+d),H
            case 0xd5: { uword a=WZ=IY+d;; L=mem.r8(a)|0x4; mem.w8(a,IYL);} return 23; // SET 2,(IY+d),L
            case 0xd6: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x4);} return 23; // SET 2,(IY+d)
            case 0xd7: { uword a=WZ=IY+d;; A=mem.r8(a)|0x4; mem.w8(a,A);} return 23; // SET 2,(IY+d),A
            case 0xd8: { uword a=WZ=IY+d;; B=mem.r8(a)|0x8; mem.w8(a,B);} return 23; // SET 3,(IY+d),B
            case 0xd9: { uword a=WZ=IY+d;; C=mem.r8(a)|0x8; mem.w8(a,C);} return 23; // SET 3,(IY+d),C
            case 0xda: { uword a=WZ=IY+d;; D=mem.r8(a)|0x8; mem.w8(a,D);} return 23; // SET 3,(IY+d),D
            case 0xdb: { uword a=WZ=IY+d;; E=mem.r8(a)|0x8; mem.w8(a,E);} return 23; // SET 3,(IY+d),E
            case 0xdc: { uword a=WZ=IY+d;; H=mem.r8(a)|0x8; mem.w8(a,IYH);} return 23; // SET 3,(IY+d),H
            case 0xdd: { uword a=WZ=IY+d;; L=mem.r8(a)|0x8; mem.w8(a,IYL);} return 23; // SET 3,(IY+d),L
            case 0xde: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x8);} return 23; // SET 3,(IY+d)
            case 0xdf: { uword a=WZ=IY+d;; A=mem.r8(a)|0x8; mem.w8(a,A);} return 23; // SET 3,(IY+d),A
            case 0xe0: { uword a=WZ=IY+d;; B=mem.r8(a)|0x10; mem.w8(a,B);} return 23; // SET 4,(IY+d),B
            case 0xe1: { uword a=WZ=IY+d;; C=mem.r8(a)|0x10; mem.w8(a,C);} return 23; // SET 4,(IY+d),C
            case 0xe2: { uword a=WZ=IY+d;; D=mem.r8(a)|0x10; mem.w8(a,D);} return 23; // SET 4,(IY+d),D
            case 0xe3: { uword a=WZ=IY+d;; E=mem.r8(a)|0x10; mem.w8(a,E);} return 23; // SET 4,(IY+d),E
            case 0xe4: { uword a=WZ=IY+d;; H=mem.r8(a)|0x10; mem.w8(a,IYH);} return 23; // SET 4,(IY+d),H
            case 0xe5: { uword a=WZ=IY+d;; L=mem.r8(a)|0x10; mem.w8(a,IYL);} return 23; // SET 4,(IY+d),L
            case 0xe6: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x10);} return 23; // SET 4,(IY+d)
            case 0xe7: { uword a=WZ=IY+d;; A=mem.r8(a)|0x10; mem.w8(a,A);} return 23; // SET 4,(IY+d),A
            case 0xe8: { uword a=WZ=IY+d;; B=mem.r8(a)|0x20; mem.w8(a,B);} return 23; // SET 5,(IY+d),B
            case 0xe9: { uword a=WZ=IY+d;; C=mem.r8(a)|0x20; mem.w8(a,C);} return 23; // SET 5,(IY+d),C
            case 0xea: { uword a=WZ=IY+d;; D=mem.r8(a)|0x20; mem.w8(a,D);} return 23; // SET 5,(IY+d),D
            case 0xeb: { uword a=WZ=IY+d;; E=mem.r8(a)|0x20; mem.w8(a,E);} return 23; // SET 5,(IY+d),E
            case 0xec: { uword a=WZ=IY+d;; H=mem.r8(a)|0x20; mem.w8(a,IYH);} return 23; // SET 5,(IY+d),H
            case 0xed: { uword a=WZ=IY+d;; L=mem.r8(a)|0x20; mem.w8(a,IYL);} return 23; // SET 5,(IY+d),L
            case 0xee: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x20);} return 23; // SET 5,(IY+d)
            case 0xef: { uword a=WZ=IY+d;; A=mem.r8(a)|0x20; mem.w8(a,A);} return 23; // SET 5,(IY+d),A
            case 0xf0: { uword a=WZ=IY+d;; B=mem.r8(a)|0x40; mem.w8(a,B);} return 23; // SET 6,(IY+d),B
            case 0xf1: { uword a=WZ=IY+d;; C=mem.r8(a)|0x40; mem.w8(a,C);} return 23; // SET 6,(IY+d),C
            case 0xf2: { uword a=WZ=IY+d;; D=mem.r8(a)|0x40; mem.w8(a,D);} return 23; // SET 6,(IY+d),D
            case 0xf3: { uword a=WZ=IY+d;; E=mem.r8(a)|0x40; mem.w8(a,E);} return 23; // SET 6,(IY+d),E
            case 0xf4: { uword a=WZ=IY+d;; H=mem.r8(a)|0x40; mem.w8(a,IYH);} return 23; // SET 6,(IY+d),H
            case 0xf5: { uword a=WZ=IY+d;; L=mem.r8(a)|0x40; mem.w8(a,IYL);} return 23; // SET 6,(IY+d),L
            case 0xf6: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x40);} return 23; // SET 6,(IY+d)
            case 0xf7: { uword a=WZ=IY+d;; A=mem.r8(a)|0x40; mem.w8(a,A);} return 23; // SET 6,(IY+d),A
            case 0xf8: { uword a=WZ=IY+d;; B=mem.r8(a)|0x80; mem.w8(a,B);} return 23; // SET 7,(IY+d),B
            case 0xf9: { uword a=WZ=IY+d;; C=mem.r8(a)|0x80; mem.w8(a,C);} return 23; // SET 7,(IY+d),C
            case 0xfa: { uword a=WZ=IY+d;; D=mem.r8(a)|0x80; mem.w8(a,D);} return 23; // SET 7,(IY+d),D
            case 0xfb: { uword a=WZ=IY+d;; E=mem.r8(a)|0x80; mem.w8(a,E);} return 23; // SET 7,(IY+d),E
            case 0xfc: { uword a=WZ=IY+d;; H=mem.r8(a)|0x80; mem.w8(a,IYH);} return 23; // SET 7,(IY+d),H
            case 0xfd: { uword a=WZ=IY+d;; L=mem.r8(a)|0x80; mem.w8(a,IYL);} return 23; // SET 7,(IY+d),L
            case 0xfe: { uword a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x80);} return 23; // SET 7,(IY+d)
            case 0xff: { uword a=WZ=IY+d;; A=mem.r8(a)|0x80; mem.w8(a,A);} return 23; // SET 7,(IY+d),A
            default: return invalid_opcode(4);
          }
          break;
          }
        case 0xcc: WZ=mem.r16(PC); PC+=2; if ((F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL Z,nn
        case 0xcd: SP-=2; mem.w16(SP,PC+2); WZ=PC=mem.r16(PC); return 21; // CALL nn
        case 0xce: adc8(mem.r8(PC++)); return 11; // ADC n
        case 0xcf: rst(0x8); return 15; // RST 0x8
        case 0xd0: if (!(F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET NC
        case 0xd1: DE=mem.r16(SP); SP+=2; return 14; // POP DE
        case 0xd2: WZ=mem.r16(PC); if (!(F&CF)) { PC=WZ; } else { PC+=2; }; return 14; // JP NC,nn
        case 0xd3: out(bus, (A<<8)|mem.r8(PC++),A); return 15; // OUT (n),A
        case 0xd4: WZ=mem.r16(PC); PC+=2; if (!(F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL NC,nn
        case 0xd5: SP-=2; mem.w16(SP,DE); return 15; // PUSH DE
        case 0xd6: sub8(mem.r8(PC++)); return 11; // SUB n
        case 0xd7: rst(0x10); return 15; // RST 0x10
        case 0xd8: if ((F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET C
        case 0xd9: swap16(BC,BC_); swap16(DE,DE_); swap16(HL,HL_); swap16(WZ,WZ_); return 8; // EXX
        case 0xda: WZ=mem.r16(PC); if ((F&CF)) { PC=WZ; } else { PC+=2; }; return 14; // JP C,nn
        case 0xdb: A=in(bus, (A<<8)|mem.r8(PC++)); return 15; // IN A,(n)
        case 0xdc: WZ=mem.r16(PC); PC+=2; if ((F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL C,nn
        case 0xde: sbc8(mem.r8(PC++)); return 11; // SBC n
        case 0xdf: rst(0x18); return 15; // RST 0x18
        case 0xe0: if (!(F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET PO
        case 0xe1: IY=mem.r16(SP); SP+=2; return 14; // POP IY
        case 0xe2: WZ=mem.r16(PC); if (!(F&PF)) { PC=WZ; } else { PC+=2; }; return 14; // JP PO,nn
        case 0xe3: {uword swp=mem.r16(SP); mem.w16(SP,IY); IY=WZ=swp;} return 23; // EX (SP),IY
        case 0xe4: WZ=mem.r16(PC); PC+=2; if (!(F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL PO,nn
        case 0xe5: SP-=2; mem.w16(SP,IY); return 15; // PUSH IY
        case 0xe6: and8(mem.r8(PC++)); return 11; // AND n
        case 0xe7: rst(0x20); return 15; // RST 0x20
        case 0xe8: if ((F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET PE
        case 0xe9: PC=IY; return 8; // JP IY
        case 0xea: WZ=mem.r16(PC); if ((F&PF)) { PC=WZ; } else { PC+=2; }; return 14; // JP PE,nn
        case 0xeb: swap16(DE,HL); return 8; // EX DE,HL
        case 0xec: WZ=mem.r16(PC); PC+=2; if ((F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL PE,nn
        case 0xee: xor8(mem.r8(PC++)); return 11; // XOR n
        case 0xef: rst(0x28); return 15; // RST 0x28
        case 0xf0: if (!(F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET P
        case 0xf1: AF=mem.r16(SP); SP+=2; return 14; // POP AF
        case 0xf2: WZ=mem.r16(PC); if (!(F&SF)) { PC=WZ; } else { PC+=2; }; return 14; // JP P,nn
        case 0xf3: di(); return 8; // DI
        case 0xf4: WZ=mem.r16(PC); PC+=2; if (!(F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL P,nn
        case 0xf5: SP-=2; mem.w16(SP,AF); return 15; // PUSH AF
        case 0xf6: or8(mem.r8(PC++)); return 11; // OR n
        case 0xf7: rst(0x30); return 15; // RST 0x30
        case 0xf8: if ((F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return 15; } else return 9; // RET M
        case 0xf9: SP=IY; return 10; // LD SP,IY
        case 0xfa: WZ=mem.r16(PC); if ((F&SF)) { PC=WZ; } else { PC+=2; }; return 14; // JP M,nn
        case 0xfb: ei(); return 8; // EI
        case 0xfc: WZ=mem.r16(PC); PC+=2; if ((F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return 21; } else { return 14; } // CALL M,nn
        case 0xfe: cp8(mem.r8(PC++)); return 11; // CP n
        case 0xff: rst(0x38); return 15; // RST 0x38
        default: return invalid_opcode(2);
      }
      break;
    case 0xfe: cp8(mem.r8(PC++)); return 7; // CP n
    case 0xff: rst(0x38); return 11; // RST 0x38
    default: return invalid_opcode(1);
  }
}
} // namespace YAKC
