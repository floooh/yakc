#-------------------------------------------------------------------------------
#   z80_cycles.py
#   Generate Z80 instruction cycle count tables.
#
#   This is structured like the z80_opcodes.py script which generates
#   the instruction decoder code, but instead of generating decoder
#   code, cycle-count tables for instructions are generated.
#
#   The cycle count info is created by defining the length and what
#   happens in each machine cycle of an instruction.
#
#   See: 
#       http://www.z80.info/z80ins.txt
#-------------------------------------------------------------------------------

# fips code generator version stamp
Version = 20

# tab-width for generated code
TabWidth = 2

# the target file handle
Out = None

# 8-bit register table, the 'HL' entry is for instructions that use
# (HL), (IX+d) and (IY+d), and will be patched to 'IX' or 'IY' for
# the DD/FD prefix instructions
r = [ 'B', 'C', 'D', 'E', 'H', 'L', 'HL', 'A' ]

# the same, but the 'HL' item is never patched to IX/IY, this is
# for indexed instructions that load into H or L (e.g. LD H,(IX+d))
r2 = [ 'B', 'C', 'D', 'E', 'H', 'L', 'HL', 'A' ]

# 16-bit register table, with SP
rp = [ 'BC', 'DE', 'HL', 'SP' ]

# 16-bit register table, with AF (only used for PUSH/POP)
rp2 = [ 'BC', 'DE', 'HL', 'AF' ]

# condition-code table (for conditional jumps etc)
cond = [
#   NZ         Z         NC         C         PO         PE        P          M
    '!(F&ZF)', '(F&ZF)', '!(F&CF)', '(F&CF)', '!(F&PF)', '(F&PF)', '!(F&SF)', '(F&SF)'
]

# the same as 'human readable' flags for comments
cond_cmt = [ 'NZ', 'Z', 'NC', 'C', 'PO', 'PE', 'P', 'M' ]

# 8-bit ALU instruction table (C++ method names)
alu = [ 'add8', 'adc8', 'sub8', 'sbc8', 'and8', 'xor8', 'or8', 'cp8' ]

# the same 'human readable' for comments
alu_cmt = [ 'ADD', 'ADC', 'SUB', 'SBC', 'AND', 'XOR', 'OR', 'CP' ]

# rot and shift instruction table (C++ method names)
rot = [ 'rlc8', 'rrc8', 'rl8', 'rr8', 'sla8', 'sra8', 'sll8', 'srl8' ]

# the same 'human readbla for comments
rot_cmt = [ 'RLC', 'RRC', 'RL', 'RR', 'SLA', 'SRA', 'SLL', 'SRL' ]

import sys
import genutil  # fips code generator helpers

#-------------------------------------------------------------------------------
# return comment string for (HL), (IX+d), (IY+d)
#
def iHLcmt(ext) :
    if (ext) :
        return '({}+d)'.format(r[6])
    else :
        return '({})'.format(r[6])

#-------------------------------------------------------------------------------
class op_info:
    def __init__(self, op):
        if op:
            self.count = op.count
            self.excount = op.excount
            self.info = op.info
        else:
            self.count = 0
            self.excount = 0
            self.info = ''
        self.cmd = ''
    def mc(self, type, num):
        self.count += num
        self.info += '{}({}) '.format(type, num)
    def mc_ex(self, type, num):
        self.excount += num
        self.info += '{}_ex({}) '.format(type, num)
    def ocf(self, num):
        # opcode fetch
        self.mc('OCF', num)
    def od(self, num):
        # opcode data read (single byte)
        self.mc('OD', num)
    def odl(self, num):
        # opcode data read (low byte)
        self.mc('ODL', num)
    def odh(self, num):
        # opcode data read (high byte)
        self.mc('ODH', num)
    def io(self, num):
        # general CPU operation
        self.mc('IO', num)
    def io_ex(self, num):
        # general CPU operation, conditional
        self.mc_ex('IO', num)
    def mr(self, num):
        # memory read (single byte)
        self.mc('MR', num)
    def mrh(self, num):
        # memory read (high byte)
        self.mc('MRH', num)
    def mrl(self, num):
        # memory read (low byte)
        self.mc('MRL', num)
    def mw(self, num):
        # memory write (single byte)
        self.mc('MW', num)
    def mwh(self, num):
        # memory write (high byte)
        self.mc('MWH', num)
    def mwl(self, num):
        # memory write (low byte)
        self.mc('MWL', num)
    def pr(self, num):
        # port read
        self.mc('PR', num)
    def pw(self, num):
        # port write
        self.mc('PW', num)
    def srh(self, num):
        # stack read of high byte
        self.mc('SRH', num)
    def srh_ex(self, num):
        # stack read of high byte, conditional
        self.mc_ex('SRH', num)
    def srl(self, num):
        # stack read of low byte
        self.mc('SRL', num)
    def srl_ex(self, num):
        # stack read of low byte, conditional
        self.mc_ex('SRL', num)
    def swh(self, num):
        # stack write of high byte
        self.mc('SWH', num)
    def swh_ex(self, num):
        # stack write of high byte, conditional
        self.mc_ex('SWH', num)
    def swl(self, num):
        # stack write of low byte
        self.mc('SWL', num)
    def swl_ex(self, num):
        # stack write of low byte, conditional
        self.mc_ex('SWL', num)
    def cm(self, s):
        # command mnemonics
        self.cmd = s

