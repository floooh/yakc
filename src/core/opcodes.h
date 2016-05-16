#pragma once
// #version:2#
// machine generated, do not edit!
#include "core/common.h"
namespace yakc {
inline uint32_t z80::do_op_0xcb(ubyte op) {
    switch (op) {
        case 0x0:
            // RLC B
            B = rlc8(B, true);
            return 8;
        case 0x1:
            // RLC C
            C = rlc8(C, true);
            return 8;
        case 0x2:
            // RLC D
            D = rlc8(D, true);
            return 8;
        case 0x3:
            // RLC E
            E = rlc8(E, true);
            return 8;
        case 0x4:
            // RLC H
            H = rlc8(H, true);
            return 8;
        case 0x5:
            // RLC L
            L = rlc8(L, true);
            return 8;
        case 0x6:
            // RLC (HL)
            mem.w8(HL,rlc8(mem.r8(HL), true));
            return 15;
        case 0x7:
            // RLC A
            A = rlc8(A, true);
            return 8;
        case 0x8:
            // RRC B
            B = rrc8(B, true);
            return 8;
        case 0x9:
            // RRC C
            C = rrc8(C, true);
            return 8;
        case 0xa:
            // RRC D
            D = rrc8(D, true);
            return 8;
        case 0xb:
            // RRC E
            E = rrc8(E, true);
            return 8;
        case 0xc:
            // RRC H
            H = rrc8(H, true);
            return 8;
        case 0xd:
            // RRC L
            L = rrc8(L, true);
            return 8;
        case 0xe:
            // RRC (HL)
            mem.w8(HL,rrc8(mem.r8(HL), true));
            return 15;
        case 0xf:
            // RRC A
            A = rrc8(A, true);
            return 8;
        case 0x10:
            // RL B
            B = rl8(B, true);
            return 8;
        case 0x11:
            // RR C
            C = rl8(C, true);
            return 8;
        case 0x12:
            // RL D
            D = rl8(D, true);
            return 8;
        case 0x13:
            // RL E
            E = rl8(E, true);
            return 8;
        case 0x14:
            // RL H
            H = rl8(H, true);
            return 8;
        case 0x15:
            // RL L
            L = rl8(L, true);
            return 8;
        case 0x16:
            // RL (HL)
            mem.w8(HL,rl8(mem.r8(HL), true));
            return 15;
        case 0x17:
            // RL A
            A = rl8(A, true);
            return 8;
        case 0x18:
            // RR B
            B = rr8(B, true);
            return 8;
        case 0x19:
            // RR C
            C = rr8(C, true);
            return 8;
        case 0x1a:
            // RR D
            D = rr8(D, true);
            return 8;
        case 0x1b:
            // RR E
            E = rr8(E, true);
            return 8;
        case 0x1c:
            // RR H
            H = rr8(H, true);
            return 8;
        case 0x1d:
            // RR L
            L = rr8(L, true);
            return 8;
        case 0x1e:
            // RR (HL)
            mem.w8(HL,rr8(mem.r8(HL), true));
            return 15;
        case 0x1f:
            // RR A
            A = rr8(A, true);
            return 8;
        case 0x20:
            // SLA B
            B = sla8(B);
            return 8;
        case 0x21:
            // SLA C
            C = sla8(C);
            return 8;
        case 0x22:
            // SLA D
            D = sla8(D);
            return 8;
        case 0x23:
            // SLA E
            E = sla8(E);
            return 8;
        case 0x24:
            // SLA H
            H = sla8(H);
            return 8;
        case 0x25:
            // SLA L
            L = sla8(L);
            return 8;
        case 0x26:
            // SLA (HL)
            mem.w8(HL,sla8(mem.r8(HL)));
            return 15;
        case 0x27:
            // SLA A
            A = sla8(A);
            return 8;
        case 0x28:
            // SRA B
            B = sra8(B);
            return 8;
        case 0x29:
            // SRA C
            C = sra8(C);
            return 8;
        case 0x2a:
            // SRA D
            D = sra8(D);
            return 8;
        case 0x2b:
            // SRA E
            E = sra8(E);
            return 8;
        case 0x2c:
            // SRA H
            H = sra8(H);
            return 8;
        case 0x2d:
            // SRA L
            L = sra8(L);
            return 8;
        case 0x2e:
            // SRA (HL)
            mem.w8(HL,sra8(mem.r8(HL)));
            return 15;
        case 0x2f:
            // SRA A
            A = sra8(A);
            return 8;
        case 0x30:
            // SLL B
            B = sll8(B);
            return 8;
        case 0x31:
            // SLL C
            C = sll8(C);
            return 8;
        case 0x32:
            // SLL D
            D = sll8(D);
            return 8;
        case 0x33:
            // SLL E
            E = sll8(E);
            return 8;
        case 0x34:
            // SLL H
            H = sll8(H);
            return 8;
        case 0x35:
            // SLL L
            L = sll8(L);
            return 8;
        case 0x36:
            // SLL (HL)
            mem.w8(HL,sll8(mem.r8(HL)));
            return 15;
        case 0x37:
            // SLL A
            A = sll8(A);
            return 8;
        case 0x38:
            // SRL B
            B = srl8(B);
            return 8;
        case 0x39:
            // SRL C
            C = srl8(C);
            return 8;
        case 0x3a:
            // SRL D
            D = srl8(D);
            return 8;
        case 0x3b:
            // SRL E
            E = srl8(E);
            return 8;
        case 0x3c:
            // SRL H
            H = srl8(H);
            return 8;
        case 0x3d:
            // SRL L
            L = srl8(L);
            return 8;
        case 0x3e:
            // SRL (HL)
            mem.w8(HL,srl8(mem.r8(HL)));
            return 15;
        case 0x3f:
            // SRL A
            A = srl8(A);
            return 8;
        case 0x40:
            // BIT 0,B
            bit(B,(1<<0));
            return 8;
        case 0x41:
            // BIT 0,C
            bit(C,(1<<0));
            return 8;
        case 0x42:
            // BIT 0,D
            bit(D,(1<<0));
            return 8;
        case 0x43:
            // BIT 0,E
            bit(E,(1<<0));
            return 8;
        case 0x44:
            // BIT 0,H
            bit(H,(1<<0));
            return 8;
        case 0x45:
            // BIT 0,L
            bit(L,(1<<0));
            return 8;
        case 0x46:
            // BIT 0,(HL)
            bit(mem.r8(HL),(1<<0));
            return 12;
        case 0x47:
            // BIT 0,A
            bit(A,(1<<0));
            return 8;
        case 0x48:
            // BIT 1,B
            bit(B,(1<<1));
            return 8;
        case 0x49:
            // BIT 1,C
            bit(C,(1<<1));
            return 8;
        case 0x4a:
            // BIT 1,D
            bit(D,(1<<1));
            return 8;
        case 0x4b:
            // BIT 1,E
            bit(E,(1<<1));
            return 8;
        case 0x4c:
            // BIT 1,H
            bit(H,(1<<1));
            return 8;
        case 0x4d:
            // BIT 1,L
            bit(L,(1<<1));
            return 8;
        case 0x4e:
            // BIT 1,(HL)
            bit(mem.r8(HL),(1<<1));
            return 12;
        case 0x4f:
            // BIT 1,A
            bit(A,(1<<1));
            return 8;
        case 0x50:
            // BIT 2,B
            bit(B,(1<<2));
            return 8;
        case 0x51:
            // BIT 2,C
            bit(C,(1<<2));
            return 8;
        case 0x52:
            // BIT 2,D
            bit(D,(1<<2));
            return 8;
        case 0x53:
            // BIT 2,E
            bit(E,(1<<2));
            return 8;
        case 0x54:
            // BIT 2,H
            bit(H,(1<<2));
            return 8;
        case 0x55:
            // BIT 2,L
            bit(L,(1<<2));
            return 8;
        case 0x56:
            // BIT 2,(HL)
            bit(mem.r8(HL),(1<<2));
            return 12;
        case 0x57:
            // BIT 2,A
            bit(A,(1<<2));
            return 8;
        case 0x58:
            // BIT 3,B
            bit(B,(1<<3));
            return 8;
        case 0x59:
            // BIT 3,C
            bit(C,(1<<3));
            return 8;
        case 0x5a:
            // BIT 3,D
            bit(D,(1<<3));
            return 8;
        case 0x5b:
            // BIT 3,E
            bit(E,(1<<3));
            return 8;
        case 0x5c:
            // BIT 3,H
            bit(H,(1<<3));
            return 8;
        case 0x5d:
            // BIT 3,L
            bit(L,(1<<3));
            return 8;
        case 0x5e:
            // BIT 3,(HL)
            bit(mem.r8(HL),(1<<3));
            return 12;
        case 0x5f:
            // BIT 3,A
            bit(A,(1<<3));
            return 8;
        case 0x60:
            // BIT 4,B
            bit(B,(1<<4));
            return 8;
        case 0x61:
            // BIT 4,C
            bit(C,(1<<4));
            return 8;
        case 0x62:
            // BIT 4,D
            bit(D,(1<<4));
            return 8;
        case 0x63:
            // BIT 4,E
            bit(E,(1<<4));
            return 8;
        case 0x64:
            // BIT 4,H
            bit(H,(1<<4));
            return 8;
        case 0x65:
            // BIT 4,L
            bit(L,(1<<4));
            return 8;
        case 0x66:
            // BIT 4,(HL)
            bit(mem.r8(HL),(1<<4));
            return 12;
        case 0x67:
            // BIT 4,A
            bit(A,(1<<4));
            return 8;
        case 0x68:
            // BIT 5,B
            bit(B,(1<<5));
            return 8;
        case 0x69:
            // BIT 5,C
            bit(C,(1<<5));
            return 8;
        case 0x6a:
            // BIT 5,D
            bit(D,(1<<5));
            return 8;
        case 0x6b:
            // BIT 5,E
            bit(E,(1<<5));
            return 8;
        case 0x6c:
            // BIT 5,H
            bit(H,(1<<5));
            return 8;
        case 0x6d:
            // BIT 5,L
            bit(L,(1<<5));
            return 8;
        case 0x6e:
            // BIT 5,(HL)
            bit(mem.r8(HL),(1<<5));
            return 12;
        case 0x6f:
            // BIT 5,A
            bit(A,(1<<5));
            return 8;
        case 0x70:
            // BIT 6,B
            bit(B,(1<<6));
            return 8;
        case 0x71:
            // BIT 6,C
            bit(C,(1<<6));
            return 8;
        case 0x72:
            // BIT 6,D
            bit(D,(1<<6));
            return 8;
        case 0x73:
            // BIT 6,E
            bit(E,(1<<6));
            return 8;
        case 0x74:
            // BIT 6,H
            bit(H,(1<<6));
            return 8;
        case 0x75:
            // BIT 6,L
            bit(L,(1<<6));
            return 8;
        case 0x76:
            // BIT 6,(HL)
            bit(mem.r8(HL),(1<<6));
            return 12;
        case 0x77:
            // BIT 6,A
            bit(A,(1<<6));
            return 8;
        case 0x78:
            // BIT 7,B
            bit(B,(1<<7));
            return 8;
        case 0x79:
            // BIT 7,C
            bit(C,(1<<7));
            return 8;
        case 0x7a:
            // BIT 7,D
            bit(D,(1<<7));
            return 8;
        case 0x7b:
            // BIT 7,E
            bit(E,(1<<7));
            return 8;
        case 0x7c:
            // BIT 7,H
            bit(H,(1<<7));
            return 8;
        case 0x7d:
            // BIT 7,L
            bit(L,(1<<7));
            return 8;
        case 0x7e:
            // BIT 7,(HL)
            bit(mem.r8(HL),(1<<7));
            return 12;
        case 0x7f:
            // BIT 7,A
            bit(A,(1<<7));
            return 8;
        case 0x80:
            // RES 0,B
            B = B&~(1<<0);
            return 8;
        case 0x81:
            // RES 0,C
            C = C&~(1<<0);
            return 8;
        case 0x82:
            // RES 0,D
            D = D&~(1<<0);
            return 8;
        case 0x83:
            // RES 0,E
            E = E&~(1<<0);
            return 8;
        case 0x84:
            // RES 0,H
            H = H&~(1<<0);
            return 8;
        case 0x85:
            // RES 0,L
            L = L&~(1<<0);
            return 8;
        case 0x86:
            // RES 0,(HL)
            mem.w8(HL,mem.r8(HL)&~(1<<0));
            return 15;
        case 0x87:
            // RES 0,A
            A = A&~(1<<0);
            return 8;
        case 0x88:
            // RES 1,B
            B = B&~(1<<1);
            return 8;
        case 0x89:
            // RES 1,C
            C = C&~(1<<1);
            return 8;
        case 0x8a:
            // RES 1,D
            D = D&~(1<<1);
            return 8;
        case 0x8b:
            // RES 1,E
            E = E&~(1<<1);
            return 8;
        case 0x8c:
            // RES 1,H
            H = H&~(1<<1);
            return 8;
        case 0x8d:
            // RES 1,L
            L = L&~(1<<1);
            return 8;
        case 0x8e:
            // RES 1,(HL)
            mem.w8(HL,mem.r8(HL)&~(1<<1));
            return 15;
        case 0x8f:
            // RES 1,A
            A = A&~(1<<1);
            return 8;
        case 0x90:
            // RES 2,B
            B = B&~(1<<2);
            return 8;
        case 0x91:
            // RES 2,C
            C = C&~(1<<2);
            return 8;
        case 0x92:
            // RES 2,D
            D = D&~(1<<2);
            return 8;
        case 0x93:
            // RES 2,E
            E = E&~(1<<2);
            return 8;
        case 0x94:
            // RES 2,H
            H = H&~(1<<2);
            return 8;
        case 0x95:
            // RES 2,L
            L = L&~(1<<2);
            return 8;
        case 0x96:
            // RES 2,(HL)
            mem.w8(HL,mem.r8(HL)&~(1<<2));
            return 15;
        case 0x97:
            // RES 2,A
            A = A&~(1<<2);
            return 8;
        case 0x98:
            // RES 3,B
            B = B&~(1<<3);
            return 8;
        case 0x99:
            // RES 3,C
            C = C&~(1<<3);
            return 8;
        case 0x9a:
            // RES 3,D
            D = D&~(1<<3);
            return 8;
        case 0x9b:
            // RES 3,E
            E = E&~(1<<3);
            return 8;
        case 0x9c:
            // RES 3,H
            H = H&~(1<<3);
            return 8;
        case 0x9d:
            // RES 3,L
            L = L&~(1<<3);
            return 8;
        case 0x9e:
            // RES 3,(HL)
            mem.w8(HL,mem.r8(HL)&~(1<<3));
            return 15;
        case 0x9f:
            // RES 3,A
            A = A&~(1<<3);
            return 8;
        case 0xa0:
            // RES 4,B
            B = B&~(1<<4);
            return 8;
        case 0xa1:
            // RES 4,C
            C = C&~(1<<4);
            return 8;
        case 0xa2:
            // RES 4,D
            D = D&~(1<<4);
            return 8;
        case 0xa3:
            // RES 4,E
            E = E&~(1<<4);
            return 8;
        case 0xa4:
            // RES 4,H
            H = H&~(1<<4);
            return 8;
        case 0xa5:
            // RES 4,L
            L = L&~(1<<4);
            return 8;
        case 0xa6:
            // RES 4,(HL)
            mem.w8(HL,mem.r8(HL)&~(1<<4));
            return 15;
        case 0xa7:
            // RES 4,A
            A = A&~(1<<4);
            return 8;
        case 0xa8:
            // RES 5,B
            B = B&~(1<<5);
            return 8;
        case 0xa9:
            // RES 5,C
            C = C&~(1<<5);
            return 8;
        case 0xaa:
            // RES 5,D
            D = D&~(1<<5);
            return 8;
        case 0xab:
            // RES 5,E
            E = E&~(1<<5);
            return 8;
        case 0xac:
            // RES 5,H
            H = H&~(1<<5);
            return 8;
        case 0xad:
            // RES 5,L
            L = L&~(1<<5);
            return 8;
        case 0xae:
            // RES 5,(HL)
            mem.w8(HL,mem.r8(HL)&~(1<<5));
            return 15;
        case 0xaf:
            // RES 5,A
            A = A&~(1<<5);
            return 8;
        case 0xb0:
            // RES 6,B
            B = B&~(1<<6);
            return 8;
        case 0xb1:
            // RES 6,C
            C = C&~(1<<6);
            return 8;
        case 0xb2:
            // RES 6,D
            D = D&~(1<<6);
            return 8;
        case 0xb3:
            // RES 6,E
            E = E&~(1<<6);
            return 8;
        case 0xb4:
            // RES 6,H
            H = H&~(1<<6);
            return 8;
        case 0xb5:
            // RES 6,L
            L = L&~(1<<6);
            return 8;
        case 0xb6:
            // RES 6,(HL)
            mem.w8(HL,mem.r8(HL)&~(1<<6));
            return 15;
        case 0xb7:
            // RES 6,A
            A = A&~(1<<6);
            return 8;
        case 0xb8:
            // RES 7,B
            B = B&~(1<<7);
            return 8;
        case 0xb9:
            // RES 7,C
            C = C&~(1<<7);
            return 8;
        case 0xba:
            // RES 7,D
            D = D&~(1<<7);
            return 8;
        case 0xbb:
            // RES 7,E
            E = E&~(1<<7);
            return 8;
        case 0xbc:
            // RES 7,H
            H = H&~(1<<7);
            return 8;
        case 0xbd:
            // RES 7,L
            L = L&~(1<<7);
            return 8;
        case 0xbe:
            // RES 7,(HL)
            mem.w8(HL,mem.r8(HL)&~(1<<7));
            return 15;
        case 0xbf:
            // RES 7,A
            A = A&~(1<<7);
            return 8;
        case 0xc0:
            // SET 0,B
            B = B|(1<<0);
            return 8;
        case 0xc1:
            // SET 0,C
            C = C|(1<<0);
            return 8;
        case 0xc2:
            // SET 0,D
            D = D|(1<<0);
            return 8;
        case 0xc3:
            // SET 0,E
            E = E|(1<<0);
            return 8;
        case 0xc4:
            // SET 0,H
            H = H|(1<<0);
            return 8;
        case 0xc5:
            // SET 0,L
            L = L|(1<<0);
            return 8;
        case 0xc6:
            // SET 0,(HL)
            mem.w8(HL,mem.r8(HL)|(1<<0));
            return 15;
        case 0xc7:
            // SET 0,A
            A = A|(1<<0);
            return 8;
        case 0xc8:
            // SET 1,B
            B = B|(1<<1);
            return 8;
        case 0xc9:
            // SET 1,C
            C = C|(1<<1);
            return 8;
        case 0xca:
            // SET 1,D
            D = D|(1<<1);
            return 8;
        case 0xcb:
            // SET 1,E
            E = E|(1<<1);
            return 8;
        case 0xcc:
            // SET 1,H
            H = H|(1<<1);
            return 8;
        case 0xcd:
            // SET 1,L
            L = L|(1<<1);
            return 8;
        case 0xce:
            // SET 1,(HL)
            mem.w8(HL,mem.r8(HL)|(1<<1));
            return 15;
        case 0xcf:
            // SET 1,A
            A = A|(1<<1);
            return 8;
        case 0xd0:
            // SET 2,B
            B = B|(1<<2);
            return 8;
        case 0xd1:
            // SET 2,C
            C = C|(1<<2);
            return 8;
        case 0xd2:
            // SET 2,D
            D = D|(1<<2);
            return 8;
        case 0xd3:
            // SET 2,E
            E = E|(1<<2);
            return 8;
        case 0xd4:
            // SET 2,H
            H = H|(1<<2);
            return 8;
        case 0xd5:
            // SET 2,L
            L = L|(1<<2);
            return 8;
        case 0xd6:
            // SET 2,(HL)
            mem.w8(HL,mem.r8(HL)|(1<<2));
            return 15;
        case 0xd7:
            // SET 2,A
            A = A|(1<<2);
            return 8;
        case 0xd8:
            // SET 3,B
            B = B|(1<<3);
            return 8;
        case 0xd9:
            // SET 3,C
            C = C|(1<<3);
            return 8;
        case 0xda:
            // SET 3,D
            D = D|(1<<3);
            return 8;
        case 0xdb:
            // SET 3,E
            E = E|(1<<3);
            return 8;
        case 0xdc:
            // SET 3,H
            H = H|(1<<3);
            return 8;
        case 0xdd:
            // SET 3,L
            L = L|(1<<3);
            return 8;
        case 0xde:
            // SET 3,(HL)
            mem.w8(HL,mem.r8(HL)|(1<<3));
            return 15;
        case 0xdf:
            // SET 3,A
            A = A|(1<<3);
            return 8;
        case 0xe0:
            // SET 4,B
            B = B|(1<<4);
            return 8;
        case 0xe1:
            // SET 4,C
            C = C|(1<<4);
            return 8;
        case 0xe2:
            // SET 4,D
            D = D|(1<<4);
            return 8;
        case 0xe3:
            // SET 4,E
            E = E|(1<<4);
            return 8;
        case 0xe4:
            // SET 4,H
            H = H|(1<<4);
            return 8;
        case 0xe5:
            // SET 4,L
            L = L|(1<<4);
            return 8;
        case 0xe6:
            // SET 4,(HL)
            mem.w8(HL,mem.r8(HL)|(1<<4));
            return 15;
        case 0xe7:
            // SET 4,A
            A = A|(1<<4);
            return 8;
        case 0xe8:
            // SET 5,B
            B = B|(1<<5);
            return 8;
        case 0xe9:
            // SET 5,C
            C = C|(1<<5);
            return 8;
        case 0xea:
            // SET 5,D
            D = D|(1<<5);
            return 8;
        case 0xeb:
            // SET 5,E
            E = E|(1<<5);
            return 8;
        case 0xec:
            // SET 5,H
            H = H|(1<<5);
            return 8;
        case 0xed:
            // SET 5,L
            L = L|(1<<5);
            return 8;
        case 0xee:
            // SET 5,(HL)
            mem.w8(HL,mem.r8(HL)|(1<<5));
            return 15;
        case 0xef:
            // SET 5,A
            A = A|(1<<5);
            return 8;
        case 0xf0:
            // SET 6,B
            B = B|(1<<6);
            return 8;
        case 0xf1:
            // SET 6,C
            C = C|(1<<6);
            return 8;
        case 0xf2:
            // SET 6,D
            D = D|(1<<6);
            return 8;
        case 0xf3:
            // SET 6,E
            E = E|(1<<6);
            return 8;
        case 0xf4:
            // SET 6,H
            H = H|(1<<6);
            return 8;
        case 0xf5:
            // SET 6,L
            L = L|(1<<6);
            return 8;
        case 0xf6:
            // SET 6,(HL)
            mem.w8(HL,mem.r8(HL)|(1<<6));
            return 15;
        case 0xf7:
            // SET 6,A
            A = A|(1<<6);
            return 8;
        case 0xf8:
            // SET 7,B
            B = B|(1<<7);
            return 8;
        case 0xf9:
            // SET 7,C
            C = C|(1<<7);
            return 8;
        case 0xfa:
            // SET 7,D
            D = D|(1<<7);
            return 8;
        case 0xfb:
            // SET 7,E
            E = E|(1<<7);
            return 8;
        case 0xfc:
            // SET 7,H
            H = H|(1<<7);
            return 8;
        case 0xfd:
            // SET 7,L
            L = L|(1<<7);
            return 8;
        case 0xfe:
            // SET 7,(HL)
            mem.w8(HL,mem.r8(HL)|(1<<7));
            return 15;
        case 0xff:
            // SET 7,A
            A = A|(1<<7);
            return 8;
        default:
            return invalid_opcode(2);
    }
};
inline uint32_t z80::do_op_0xdd(ubyte op) {
    int d; uword u16tmp;
    switch (op) {
        case 0x9:
            // ADD IX,BC
            IX = add16(IX, BC);
            return 15;
        case 0x19:
            // ADD IX,DE
            IX = add16(IX, DE);
            return 15;
        case 0x21:
            // LD IX,nn
            IX = mem.r16(PC);
            PC += 2;
            return 14;
        case 0x22:
            // LD (nn),IX
            mem.w16(mem.r16(PC), IX);
            PC += 2;
            return 20;
        case 0x23:
            // INC IX
            IX++;
            return 10;
        case 0x24:
            // INC IXH
            IXH = inc8(IXH);
            return 8;
        case 0x25:
            // DEC IXH
            IXH = dec8(IXH);
            return 8;
        case 0x26:
            // LD IXH,n
            IXH = mem.r8(PC++);
            return 11;
        case 0x29:
            // ADD IX,IX
            IX = add16(IX, IX);
            return 15;
        case 0x2a:
            // LD IX,(nn)
            IX = mem.r16(mem.r16(PC));
            PC += 2;
            return 20;
        case 0x2b:
            // DEC IX
            IX--;
            return 10;
        case 0x2c:
            // INC IXL
            IXL = inc8(IXL);
            return 8;
        case 0x2d:
            // DEC IXL
            IXL = dec8(IXL);
            return 8;
        case 0x2e:
            // LD IXL,n
            IXL = mem.r8(PC++);
            return 11;
        case 0x34:
            // INC (IX+d)
            d = mem.rs8(PC++);
            mem.w8(IX+d, inc8(mem.r8(IX+d)));
            return 23;
        case 0x35:
            // DEC (IX+d)
            d = mem.rs8(PC++);
            mem.w8(IX+d, dec8(mem.r8(IX+d)));
            return 23;
        case 0x36:
            // LD (IX+d),n
            d = mem.rs8(PC++);
            mem.w8(IX + d, mem.r8(PC++));
            return 19;
        case 0x39:
            // ADD IX,SP
            IX = add16(IX, SP);
            return 15;
        case 0x40:
            // LD B,B
            B = B;
            return 8;
        case 0x41:
            // LD B,C
            B = C;
            return 8;
        case 0x42:
            // LD B,D
            B = D;
            return 8;
        case 0x43:
            // LD B,E
            B = E;
            return 8;
        case 0x44:
            // LD B,IXH
            B = IXH;
            return 8;
        case 0x45:
            // LD B,IXL
            B = IXL;
            return 8;
        case 0x46:
            // LD B,(IX+d)
            d = mem.rs8(PC++);
            B = mem.r8(IX + d);
            return 19;
        case 0x47:
            // LD B,A
            B = A;
            return 8;
        case 0x48:
            // LD C,B
            C = B;
            return 8;
        case 0x49:
            // LD C,C
            C = C;
            return 8;
        case 0x4a:
            // LD C,D
            C = D;
            return 8;
        case 0x4b:
            // LD C,E
            C = E;
            return 8;
        case 0x4c:
            // LD C,IXH
            C = IXH;
            return 8;
        case 0x4d:
            // LD C,IXL
            C = IXL;
            return 8;
        case 0x4e:
            // LD C,(IX+d)
            d = mem.rs8(PC++);
            C = mem.r8(IX + d);
            return 19;
        case 0x4f:
            // LD C,A
            C = A;
            return 8;
        case 0x50:
            // LD D,B
            D = B;
            return 8;
        case 0x51:
            // LD D,C
            D = C;
            return 8;
        case 0x52:
            // LD D,D
            D = D;
            return 8;
        case 0x53:
            // LD D,E
            D = E;
            return 8;
        case 0x54:
            // LD D,IXH
            D = IXH;
            return 8;
        case 0x55:
            // LD D,IXL
            D = IXL;
            return 8;
        case 0x56:
            // LD D,(IX+d)
            d = mem.rs8(PC++);
            D = mem.r8(IX + d);
            return 19;
        case 0x57:
            // LD D,A
            D = A;
            return 8;
        case 0x58:
            // LD E,B
            E = B;
            return 8;
        case 0x59:
            // LD E,C
            E = C;
            return 8;
        case 0x5a:
            // LD E,D
            E = D;
            return 8;
        case 0x5b:
            // LD E,E
            E = E;
            return 8;
        case 0x5c:
            // LD E,IXH
            E = IXH;
            return 8;
        case 0x5d:
            // LD E,IXL
            E = IXL;
            return 8;
        case 0x5e:
            // LD E,(IX+d)
            d = mem.rs8(PC++);
            E = mem.r8(IX + d);
            return 19;
        case 0x5f:
            // LD E,A
            E = A;
            return 8;
        case 0x60:
            // LD IXH,B
            IXH = B;
            return 8;
        case 0x61:
            // LD IXH,C
            IXH = C;
            return 8;
        case 0x62:
            // LD IXH,D
            IXH = D;
            return 8;
        case 0x63:
            // LD IXH,E
            IXH = E;
            return 8;
        case 0x64:
            // LD IXH,IXH
            IXH = IXH;
            return 8;
        case 0x65:
            // LD IXH,IXL
            IXH = IXL;
            return 8;
        case 0x66:
            // LD H,(IX+d)
            d = mem.rs8(PC++);
            H = mem.r8(IX + d);
            return 19;
        case 0x67:
            // LD IXH,A
            IXH = A;
            return 8;
        case 0x68:
            // LD IXL,B
            IXL = B;
            return 8;
        case 0x69:
            // LD IXL,C
            IXL = C;
            return 8;
        case 0x6a:
            // LD IXL,D
            IXL = D;
            return 8;
        case 0x6b:
            // LD IXL,E
            IXL = E;
            return 8;
        case 0x6c:
            // LD IXL,IXH
            IXL = IXH;
            return 8;
        case 0x6d:
            // LD IXL,IXL
            IXL = IXL;
            return 8;
        case 0x6e:
            // LD L,(IX+d)
            d = mem.rs8(PC++);
            L = mem.r8(IX + d);
            return 19;
        case 0x6f:
            // LD IXL,A
            IXL = A;
            return 8;
        case 0x70:
            // LD (IX+d),B
            d = mem.rs8(PC++);
            mem.w8(IX + d, B);
            return 19;
        case 0x71:
            // LD (IX+d),C
            d = mem.rs8(PC++);
            mem.w8(IX + d, C);
            return 19;
        case 0x72:
            // LD (IX+d),D
            d = mem.rs8(PC++);
            mem.w8(IX + d, D);
            return 19;
        case 0x73:
            // LD (IX+d),E
            d = mem.rs8(PC++);
            mem.w8(IX + d, E);
            return 19;
        case 0x74:
            // LD (IX+d),H
            d = mem.rs8(PC++);
            mem.w8(IX + d, H);
            return 19;
        case 0x75:
            // LD (IX+d),L
            d = mem.rs8(PC++);
            mem.w8(IX + d, L);
            return 19;
        case 0x77:
            // LD (IX+d),A
            d = mem.rs8(PC++);
            mem.w8(IX + d, A);
            return 19;
        case 0x78:
            // LD A,B
            A = B;
            return 8;
        case 0x79:
            // LD A,C
            A = C;
            return 8;
        case 0x7a:
            // LD A,D
            A = D;
            return 8;
        case 0x7b:
            // LD A,E
            A = E;
            return 8;
        case 0x7c:
            // LD A,IXH
            A = IXH;
            return 8;
        case 0x7d:
            // LD A,IXL
            A = IXL;
            return 8;
        case 0x7e:
            // LD A,(IX+d)
            d = mem.rs8(PC++);
            A = mem.r8(IX + d);
            return 19;
        case 0x7f:
            // LD A,A
            A = A;
            return 8;
        case 0x84:
            // ADD IXH
            add8(IXH);
            return 8;
        case 0x85:
            // ADD IXL
            add8(IXL);
            return 8;
        case 0x86:
            // ADD(IX+d)
            d = mem.rs8(PC++);
            add8(mem.r8(IX + d));
            return 19;
        case 0x8c:
            // ADC IXH
            adc8(IXH);
            return 8;
        case 0x8d:
            // ADC IXL
            adc8(IXL);
            return 8;
        case 0x8e:
            // ADC(IX+d)
            d = mem.rs8(PC++);
            adc8(mem.r8(IX + d));
            return 19;
        case 0x94:
            // SUB IXH
            sub8(IXH);
            return 8;
        case 0x95:
            // SUB IXL
            sub8(IXL);
            return 8;
        case 0x96:
            // SUB(IX+d)
            d = mem.rs8(PC++);
            sub8(mem.r8(IX + d));
            return 19;
        case 0x9c:
            // SBC IXH
            sbc8(IXH);
            return 8;
        case 0x9d:
            // SBC IXL
            sbc8(IXL);
            return 8;
        case 0x9e:
            // SBC(IX+d)
            d = mem.rs8(PC++);
            sbc8(mem.r8(IX + d));
            return 19;
        case 0xa4:
            // AND IXH
            and8(IXH);
            return 8;
        case 0xa5:
            // AND IXL
            and8(IXL);
            return 8;
        case 0xa6:
            // AND(IX+d)
            d = mem.rs8(PC++);
            and8(mem.r8(IX + d));
            return 19;
        case 0xac:
            // XOR IXH
            xor8(IXH);
            return 8;
        case 0xad:
            // XOR IXL
            xor8(IXL);
            return 8;
        case 0xae:
            // XOR(IX+d)
            d = mem.rs8(PC++);
            xor8(mem.r8(IX + d));
            return 19;
        case 0xb4:
            // OR IXH
            or8(IXH);
            return 8;
        case 0xb5:
            // OR IXL
            or8(IXL);
            return 8;
        case 0xb6:
            // OR(IX+d)
            d = mem.rs8(PC++);
            or8(mem.r8(IX + d));
            return 19;
        case 0xbc:
            // CP IXH
            cp8(IXH);
            return 8;
        case 0xbd:
            // CP IXL
            cp8(IXL);
            return 8;
        case 0xbe:
            // CP(IX+d)
            d = mem.rs8(PC++);
            cp8(mem.r8(IX + d));
            return 19;
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
        case 0xe1:
            // POP IX
            IX = mem.r16(SP);
            SP += 2;
            return 14;
        case 0xe3:
            // EX (SP),IX
            u16tmp = mem.r16(SP);
            mem.w16(SP, IX);
            IX = u16tmp;
            return 23;
        case 0xe5:
            // PUSH IX
            SP -= 2;
            mem.w16(SP, IX);
            return 15;
        case 0xe9:
            // JP (IX)
            PC = IX;
            return 8;
        case 0xf9:
            // LD SP,IX
            SP = IX;
            return 10;
        default:
            return invalid_opcode(2);
    }
};
inline uint32_t z80::do_op_0xfd(ubyte op) {
    int d; uword u16tmp;
    switch (op) {
        case 0x9:
            // ADD IY,BC
            IY = add16(IY, BC);
            return 15;
        case 0x19:
            // ADD IY,DE
            IY = add16(IY, DE);
            return 15;
        case 0x21:
            // LD IY,nn
            IY = mem.r16(PC);
            PC += 2;
            return 14;
        case 0x22:
            // LD (nn),IY
            mem.w16(mem.r16(PC), IY);
            PC += 2;
            return 20;
        case 0x23:
            // INC IY
            IY++;
            return 10;
        case 0x24:
            // INC IYH
            IYH = inc8(IYH);
            return 8;
        case 0x25:
            // DEC IYH
            IYH = dec8(IYH);
            return 8;
        case 0x26:
            // LD IYH,n
            IYH = mem.r8(PC++);
            return 11;
        case 0x29:
            // ADD IY,IY
            IY = add16(IY, IY);
            return 15;
        case 0x2a:
            // LD IY,(nn)
            IY = mem.r16(mem.r16(PC));
            PC += 2;
            return 20;
        case 0x2b:
            // DEC IY
            IY--;
            return 10;
        case 0x2c:
            // INC IYL
            IYL = inc8(IYL);
            return 8;
        case 0x2d:
            // DEC IYL
            IYL = dec8(IYL);
            return 8;
        case 0x2e:
            // LD IYL,n
            IYL = mem.r8(PC++);
            return 11;
        case 0x34:
            // INC (IY+d)
            d = mem.rs8(PC++);
            mem.w8(IY+d, inc8(mem.r8(IY+d)));
            return 23;
        case 0x35:
            // DEC (IY+d)
            d = mem.rs8(PC++);
            mem.w8(IY+d, dec8(mem.r8(IY+d)));
            return 23;
        case 0x36:
            // LD (IY+d),n
            d = mem.rs8(PC++);
            mem.w8(IY + d, mem.r8(PC++));
            return 19;
        case 0x39:
            // ADD IY,SP
            IY = add16(IY, SP);
            return 15;
        case 0x40:
            // LD B,B
            B = B;
            return 8;
        case 0x41:
            // LD B,C
            B = C;
            return 8;
        case 0x42:
            // LD B,D
            B = D;
            return 8;
        case 0x43:
            // LD B,E
            B = E;
            return 8;
        case 0x44:
            // LD B,IYH
            B = IYH;
            return 8;
        case 0x45:
            // LD B,IYL
            B = IYL;
            return 8;
        case 0x46:
            // LD B,(IY+d)
            d = mem.rs8(PC++);
            B = mem.r8(IY + d);
            return 19;
        case 0x47:
            // LD B,A
            B = A;
            return 8;
        case 0x48:
            // LD C,B
            C = B;
            return 8;
        case 0x49:
            // LD C,C
            C = C;
            return 8;
        case 0x4a:
            // LD C,D
            C = D;
            return 8;
        case 0x4b:
            // LD C,E
            C = E;
            return 8;
        case 0x4c:
            // LD C,IYH
            C = IYH;
            return 8;
        case 0x4d:
            // LD C,IYL
            C = IYL;
            return 8;
        case 0x4e:
            // LD C,(IY+d)
            d = mem.rs8(PC++);
            C = mem.r8(IY + d);
            return 19;
        case 0x4f:
            // LD C,A
            C = A;
            return 8;
        case 0x50:
            // LD D,B
            D = B;
            return 8;
        case 0x51:
            // LD D,C
            D = C;
            return 8;
        case 0x52:
            // LD D,D
            D = D;
            return 8;
        case 0x53:
            // LD D,E
            D = E;
            return 8;
        case 0x54:
            // LD D,IYH
            D = IYH;
            return 8;
        case 0x55:
            // LD D,IYL
            D = IYL;
            return 8;
        case 0x56:
            // LD D,(IY+d)
            d = mem.rs8(PC++);
            D = mem.r8(IY + d);
            return 19;
        case 0x57:
            // LD D,A
            D = A;
            return 8;
        case 0x58:
            // LD E,B
            E = B;
            return 8;
        case 0x59:
            // LD E,C
            E = C;
            return 8;
        case 0x5a:
            // LD E,D
            E = D;
            return 8;
        case 0x5b:
            // LD E,E
            E = E;
            return 8;
        case 0x5c:
            // LD E,IYH
            E = IYH;
            return 8;
        case 0x5d:
            // LD E,IYL
            E = IYL;
            return 8;
        case 0x5e:
            // LD E,(IY+d)
            d = mem.rs8(PC++);
            E = mem.r8(IY + d);
            return 19;
        case 0x5f:
            // LD E,A
            E = A;
            return 8;
        case 0x60:
            // LD IYH,B
            IYH = B;
            return 8;
        case 0x61:
            // LD IYH,C
            IYH = C;
            return 8;
        case 0x62:
            // LD IYH,D
            IYH = D;
            return 8;
        case 0x63:
            // LD IYH,E
            IYH = E;
            return 8;
        case 0x64:
            // LD IYH,IYH
            IYH = IYH;
            return 8;
        case 0x65:
            // LD IYH,IYL
            IYH = IYL;
            return 8;
        case 0x66:
            // LD H,(IY+d)
            d = mem.rs8(PC++);
            H = mem.r8(IY + d);
            return 19;
        case 0x67:
            // LD IYH,A
            IYH = A;
            return 8;
        case 0x68:
            // LD IYL,B
            IYL = B;
            return 8;
        case 0x69:
            // LD IYL,C
            IYL = C;
            return 8;
        case 0x6a:
            // LD IYL,D
            IYL = D;
            return 8;
        case 0x6b:
            // LD IYL,E
            IYL = E;
            return 8;
        case 0x6c:
            // LD IYL,IYH
            IYL = IYH;
            return 8;
        case 0x6d:
            // LD IYL,IYL
            IYL = IYL;
            return 8;
        case 0x6e:
            // LD L,(IY+d)
            d = mem.rs8(PC++);
            L = mem.r8(IY + d);
            return 19;
        case 0x6f:
            // LD IYL,A
            IYL = A;
            return 8;
        case 0x70:
            // LD (IY+d),B
            d = mem.rs8(PC++);
            mem.w8(IY + d, B);
            return 19;
        case 0x71:
            // LD (IY+d),C
            d = mem.rs8(PC++);
            mem.w8(IY + d, C);
            return 19;
        case 0x72:
            // LD (IY+d),D
            d = mem.rs8(PC++);
            mem.w8(IY + d, D);
            return 19;
        case 0x73:
            // LD (IY+d),E
            d = mem.rs8(PC++);
            mem.w8(IY + d, E);
            return 19;
        case 0x74:
            // LD (IY+d),H
            d = mem.rs8(PC++);
            mem.w8(IY + d, H);
            return 19;
        case 0x75:
            // LD (IY+d),L
            d = mem.rs8(PC++);
            mem.w8(IY + d, L);
            return 19;
        case 0x77:
            // LD (IY+d),A
            d = mem.rs8(PC++);
            mem.w8(IY + d, A);
            return 19;
        case 0x78:
            // LD A,B
            A = B;
            return 8;
        case 0x79:
            // LD A,C
            A = C;
            return 8;
        case 0x7a:
            // LD A,D
            A = D;
            return 8;
        case 0x7b:
            // LD A,E
            A = E;
            return 8;
        case 0x7c:
            // LD A,IYH
            A = IYH;
            return 8;
        case 0x7d:
            // LD A,IYL
            A = IYL;
            return 8;
        case 0x7e:
            // LD A,(IY+d)
            d = mem.rs8(PC++);
            A = mem.r8(IY + d);
            return 19;
        case 0x7f:
            // LD A,A
            A = A;
            return 8;
        case 0x84:
            // ADD IYH
            add8(IYH);
            return 8;
        case 0x85:
            // ADD IYL
            add8(IYL);
            return 8;
        case 0x86:
            // ADD(IY+d)
            d = mem.rs8(PC++);
            add8(mem.r8(IY + d));
            return 19;
        case 0x8c:
            // ADC IYH
            adc8(IYH);
            return 8;
        case 0x8d:
            // ADC IYL
            adc8(IYL);
            return 8;
        case 0x8e:
            // ADC(IY+d)
            d = mem.rs8(PC++);
            adc8(mem.r8(IY + d));
            return 19;
        case 0x94:
            // SUB IYH
            sub8(IYH);
            return 8;
        case 0x95:
            // SUB IYL
            sub8(IYL);
            return 8;
        case 0x96:
            // SUB(IY+d)
            d = mem.rs8(PC++);
            sub8(mem.r8(IY + d));
            return 19;
        case 0x9c:
            // SBC IYH
            sbc8(IYH);
            return 8;
        case 0x9d:
            // SBC IYL
            sbc8(IYL);
            return 8;
        case 0x9e:
            // SBC(IY+d)
            d = mem.rs8(PC++);
            sbc8(mem.r8(IY + d));
            return 19;
        case 0xa4:
            // AND IYH
            and8(IYH);
            return 8;
        case 0xa5:
            // AND IYL
            and8(IYL);
            return 8;
        case 0xa6:
            // AND(IY+d)
            d = mem.rs8(PC++);
            and8(mem.r8(IY + d));
            return 19;
        case 0xac:
            // XOR IYH
            xor8(IYH);
            return 8;
        case 0xad:
            // XOR IYL
            xor8(IYL);
            return 8;
        case 0xae:
            // XOR(IY+d)
            d = mem.rs8(PC++);
            xor8(mem.r8(IY + d));
            return 19;
        case 0xb4:
            // OR IYH
            or8(IYH);
            return 8;
        case 0xb5:
            // OR IYL
            or8(IYL);
            return 8;
        case 0xb6:
            // OR(IY+d)
            d = mem.rs8(PC++);
            or8(mem.r8(IY + d));
            return 19;
        case 0xbc:
            // CP IYH
            cp8(IYH);
            return 8;
        case 0xbd:
            // CP IYL
            cp8(IYL);
            return 8;
        case 0xbe:
            // CP(IY+d)
            d = mem.rs8(PC++);
            cp8(mem.r8(IY + d));
            return 19;
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
        case 0xe1:
            // POP IY
            IY = mem.r16(SP);
            SP += 2;
            return 14;
        case 0xe3:
            // EX (SP),IY
            u16tmp = mem.r16(SP);
            mem.w16(SP, IY);
            IY = u16tmp;
            return 23;
        case 0xe5:
            // PUSH IY
            SP -= 2;
            mem.w16(SP, IY);
            return 15;
        case 0xe9:
            // JP (IY)
            PC = IY;
            return 8;
        case 0xf9:
            // LD SP,IY
            SP = IY;
            return 10;
        default:
            return invalid_opcode(2);
    }
};
inline uint32_t z80::do_op_0xed(ubyte op) {
    switch (op) {
        case 0x40:
            // IN B,(C)
            B = in(BC);
            F = szp[B]|(F&CF);
            return 12;
        case 0x41:
            // OUT (C),B
            out(BC, B);
            return 12;
        case 0x42:
            // SBC HL,BC
            HL = sbc16(HL, BC);
            return 15;
        case 0x43:
            // LD (nn),BC
            mem.w16(mem.r16(PC), BC);
            PC += 2;
            return 20;
        case 0x44:
            // NEG
            neg8();
            return 8;
        case 0x46:
            // IM 0
            IM = 0;
            return 8;
        case 0x47:
            // LD I,A
            I = A;
            return 9;
        case 0x48:
            // IN C,(C)
            C = in(BC);
            F = szp[C]|(F&CF);
            return 12;
        case 0x49:
            // OUT (C),C
            out(BC, C);
            return 12;
        case 0x4a:
            // ADC HL,BC
            HL = adc16(HL, BC);
            return 15;
        case 0x4b:
            // LD BC,(nn)
            BC = mem.r16(mem.r16(PC));
            PC += 2;
            return 20;
        case 0x4d:
            // RETI
            reti();
            return 15;
        case 0x4f:
            // LD R,A
            R = A;
            return 9;
        case 0x50:
            // IN D,(C)
            D = in(BC);
            F = szp[D]|(F&CF);
            return 12;
        case 0x51:
            // OUT (C),D
            out(BC, D);
            return 12;
        case 0x52:
            // SBC HL,DE
            HL = sbc16(HL, DE);
            return 15;
        case 0x53:
            // LD (nn),DE
            mem.w16(mem.r16(PC), DE);
            PC += 2;
            return 20;
        case 0x56:
            // IM 1
            IM = 1;
            return 8;
        case 0x57:
            // LD A,I
            A = I;
            F = sziff2(I,IFF2)|(F&CF);
            return 9;
        case 0x58:
            // IN E,(C)
            E = in(BC);
            F = szp[E]|(F&CF);
            return 12;
        case 0x59:
            // OUT (C),E
            out(BC, E);
            return 12;
        case 0x5a:
            // ADC HL,DE
            HL = adc16(HL, DE);
            return 15;
        case 0x5b:
            // LD DE,(nn)
            DE = mem.r16(mem.r16(PC));
            PC += 2;
            return 20;
        case 0x5e:
            // IM 2
            IM = 2;
            return 8;
        case 0x5f:
            // LD A,R
            A = R;
            F = sziff2(R,IFF2)|(F&CF);
            return 9;
        case 0x60:
            // IN H,(C)
            H = in(BC);
            F = szp[H]|(F&CF);
            return 12;
        case 0x61:
            // OUT (C),H
            out(BC, H);
            return 12;
        case 0x62:
            // SBC HL,HL
            HL = sbc16(HL, HL);
            return 15;
        case 0x63:
            // LD (nn),HL
            mem.w16(mem.r16(PC), HL);
            PC += 2;
            return 20;
        case 0x67:
            // RRD
            rrd();
            return 18;
        case 0x68:
            // IN L,(C)
            L = in(BC);
            F = szp[L]|(F&CF);
            return 12;
        case 0x69:
            // OUT (C),L
            out(BC, L);
            return 12;
        case 0x6a:
            // ADC HL,HL
            HL = adc16(HL, HL);
            return 15;
        case 0x6b:
            // LD HL,(nn)
            HL = mem.r16(mem.r16(PC));
            PC += 2;
            return 20;
        case 0x6f:
            // RLD
            rld();
            return 18;
        case 0x72:
            // SBC HL,SP
            HL = sbc16(HL, SP);
            return 15;
        case 0x73:
            // LD (nn),SP
            mem.w16(mem.r16(PC), SP);
            PC += 2;
            return 20;
        case 0x78:
            // IN A,(C)
            A = in(BC);
            F = szp[A]|(F&CF);
            return 12;
        case 0x79:
            // OUT (C),A
            out(BC, A);
            return 12;
        case 0x7a:
            // ADC HL,SP
            HL = adc16(HL, SP);
            return 15;
        case 0x7b:
            // LD SP,(nn)
            SP = mem.r16(mem.r16(PC));
            PC += 2;
            return 20;
        case 0xa0:
            // LDI
            ldi();
            return 16;
        case 0xa1:
            // CPI
            cpi();
            return 16;
        case 0xa2:
            // INI
            ini();
            return 16;
        case 0xa3:
            // OUTI
            outi();
            return 16;
        case 0xa8:
            // LDD
            ldd();
            return 16;
        case 0xa9:
            // CPD
            cpd();
            return 16;
        case 0xaa:
            // IND
            ind();
            return 16;
        case 0xab:
            // OUTD
            outd();
            return 16;
        case 0xb0:
            // LDIR
            return ldir();
        case 0xb1:
            // CPIR
            return cpir();
        case 0xb2:
            // INIR
            return inir();
        case 0xb3:
            // OTIR
            return otir();
        case 0xb8:
            // LDDR
            return lddr();
        case 0xb9:
            // CPDR
            return cpdr();
        case 0xba:
            // INDR
            return indr();
        case 0xbb:
            // OTDR
            return otdr();
        default:
            return invalid_opcode(2);
    }
};
inline uint32_t z80::do_op(ubyte op) {
    uword u16tmp;
    switch (op) {
        case 0x0:
            // NOP
            return 4;
        case 0x1:
            // LD BC,nn
            BC = mem.r16(PC);
            PC += 2;
            return 10;
        case 0x2:
            // LD (BC),A
            mem.w8(BC, A);
            return 7;
        case 0x3:
            // INC BC
            BC++;
            return 6;
        case 0x4:
            // INC B
            B = inc8(B);
            return 4;
        case 0x5:
            // DEC B
            B = dec8(B);
            return 4;
        case 0x6:
            // LD B,n
            B = mem.r8(PC++);
            return 7;
        case 0x7:
            // RLCA
            A = rlc8(A, false);
            return 4;
        case 0x8:
            // EX AF,AF'
            swap16(AF, AF_);
            return 4;
        case 0x9:
            // ADD HL,BC
            HL = add16(HL, BC);
            return 11;
        case 0xa:
            // LD A,(BC)
            A = mem.r8(BC);
            return 7;
        case 0xb:
            // DEC BC
            BC--;
            return 6;
        case 0xc:
            // INC C
            C = inc8(C);
            return 4;
        case 0xd:
            // DEC C
            C = dec8(C);
            return 4;
        case 0xe:
            // LD C,n
            C = mem.r8(PC++);
            return 7;
        case 0xf:
            // RRCA
            A = rrc8(A, false);
            return 4;
        case 0x10:
            // DJNZ e
            if (--B > 0) {
                PC += mem.rs8(PC) + 1;
                return 13;
            }
            else {
                PC++;
                return 8;
            }
        case 0x11:
            // LD DE,nn
            DE = mem.r16(PC);
            PC += 2;
            return 10;
        case 0x12:
            // LD (DE),A
            mem.w8(DE, A);
            return 7;
        case 0x13:
            // INC DE
            DE++;
            return 6;
        case 0x14:
            // INC D
            D = inc8(D);
            return 4;
        case 0x15:
            // DEC D
            D = dec8(D);
            return 4;
        case 0x16:
            // LD D,n
            D = mem.r8(PC++);
            return 7;
        case 0x17:
            // RLA
            A = rl8(A, false);
            return 4;
        case 0x18:
            // JR e
            PC += mem.rs8(PC) + 1;
            return 12;
        case 0x19:
            // ADD HL,DE
            HL = add16(HL, DE);
            return 11;
        case 0x1a:
            // LD A,(DE)
            A = mem.r8(DE);
            return 7;
        case 0x1b:
            // DEC DE
            DE--;
            return 6;
        case 0x1c:
            // INC E
            E = inc8(E);
            return 4;
        case 0x1d:
            // DEC E
            E = dec8(E);
            return 4;
        case 0x1e:
            // LD E,n
            E = mem.r8(PC++);
            return 7;
        case 0x1f:
            // RRA
            A = rr8(A, false);
            return 4;
        case 0x20:
            // JR NZ,e
            if (!(F & ZF)) {
                PC += mem.rs8(PC) + 1;
                return 12;
            }
            else {
                PC++;
                return 7;
            }
        case 0x21:
            // LD HL,nn
            HL = mem.r16(PC);
            PC += 2;
            return 10;
        case 0x22:
            // LD (nn),HL
            mem.w16(mem.r16(PC), HL);
            PC += 2;
            return 16;
        case 0x23:
            // INC HL
            HL++;
            return 6;
        case 0x24:
            // INC H
            H = inc8(H);
            return 4;
        case 0x25:
            // DEC H
            H = dec8(H);
            return 4;
        case 0x26:
            // LD H,n
            H = mem.r8(PC++);
            return 7;
        case 0x27:
            // DAA
            daa();
            return 4;
        case 0x28:
            // JR Z,e
            if (F & ZF) {
                PC += mem.rs8(PC) + 1;
                return 12;
            }
            else {
                PC++;
                return 7;
            }
        case 0x29:
            // ADD HL,HL
            HL = add16(HL, HL);
            return 11;
        case 0x2a:
            // LD HL,(nn)
            HL = mem.r16(mem.r16(PC));
            PC += 2;
            return 16;
        case 0x2b:
            // DEC HL
            HL--;
            return 6;
        case 0x2c:
            // INC L
            L = inc8(L);
            return 4;
        case 0x2d:
            // DEC L
            L = dec8(L);
            return 4;
        case 0x2e:
            // LD L,n
            L = mem.r8(PC++);
            return 7;
        case 0x2f:
            // CPL
            A ^= 0xFF;
            F = (F&(SF|ZF|PF|CF)) | HF | NF | (A&(YF|XF));
            return 4;
        case 0x30:
            // JR NC,e
            if (!(F & CF)) {
                PC += mem.rs8(PC) + 1;
                return 12;
            }
            else {
                PC++;
                return 7;
            }
        case 0x31:
            // LD SP,nn
            SP = mem.r16(PC);
            PC += 2;
            return 10;
        case 0x32:
            // LD (nn),A
            mem.w8(mem.r16(PC), A);
            PC += 2;
            return 13;
        case 0x33:
            // INC SP
            SP++;
            return 6;
        case 0x34:
            // INC (HL)
            mem.w8(HL, inc8(mem.r8(HL)));
            return 11;
        case 0x35:
            // DEC (HL)
            mem.w8(HL, dec8(mem.r8(HL)));
            return 11;
        case 0x36:
            // LD (HL),n
            mem.w8(HL, mem.r8(PC++));
            return 10;
        case 0x37:
            // SCF
            F = (F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF));
            return 4;
        case 0x38:
            // JR C,e
            if (F & CF) {
                PC += mem.rs8(PC) + 1;
                return 12;
            }
            else {
                PC++;
                return 7;
            }
        case 0x39:
            // ADD HL,SP
            HL = add16(HL, SP);
            return 11;
        case 0x3a:
            // LD A,(nn)
            A = mem.r8(mem.r16(PC));
            PC += 2;
            return 13;
        case 0x3b:
            // DEC SP
            SP--;
            return 6;
        case 0x3c:
            // INC A
            A = inc8(A);
            return 4;
        case 0x3d:
            // DEC A
            A = dec8(A);
            return 4;
        case 0x3e:
            // LD A,n
            A = mem.r8(PC++);
            return 7;
        case 0x3f:
            // CCF
            F = ((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF;
            return 4;
        case 0x40:
            // LD B,B
            B = B;
            return 4;
        case 0x41:
            // LD B,C
            B = C;
            return 4;
        case 0x42:
            // LD B,D
            B = D;
            return 4;
        case 0x43:
            // LD B,E
            B = E;
            return 4;
        case 0x44:
            // LD B,H
            B = H;
            return 4;
        case 0x45:
            // LD B,L
            B = L;
            return 4;
        case 0x46:
            // LD B,(HL)
            B = mem.r8(HL);
            return 7;
        case 0x47:
            // LD B,A
            B = A;
            return 4;
        case 0x48:
            // LD C,B
            C = B;
            return 4;
        case 0x49:
            // LD C,C
            C = C;
            return 4;
        case 0x4a:
            // LD C,D
            C = D;
            return 4;
        case 0x4b:
            // LD C,E
            C = E;
            return 4;
        case 0x4c:
            // LD C,H
            C = H;
            return 4;
        case 0x4d:
            // LD C,L
            C = L;
            return 4;
        case 0x4e:
            // LD C,(HL)
            C = mem.r8(HL);
            return 7;
        case 0x4f:
            // LD C,A
            C = A;
            return 4;
        case 0x50:
            // LD D,B
            D = B;
            return 4;
        case 0x51:
            // LD D,C
            D = C;
            return 4;
        case 0x52:
            // LD D,D
            D = D;
            return 4;
        case 0x53:
            // LD D,E
            D = E;
            return 4;
        case 0x54:
            // LD D,H
            D = H;
            return 4;
        case 0x55:
            // LD D,L
            D = L;
            return 4;
        case 0x56:
            // LD D,(HL)
            D = mem.r8(HL);
            return 7;
        case 0x57:
            // LD D,A
            D = A;
            return 4;
        case 0x58:
            // LD E,B
            E = B;
            return 4;
        case 0x59:
            // LD E,C
            E = C;
            return 4;
        case 0x5a:
            // LD E,D
            E = D;
            return 4;
        case 0x5b:
            // LD E,E
            E = E;
            return 4;
        case 0x5c:
            // LD E,H
            E = H;
            return 4;
        case 0x5d:
            // LD E,L
            E = L;
            return 4;
        case 0x5e:
            // LD E,(HL)
            E = mem.r8(HL);
            return 7;
        case 0x5f:
            // LD E,A
            E = A;
            return 4;
        case 0x60:
            // LD H,B
            H = B;
            return 4;
        case 0x61:
            // LD H,C
            H = C;
            return 4;
        case 0x62:
            // LD H,D
            H = D;
            return 4;
        case 0x63:
            // LD H,E
            H = E;
            return 4;
        case 0x64:
            // LD H,H
            H = H;
            return 4;
        case 0x65:
            // LD H,L
            H = L;
            return 4;
        case 0x66:
            // LD H,(HL)
            H = mem.r8(HL);
            return 7;
        case 0x67:
            // LD H,A
            H = A;
            return 4;
        case 0x68:
            // LD L,B
            L = B;
            return 4;
        case 0x69:
            // LD L,C
            L = C;
            return 4;
        case 0x6a:
            // LD L,D
            L = D;
            return 4;
        case 0x6b:
            // LD L,E
            L = E;
            return 4;
        case 0x6c:
            // LD L,H
            L = H;
            return 4;
        case 0x6d:
            // LD L,L
            L = L;
            return 4;
        case 0x6e:
            // LD L,(HL)
            L = mem.r8(HL);
            return 7;
        case 0x6f:
            // LD L,A
            L = A;
            return 4;
        case 0x70:
            // LD (HL),B
            mem.w8(HL, B);
            return 7;
        case 0x71:
            // LD (HL),C
            mem.w8(HL, C);
            return 7;
        case 0x72:
            // LD (HL),D
            mem.w8(HL, D);
            return 7;
        case 0x73:
            // LD (HL),E
            mem.w8(HL, E);
            return 7;
        case 0x74:
            // LD (HL),H
            mem.w8(HL, H);
            return 7;
        case 0x75:
            // LD (HL),L
            mem.w8(HL, L);
            return 7;
        case 0x76:
            // HALT
            halt();
            return 4;
        case 0x77:
            // LD (HL),A
            mem.w8(HL, A);
            return 7;
        case 0x78:
            // LD A,B
            A = B;
            return 4;
        case 0x79:
            // LD A,C
            A = C;
            return 4;
        case 0x7a:
            // LD A,D
            A = D;
            return 4;
        case 0x7b:
            // LD A,E
            A = E;
            return 4;
        case 0x7c:
            // LD A,H
            A = H;
            return 4;
        case 0x7d:
            // LD A,L
            A = L;
            return 4;
        case 0x7e:
            // LD A,(HL)
            A = mem.r8(HL);
            return 7;
        case 0x7f:
            // LD A,A
            A = A;
            return 4;
        case 0x80:
            // ADD B
            add8(B);
            return 4;
        case 0x81:
            // ADD C
            add8(C);
            return 4;
        case 0x82:
            // ADD D
            add8(D);
            return 4;
        case 0x83:
            // ADD E
            add8(E);
            return 4;
        case 0x84:
            // ADD H
            add8(H);
            return 4;
        case 0x85:
            // ADD L
            add8(L);
            return 4;
        case 0x86:
            // ADD (HL)
            add8(mem.r8(HL));
            return 7;
        case 0x87:
            // ADD A
            add8(A);
            return 4;
        case 0x88:
            // ADC B
            adc8(B);
            return 4;
        case 0x89:
            // ADC C
            adc8(C);
            return 4;
        case 0x8a:
            // ADC D
            adc8(D);
            return 4;
        case 0x8b:
            // ADC E
            adc8(E);
            return 4;
        case 0x8c:
            // ADC H
            adc8(H);
            return 4;
        case 0x8d:
            // ADC L
            adc8(L);
            return 4;
        case 0x8e:
            // ADC (HL)
            adc8(mem.r8(HL));
            return 7;
        case 0x8f:
            // ADC A
            adc8(A);
            return 4;
        case 0x90:
            // SUB B
            sub8(B);
            return 4;
        case 0x91:
            // SUB C
            sub8(C);
            return 4;
        case 0x92:
            // SUB D
            sub8(D);
            return 4;
        case 0x93:
            // SUB E
            sub8(E);
            return 4;
        case 0x94:
            // SUB H
            sub8(H);
            return 4;
        case 0x95:
            // SUB L
            sub8(L);
            return 4;
        case 0x96:
            // SUB (HL)
            sub8(mem.r8(HL));
            return 7;
        case 0x97:
            // SUB A
            sub8(A);
            return 4;
        case 0x98:
            // SBC B
            sbc8(B);
            return 4;
        case 0x99:
            // SBC C
            sbc8(C);
            return 4;
        case 0x9a:
            // SBC D
            sbc8(D);
            return 4;
        case 0x9b:
            // SBC E
            sbc8(E);
            return 4;
        case 0x9c:
            // SBC H
            sbc8(H);
            return 4;
        case 0x9d:
            // SBC L
            sbc8(L);
            return 4;
        case 0x9e:
            // SBC (HL)
            sbc8(mem.r8(HL));
            return 7;
        case 0x9f:
            // SBC A
            sbc8(A);
            return 4;
        case 0xa0:
            // AND B
            and8(B);
            return 4;
        case 0xa1:
            // AND C
            and8(C);
            return 4;
        case 0xa2:
            // AND D
            and8(D);
            return 4;
        case 0xa3:
            // AND E
            and8(E);
            return 4;
        case 0xa4:
            // AND H
            and8(H);
            return 4;
        case 0xa5:
            // AND L
            and8(L);
            return 4;
        case 0xa6:
            // AND (HL)
            and8(mem.r8(HL));
            return 7;
        case 0xa7:
            // AND A
            and8(A);
            return 4;
        case 0xa8:
            // XOR B
            xor8(B);
            return 4;
        case 0xa9:
            // XOR C
            xor8(C);
            return 4;
        case 0xaa:
            // XOR D
            xor8(D);
            return 4;
        case 0xab:
            // XOR E
            xor8(E);
            return 4;
        case 0xac:
            // XOR H
            xor8(H);
            return 4;
        case 0xad:
            // XOR L
            xor8(L);
            return 4;
        case 0xae:
            // XOR (HL)
            xor8(mem.r8(HL));
            return 7;
        case 0xaf:
            // XOR A
            xor8(A);
            return 4;
        case 0xb0:
            // OR B
            or8(B);
            return 4;
        case 0xb1:
            // OR C
            or8(C);
            return 4;
        case 0xb2:
            // OR D
            or8(D);
            return 4;
        case 0xb3:
            // OR E
            or8(E);
            return 4;
        case 0xb4:
            // OR H
            or8(H);
            return 4;
        case 0xb5:
            // OR L
            or8(L);
            return 4;
        case 0xb6:
            // OR (HL)
            or8(mem.r8(HL));
            return 7;
        case 0xb7:
            // OR A
            or8(A);
            return 4;
        case 0xb8:
            // CP B
            cp8(B);
            return 4;
        case 0xb9:
            // CP C
            cp8(C);
            return 4;
        case 0xba:
            // CP D
            cp8(D);
            return 4;
        case 0xbb:
            // CP E
            cp8(E);
            return 4;
        case 0xbc:
            // CP H
            cp8(H);
            return 4;
        case 0xbd:
            // CP L
            cp8(L);
            return 4;
        case 0xbe:
            // CP (HL)
            cp8(mem.r8(HL));
            return 7;
        case 0xbf:
            // CP A
            cp8(A);
            return 4;
        case 0xc0:
            // RET NZ
            if ((!(F & ZF))) {
                PC = mem.r16(SP);
                SP += 2;
                return 11;
            }
            else {
                return 5;
            }
        case 0xc1:
            // POP BC
            BC = mem.r16(SP);
            SP += 2;
            return 10;
        case 0xc2:
            // JP NZ,nn
            PC = (!(F & ZF)) ? mem.r16(PC) : PC+2;
            return 10;
        case 0xc3:
            // JP nn
            PC = mem.r16(PC);
            return 10;
        case 0xc4:
            // CALL NZ,nn
            if ((!(F & ZF))) {
                SP -= 2;
                mem.w16(SP, PC+2);
                PC = mem.r16(PC);
                return 17;
            }
            else {
                PC += 2;
                return 10;
            }
        case 0xc5:
            // PUSH BC
            SP -= 2;
            mem.w16(SP, BC);
            return 11;
        case 0xc6:
            // ADD n
            add8(mem.r8(PC++));
            return 7;
        case 0xc7:
            // RST 0
            rst(0);
            return 11;
        case 0xc8:
            // RET Z
            if ((F & ZF)) {
                PC = mem.r16(SP);
                SP += 2;
                return 11;
            }
            else {
                return 5;
            }
        case 0xc9:
            // RET
            PC = mem.r16(SP);
            SP += 2;
            return 10;
        case 0xca:
            // JP Z,nn
            PC = (F & ZF) ? mem.r16(PC) : PC+2;
            return 10;
        case 0xcb:
            return do_op_0xcb(fetch_op());
        case 0xcc:
            // CALL Z,nn
            if ((F & ZF)) {
                SP -= 2;
                mem.w16(SP, PC+2);
                PC = mem.r16(PC);
                return 17;
            }
            else {
                PC += 2;
                return 10;
            }
        case 0xcd:
            // CALL nn
            SP -= 2;
            mem.w16(SP, PC+2);
            PC = mem.r16(PC);
            return 17;
        case 0xce:
            // ADC n
            adc8(mem.r8(PC++));
            return 7;
        case 0xcf:
            // RST 1
            rst(8);
            return 11;
        case 0xd0:
            // RET NC
            if ((!(F & CF))) {
                PC = mem.r16(SP);
                SP += 2;
                return 11;
            }
            else {
                return 5;
            }
        case 0xd1:
            // POP DE
            DE = mem.r16(SP);
            SP += 2;
            return 10;
        case 0xd2:
            // JP NC,nn
            PC = (!(F & CF)) ? mem.r16(PC) : PC+2;
            return 10;
        case 0xd3:
            // OUT (n),A
            out((A<<8)|mem.r8(PC++),A);
            return 11;
        case 0xd4:
            // CALL NC,nn
            if ((!(F & CF))) {
                SP -= 2;
                mem.w16(SP, PC+2);
                PC = mem.r16(PC);
                return 17;
            }
            else {
                PC += 2;
                return 10;
            }
        case 0xd5:
            // PUSH DE
            SP -= 2;
            mem.w16(SP, DE);
            return 11;
        case 0xd6:
            // SUB n
            sub8(mem.r8(PC++));
            return 7;
        case 0xd7:
            // RST 2
            rst(16);
            return 11;
        case 0xd8:
            // RET C
            if ((F & CF)) {
                PC = mem.r16(SP);
                SP += 2;
                return 11;
            }
            else {
                return 5;
            }
        case 0xd9:
            // EXX
            swap16(BC, BC_);
            swap16(DE, DE_);
            swap16(HL, HL_);
            return 4;
        case 0xda:
            // JP C,nn
            PC = (F & CF) ? mem.r16(PC) : PC+2;
            return 10;
        case 0xdb:
            // IN A,(n)
            A = in((A<<8)|(mem.r8(PC++)));
            return 11;
        case 0xdc:
            // CALL C,nn
            if ((F & CF)) {
                SP -= 2;
                mem.w16(SP, PC+2);
                PC = mem.r16(PC);
                return 17;
            }
            else {
                PC += 2;
                return 10;
            }
        case 0xdd:
            return do_op_0xdd(fetch_op());
        case 0xde:
            // SBC n
            sbc8(mem.r8(PC++));
            return 7;
        case 0xdf:
            // RST 3
            rst(24);
            return 11;
        case 0xe0:
            // RET PO
            if ((!(F & PF))) {
                PC = mem.r16(SP);
                SP += 2;
                return 11;
            }
            else {
                return 5;
            }
        case 0xe1:
            // POP HL
            HL = mem.r16(SP);
            SP += 2;
            return 10;
        case 0xe2:
            // JP PO,nn
            PC = (!(F & PF)) ? mem.r16(PC) : PC+2;
            return 10;
        case 0xe3:
            // EX (SP),HL
            u16tmp = mem.r16(SP);
            mem.w16(SP, HL);
            HL = u16tmp;
            return 19;
        case 0xe4:
            // CALL PO,nn
            if ((!(F & PF))) {
                SP -= 2;
                mem.w16(SP, PC+2);
                PC = mem.r16(PC);
                return 17;
            }
            else {
                PC += 2;
                return 10;
            }
        case 0xe5:
            // PUSH HL
            SP -= 2;
            mem.w16(SP, HL);
            return 11;
        case 0xe6:
            // AND n
            and8(mem.r8(PC++));
            return 7;
        case 0xe7:
            // RST 4
            rst(32);
            return 11;
        case 0xe8:
            // RET PE
            if ((F & PF)) {
                PC = mem.r16(SP);
                SP += 2;
                return 11;
            }
            else {
                return 5;
            }
        case 0xe9:
            // JP (HL)
            PC = HL;
            return 4;
        case 0xea:
            // JP PE,nn
            PC = (F & PF) ? mem.r16(PC) : PC+2;
            return 10;
        case 0xeb:
            // EX DE,HL
            swap16(DE, HL);
            return 4;
        case 0xec:
            // CALL PE,nn
            if ((F & PF)) {
                SP -= 2;
                mem.w16(SP, PC+2);
                PC = mem.r16(PC);
                return 17;
            }
            else {
                PC += 2;
                return 10;
            }
        case 0xed:
            return do_op_0xed(fetch_op());
        case 0xee:
            // XOR n
            xor8(mem.r8(PC++));
            return 7;
        case 0xef:
            // RST 5
            rst(40);
            return 11;
        case 0xf0:
            // RET P
            if ((!(F & SF))) {
                PC = mem.r16(SP);
                SP += 2;
                return 11;
            }
            else {
                return 5;
            }
        case 0xf1:
            // POP AF
            AF = mem.r16(SP);
            SP += 2;
            return 10;
        case 0xf2:
            // JP P,nn
            PC = (!(F & SF)) ? mem.r16(PC) : PC+2;
            return 10;
        case 0xf3:
            // DI
            di();
            return 4;
        case 0xf4:
            // CALL P,nn
            if ((!(F & SF))) {
                SP -= 2;
                mem.w16(SP, PC+2);
                PC = mem.r16(PC);
                return 17;
            }
            else {
                PC += 2;
                return 10;
            }
        case 0xf5:
            // PUSH AF
            SP -= 2;
            mem.w16(SP, AF);
            return 11;
        case 0xf6:
            // OR n
            or8(mem.r8(PC++));
            return 7;
        case 0xf7:
            // RST 6
            rst(48);
            return 11;
        case 0xf8:
            // RET M
            if ((F & SF)) {
                PC = mem.r16(SP);
                SP += 2;
                return 11;
            }
            else {
                return 5;
            }
        case 0xf9:
            // LD SP,HL
            SP = HL;
            return 6;
        case 0xfa:
            // JP M,nn
            PC = (F & SF) ? mem.r16(PC) : PC+2;
            return 10;
        case 0xfb:
            // EI
            ei();
            return 4;
        case 0xfc:
            // CALL M,nn
            if ((F & SF)) {
                SP -= 2;
                mem.w16(SP, PC+2);
                PC = mem.r16(PC);
                return 17;
            }
            else {
                PC += 2;
                return 10;
            }
        case 0xfd:
            return do_op_0xfd(fetch_op());
        case 0xfe:
            // CP n
            cp8(mem.r8(PC++));
            return 7;
        case 0xff:
            // RST 7
            rst(56);
            return 11;
        default:
            return invalid_opcode(1);
    }
};
} // namespace yakc