#-------------------------------------------------------------------------------
# Generate a 'cycles' object for a main-, DD- or FD-prefix-instructions.
#
def enc(val, ext, in_op) :

    if in_op:
        op = op_info(in_op)
    else:
        op = op_info(None)

    # split opcode byte into bit groups, these identify groups
    # or subgroups of instructions, or serve as register indices 
    #
    #   |xx|yyy|zzz|
    #   |xx|pp|q|zzz|
    x = val>>6
    y = (val>>3)&7
    z = val&7
    p = y>>1
    q = y&1

    #---- block 1: 8-bit loads, and HALT
    if x == 1:
        if y == 6:
            if z == 6:
                # special case: LD (HL),(HL) is HALT
                op.cm('HALT'); 
                op.ocf(4)
            else:
                # LD (HL),r; LD (IX+d),r; LD (IX+d),r
                op.cm('LD {},{}'.format(iHLcmt(ext), r2[z]))
                op.ocf(4)
                if ext:
                    op.od(3); op.io(5)
                op.mw(3)
        elif z == 6:
            # LD r,(HL); LD r,(IX+d); LD r,(IY+d)
            op.cm('LD {},{}'.format(r2[y], iHLcmt(ext)))
            op.ocf(4)
            if ext:
                op.od(3); op.io(5)
            op.mr(3)
        else:
            # LD r,s
            op.cm('LD {},{}'.format(r[y], r[z]))
            op.ocf(4)

    #---- block 2: 8-bit ALU instructions (ADD, ADC, SUB, SBC, AND, XOR, OR, CP)
    elif x == 2:
        if z == 6:
            # ALU (HL); ALU (IX+d); ALU (IY+d)
            op.cm('{} {}'.format(alu_cmt[y], iHLcmt(ext)))
            op.ocf(4)
            if ext: 
                op.od(3); op.io(5)
            op.mr(3)
        else:
            # ALU r
            op.cm('{} {}'.format(alu_cmt[y], r[z]))
            op.ocf(4)

    #---- block 0: misc ops
    elif x == 0:
        if z == 0:
            if y == 0:
                # NOP
                op.cm('NOP'); 
                op.ocf(4)
            elif y == 1:
                # EX AF,AF'
                op.cm("EX AF,AF'"); 
                op.ocf(4)
            elif y == 2:
                # DJNZ d
                op.cm('DJNZ'); 
                op.ocf(5); op.od(3); op.io_ex(5)
            elif  y == 3:
                # JR d
                op.cm('JR d'); 
                op.ocf(4); op.od(3); op.io(5)
            else:
                # JR cc,d
                op.cm('JR {},d'.format(cond_cmt[y-4]))
                op.ocf(4); op.od(3); op.io_ex(5)
        elif z == 1:
            if q == 0:
                # 16-bit immediate loads
                op.cm('LD {},nn'.format(rp[p]))
                op.ocf(4); op.odl(3); op.odh(3)
            else :
                # ADD HL,rr; ADD IX,rr; ADD IY,rr
                op.cm('ADD {},{}'.format(rp[2],rp[p]))
                op.ocf(4); op.io(4); op.io(3)
        elif z == 2:
            # indirect loads
            if y == 0:
                op.cm('LD (BC),A')
                op.ocf(4); op.mw(3)
            elif y == 1:
                op.cm('LD A,(BC)')
                op.ocf(4); op.mr(3)
            elif y == 2:
                op.cm('LD (DE),A')
                op.ocf(4); op.mw(3)
            elif y == 3:
                op.cm('LD A,(DE)')
                op.ocf(4); op.mr(3)
            elif y == 4:
                # LD (nn),HL; LD (nn),IX; LD (nn),IY
                op.cm('LD (nn),{}'.format(rp[2]))
                op.ocf(4); op.odl(3); op.odh(3); op.mwl(3); op.mwh(3)
            elif y == 5:
                # LD HL,(nn); LD IX,(nn); LD IY,(nn)
                op.cm('LD {},(nn)'.format(rp[2]))
                op.ocf(4); op.odl(3); op.odh(3); op.mrl(3); op.mrh(3)
            elif y == 6:
                op.cm('LD (nn),A')
                op.ocf(4); op.odl(3); op.odh(3); op.mw(3)
            elif y == 7:
                op.cm('LD A,(nn)')
                op.ocf(4); op.odl(3); op.odh(3); op.mr(3)
        elif z == 3:
            # 16-bit INC/DEC 
            if q == 0:
                op.cm('INC {}'.format(rp[p]))
                op.ocf(6)
            else:
                op.cm('DEC {}'.format(rp[p]))
                op.ocf(6)
        elif z == 4 or z == 5:
            cmt = 'INC' if z == 4 else 'DEC'
            if y == 6:
                # INC/DEC (HL)/(IX+d)/(IY+d)
                op.cm('{} {}'.format(cmt, iHLcmt(ext)))
                if ext:
                    op.ocf(4); op.od(3); op.io(5); op.mr(4); op.mw(3)
                else:
                    op.ocf(4); op.mr(4); op.mw(3)
            else:
                # INC/DEC r
                op.cm('{} {}'.format(cmt, r[y]))
                op.ocf(4)
        elif z == 6:
            if y == 6:
                # LD (HL),n; LD (IX+d),n; LD (IY+d),n
                op.cm('LD {},n'.format(iHLcmt(ext)))
                if ext:
                    op.ocf(4); op.od(3); op.io(5); op.mw(3)
                else:
                    op.ocf(4); op.od(3); op.mw(3)
            else:
                # LD r,n
                op.cm('LD {},n'.format(r[y]))
                op.ocf(4); op.od(3)
        elif z == 7:
            # misc ops on A and F
            if y == 0:
                op.cm('RLCA')
            elif y == 1:
                op.cm('RRCA')
            elif y == 2:
                op.cm('RLA')
            elif y == 3:
                op.cm('RRA')
            elif y == 4:
                op.cm('DAA')
            elif y == 5:
                op.cm('CPL')
            elif y == 6:
                op.cm('SCF')
            elif y == 7:
                op.cm('CCF')
            op.ocf(4)

    #--- block 3: misc and extended ops
    elif x == 3:
        if z == 0:
            # RET cc
            op.cm('RET {}'.format(cond_cmt[y]))
            op.ocf(5); op.srl_ex(3); op.srh_ex(3)
        elif z == 1:
            if q == 0:
                # POP BC,DE,HL,IX,IY,AF
                op.cm('POP {}'.format(rp2[p]))
                op.ocf(4); op.srl(3); op.srh(3)
            else:
                # misc ops
                if p == 0:
                    op.cm('RET')
                    op.ocf(4); op.srl(3); op.srh(3)
                elif p == 1:
                    op.cm('EXX')
                    op.ocf(4)
                elif p == 2:
                    # JP HL; JP IX; JP IY
                    op.cm('JP {}'.format(rp[2]))
                    op.ocf(4)
                elif p == 3:
                    # LD SP,HL; LD SP,IX; LD SP,IY
                    op.cm('LD SP,{}'.format(rp[2]))
                    op.ocf(6)
        elif z == 2:
            # JP cc,nn
            op.cm('JP {},nn'.format(cond_cmt[y]))
            op.ocf(4); op.odl(3); op.odh(3)
        elif z == 3:
            # misc ops
            if y == 0:
                op.cm('JP nn')
                op.ocf(4); op.odl(3); op.odh(3)
            elif y == 1:
                # CB prefix instructions (just 4 tstates for loading CB prefix)
                op.ocf(4)
            elif y == 2:
                op.cm('OUT (n),A')
                op.ocf(4); op.od(3); op.pw(4)
            elif y == 3:
                op.cm('IN A,(n)')
                op.ocf(4); op.od(3); op.pr(4)
            elif y == 4:
                # EX (SP),HL; EX (SP),IX; EX (SP),IY
                op.cm('EX (SP),{}'.format(rp[2]))
                op.ocf(4); op.srl(3); op.srh(4); op.swh(3); op.swl(5)
            elif y == 5:
                op.cm('EX DE,HL')
                op.ocf(4)
            elif y == 6:
                op.cm('DI')
                op.ocf(4)
            elif y == 7:
                op.cm('EI')
                op.ocf(4)
        elif z == 4:
            # CALL cc,nn
            op.cm('CALL {},nn'.format(cond_cmt[y]))
            # note this is not quite correct, right would be:
            #   cc true:  ocf(4); odl(3); odh(4); swh(3); swl(3)
            #   cc false: ocf(4); odl(3); odh(3)
            # but since we can only add additional cycles for the true
            # condition, we move one tstate from odh() to swh()
            op.ocf(4); op.odl(3); op.odh(3); op.swh_ex(4); op.swl_ex(3)
        elif z == 5:
            if q == 0:
                # PUSH BC,DE,HL,IX,IY,AF
                op.cm('PUSH {}'.format(rp2[p]))
                op.ocf(5); op.swh(3); op.swl(3)
            else:
                if p == 0:
                    op.cm('CALL nn')
                    op.ocf(4); op.odl(3); op.odh(4); op.swh(3); op.swl(3)
                else: # p==1 or p==2 or p==3
                    # DD, ED or FD prefix ops, just add 4 tstates for reading prefix
                    op.ocf(4)
        elif z == 6:
            # ALU n
            op.cm('{} n'.format(alu_cmt[y]))
            op.ocf(4); op.od(3)
        elif z == 7:
            # RST
            op.cm('RST {}'.format(hex(y*8)))
            op.ocf(5); op.swh(3); op.swl(3)
    return op

#-------------------------------------------------------------------------------
#   ED prefix instructions
#
def enc_ed(val, in_op) :
    op = op_info(in_op)
    x = val>>6
    y = (val>>3)&7
    z = val&7
    p = y>>1
    q = y&1

    if x == 2:
        # block instructions (LDIR etc)
        if z == 0:
            if y == 4:
                op.cm('LDI')
                op.ocf(4); op.mr(3); op.mw(5);
            elif y == 5:
                op.cm('LDD')
                op.ocf(4); op.mr(3); op.mw(5)
            elif y == 6:
                op.cm('LDDIR')
                op.ocf(4); op.mr(3); op.mw(5); op.io_ex(5)
            elif y == 7:
                op.cm('LDDR')
                op.ocf(4); op.mr(3); op.mw(5); op.io_ex(5)
            else:
                op.cm('INV (ED)')
                op.ocf(4)
        elif z == 1:
            if y == 4:
                op.cm('CPI')
                op.ocf(4); op.mr(3); op.mw(5)
            elif y == 5:
                op.cm('CPD')
                op.ocf(4); op.mr(3); op.mw(5)
            elif y == 6:
                op.cm('CPIR')
                op.ocf(4); op.mr(3); op.mw(5); op.io_ex(5)
            elif y == 7:
                op.cm('CPDR')
                op.ocf(4); op.mr(3); op.mw(5); op.io_ex(5)
            else:
                op.cm('INV (ED)')
                op.ocf(4)
        elif z == 2:
            if y == 4:
                op.cm('INI')
                op.ocf(5); op.pr(4); op.mw(3)
            elif y == 5:
                op.cm('IND')
                op.ocf(5); op.pr(4); op.mw(3)
            elif y == 6:
                op.cm('INIR')
                op.ocf(5); op.pr(4); op.mw(3); op.io_ex(5)
            elif y == 7:
                op.cm('INDR')
                op.ocf(5); op.pr(4); op.mw(3); op.io_ex(5)
            else:
                op.cm('INV (ED)')
                op.ocf(4)
        elif z == 3:
            if y == 4:
                op.cm('OUTI')
                op.ocf(5); op.mr(3); op.pw(4)
            elif y == 5:
                op.cm('OUTD')
                op.ocf(5); op.mr(3); op.pw(4)
            elif y == 6:
                op.cm('OTIR')
                op.ocf(5); op.mr(3); op.pw(4); op.io_ex(5)
            elif y == 7:
                op.cm('OTDR')
                op.ocf(5); op.mr(3); op.pw(4); op.io_ex(5)
            else:
                op.cm('INV (ED)')
                op.ocf(4)
        else:
            op.cm('INV (ED)')
            op.ocf(4)
    elif x == 1:
        # misc ops
        if z == 0:
            # IN r,(C)
            if y == 6:
                # undocumented special case 'IN F,(C)', only alter flags, don't store result
                op.cm('IN (C)')
                op.ocf(4); op.pr(4)
            else:
                op.cm('IN {},(C)'.format(r[y]))
                op.ocf(4); op.pr(4)
        elif z == 1:
            # OUT (C),r
            if y == 6:
                # undocumented special case 'OUT (C),F', always output 0
                op.cm('OUT (C)')
                op.ocf(4); op.pw(4)
            else:
                op.cm('OUT (C),{}'.format(r[y]))
                op.ocf(4); op.pw(4)
        elif z == 2:
            # SBC/ADC HL,rr
            cmt = 'SBC' if q == 0 else 'ADC'
            op.cm('{} HL,{}'.format(cmt, rp[p]))
            op.ocf(4); op.io(4); op.io(3)
        elif z == 3:
            # 16-bit immediate address load/store
            if q == 0:
                op.cm('LD (nn),{}'.format(rp[p]))
                op.ocf(4); op.odl(3); op.odh(3); op.mwl(3); op.mwh(3)
            else:
                op.cm('LD {},(nn)'.format(rp[p]))
                op.ocf(4); op.odl(3); op.odh(3); op.mrl(3); op.mrh(3)
        elif z == 4:
            # NEG
            op.cm('NEG')
            op.ocf(4)
        elif z == 5:
            # RETN, RETI (only RETI implemented!)
            if y == 1:
                op.cm('RETI')
                op.ocf(4); op.srl(3); op.srh(3)
            else:
                op.cm('INV (ED)')
                op.ocf(4)
        elif z == 6:
            # IM m
            im_mode = [ 0, 0, 1, 2, 0, 0, 1, 2 ]
            op.cm('IM {}'.format(im_mode[y]))
            op.ocf(4)
        elif z == 7:
            # misc ops on I,R and A
            if y == 0:
                op.cm('LD I,A')
                op.ocf(5)
            elif y == 1:
                op.cm('LD R,A')
                op.ocf(5)
            elif y == 2:
                op.cm('LD A,I')
                op.ocf(5)
            elif y == 3:
                op.cm('LD A,R')
                op.ocf(5)
            elif y == 4:
                op.cm('RRD')
                op.ocf(4); op.mr(3); op.io(4); op.mw(3)
            elif y == 5:
                op.cm('RLD')
                op.ocf(4); op.mr(3); op.io(4); op.mw(3)
            elif y == 6 or y == 7:
                op.cm('NOP (ED)')
                op.ocf(4)
    else:
        op.cm('INV (ED)')
        op.ocf(4)
    return op

#-------------------------------------------------------------------------------
#   CB prefix instructions
#
def enc_cb(val, ext, in_op) :
    op = op_info(in_op)
    x = val>>6
    y = (val>>3)&7
    z = val&7

    if x == 0:
        # rotates and shifts
        if z == 6 or ext:
            # ROT (HL); ROT (IX+d); ROT (IY+d)
            # if ext: undocumented ROT (IX+d),(IY+d),r (also stores result in a register)
            if z == 6:
                op.cm('{} {}'.format(rot_cmt[y],iHLcmt(ext)))
            else:
                # undoc: ROT (IX+d),r; ROT (IY+d),r
                op.cm('{} {},{}'.format(rot_cmt[y],iHLcmt(ext),r2[z]))
            if ext:
                op.od(3); op.io(5); op.mr(4); op.mw(3)
            else:
                op.ocf(4); op.mr(4); op.mw(3)
        else:
            # ROT r
            op.cm('{} {}'.format(rot_cmt[y],r2[z]))
            op.ocf(4)
    elif x == 1:
        # BIT n
        if z == 6 or ext:
            # BIT n,(HL); BIT n,(IX+d); BIT n,(IY+d)
            op.cm('BIT {},{}'.format(y,iHLcmt(ext)))
            if ext:
                op.od(3); op.io(5); op.mr(4)
            else:
                op.ocf(4); op.mr(4)
        else:
            # BIT n,r
            op.cm('BIT {},{}'.format(y,r2[z]))
            op.ocf(4)
    elif x == 2:
        # RES n
        if z == 6 or ext:
            # RES n,(HL); RES n,(IX+d); RES n,(IY+d)
            if z == 6:
                op.cm('RES {},{}'.format(y,iHLcmt(ext)))
            else:
                # undoc: RES n,(HL|IX+d|IY+d),r
                op.cm('RES {},{},{}'.format(y,iHLcmt(ext),r2[z]))
            if ext:
                op.od(3); op.io(5); op.mr(4); op.mw(3)
            else:
                op.ocf(4); op.mr(4); op.mw(3)
        else:
            # RES n,r
            op.cm('RES {},{}'.format(y,r2[z]))
            op.ocf(4)
    elif x == 3:
        # SET n
        if z == 6 or ext:
            # SET n,(HL); RES n,(IX+d); RES n,(IY+d)
            if z == 6:
                op.cm('SET {},{}'.format(y,iHLcmt(ext)))
            else:
                # undoc: SET n,(HL|IX+d|IY+d),r
                op.cm('SET {},{},{}'.format(y,iHLcmt(ext),r2[z]))
            if ext:
                op.od(3); op.io(5); op.mr(4); op.mw(3)
            else:
                op.ocf(4); op.mr(4); op.mw(3)
        else:
            # SET n,r
            op.cm('SET {},{}'.format(y,r2[z]))
            op.ocf(4)
    return op

#-------------------------------------------------------------------------------
# patch register lookup tables for IX or IY instructions
#
def patch_reg_tables(reg) :
    r[4] = reg+'H'; r[5] = reg+'L'; r[6] = reg;
    rp[2] = rp2[2] = reg

#-------------------------------------------------------------------------------
# 'unpatch' the register lookup tables
#
def unpatch_reg_tables() :
    r[4] = 'H'; r[5] = 'L'; r[6] = 'HL'
    rp[2] = rp2[2] = 'HL'

#-------------------------------------------------------------------------------
# output a src line
def l(s) :
    Out.write(s+'\n')

#-------------------------------------------------------------------------------
# add an op_info item to a cycle-table (and extra-cycles-table)
#
def add_item(cc_table, cc_ex_table, index, op):
    if cc_table[index] is not None:
        genutil.fmtError('cycle table item already occupied!')
    cc_table[index] = op
    if op.excount != 0:
        if cc_ex_table[index] is not None and cc_ex_table[index].excount != op.excount:
            genutil.fmtError('extra cycle table value mismatch!')
        cc_ex_table[index] = op

#-------------------------------------------------------------------------------
def write_table(name, table):
    l('const uint8_t {}[256] = {{'.format(name))
    for i in range(0, 256):
        if table[i] is not None:
            op = table[i]
            l('  {:2d}, // {:02X}: {} [{}]'.format(op.count, i, op.cmd, op.info))
        else:
            l('   0, // {:02X}: ???'.format(i))
    l('};\n')

#-------------------------------------------------------------------------------
def write_ex_table(name, table):
    l('const uint8_t {}[256] = {{'.format(name))
    for i in range(0, 256):
        if table[i] is not None:
            op = table[i]
            l('  {:2d}, // {:02X}: {} [{}]'.format(op.excount, i, op.cmd, op.info))
        else:
            l('   0, // {:02X}: ???'.format(i))
    l('};\n')

#-------------------------------------------------------------------------------
# main encoder function, this populates all the opcode tables and
# generates the C++ source code into the file f
#
def gen_source(f):

    global Out
    Out = f

    # cycle info tables
    cc_op = [None]*256
    cc_cb = [None]*256
    cc_ed = [None]*256
    cc_dd = [None]*256
    cc_fd = [None]*256
    cc_ddcb = [None]*256
    cc_fdcb = [None]*256
    cc_ex = [None]*256

    # loop over all instruction bytes
    for i in range(0, 256) :
        op = enc(i, False, None)
        # DD or FD prefix instruction?
        if i == 0xDD or i == 0xFD:
            patch_reg_tables('IX' if i==0xDD else 'IY')
            for ii in range(0, 256) :
                op_xy = enc(ii, True, op)
                if ii == 0xCB:
                    # DD/FD CB prefix
                    for iii in range(0, 256) :
                        op_xycb = enc_cb(iii, True, op_xy)
                        if i==0xDD:
                            add_item(cc_ddcb, cc_ex, iii, op_xycb)
                        else:
                            add_item(cc_fdcb, cc_ex, iii, op_xycb)
                if i==0xDD:
                    add_item(cc_dd, cc_ex, ii, op_xy)
                else:
                    add_item(cc_fd, cc_ex, ii, op_xy)
            unpatch_reg_tables()
        # ED prefix instructions
        elif i == 0xED:
            for ii in range(0, 256) :
                op_ed = enc_ed(ii, op)
                add_item(cc_ed, cc_ex, ii, op_ed)
        # CB prefix instructions
        elif i == 0xCB:
            for ii in range(0, 256) :
                op_cb = enc_cb(ii, False, op)
                add_item(cc_cb, cc_ex, ii, op_cb)
        add_item(cc_op, cc_ex, i, op)

    # special items:
    #   cc_ex[0xFF]: interrupt latency (2 cycles)
    op = op_info(None)
    op.cm('INTERRUPT LATENCY')
    op.io_ex(2)
    cc_ex[0xFF] = op


    l('// #version:{}#'.format(Version))
    l('// machine generated, do not edit!')
    l('#include "z80_cycles.h"')
    l('#include "z80.h"')
    l('namespace YAKC {')
    write_table('z80_cc_op', cc_op)
    write_table('z80_cc_cb', cc_cb)
    write_table('z80_cc_ed', cc_ed)
    write_table('z80_cc_dd', cc_dd)
    write_table('z80_cc_fd', cc_fd)
    write_table('z80_cc_ddcb', cc_ddcb)
    write_table('z80_cc_fdcb', cc_fdcb)
    write_ex_table('z80_cc_ex', cc_ex)
    l('} // namespace YAKC');

#-------------------------------------------------------------------------------
# genenerate header
#
def gen_header(f):
    global Out
    Out = f
    l('#pragma once')
    l('// #version:{}#'.format(Version))
    l('// machine generated, do not edit!')
    l('#include <stdint.h>')
    l('namespace YAKC {')
    l('extern const uint8_t z80_cc_op[256];')
    l('extern const uint8_t z80_cc_cb[256];')
    l('extern const uint8_t z80_cc_ed[256];')
    l('extern const uint8_t z80_cc_dd[256];')
    l('extern const uint8_t z80_cc_fd[256];')
    l('extern const uint8_t z80_cc_ddcb[256];')
    l('extern const uint8_t z80_cc_fdcb[256];')
    l('extern const uint8_t z80_cc_ex[256];')
    l('} // namespace YAKC')

#-------------------------------------------------------------------------------
# fips code generator entry 
#
def generate(input, out_src, out_hdr) :
    if genutil.isDirty(Version, [input], [out_src, out_hdr]) :
        with open(out_src, 'w') as f:
            gen_source(f)
        with open(out_hdr, 'w') as f:
            gen_header(f)

