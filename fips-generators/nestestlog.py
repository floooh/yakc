#-------------------------------------------------------------------------------
#   nestestlog.py
#   Parses the nestest.log text file (http://www.qmtpro.com/~nes/misc/nestest.log)
#   into a C array.
#-------------------------------------------------------------------------------

Version = 4

import os.path
import yaml
import genutil

#-------------------------------------------------------------------------------
def gen_header(in_log, out_hdr):

    with open(out_hdr, 'w') as f:
        f.write('// #version:{}#\n'.format(Version))
        f.write('// machine generated, do not edit!\n')
        f.write('#include "yakc/core/core.h"\n')
        f.write('struct cpu_state {\n')
        f.write('    uint16_t PC;\n')
        f.write('    uint8_t A,X,Y,P,S;\n')
        f.write('};\n');
        f.write('cpu_state state_table[] = {\n')
        lines = []
        with open(in_log, 'r') as fi:
            lines = fi.readlines()
        for line in lines :
            f.write('  {{ 0x{},0x{},0x{},0x{},0x{},0x{} }},\n'.format(
                line[0:4],      # PC
                line[50:52],    # A
                line[55:57],    # X
                line[60:62],    # Y
                line[65:67],    # P
                line[71:73]))   # S
        f.write('};\n')

#-------------------------------------------------------------------------------
def generate(input, out_src, out_hdr):
    if genutil.isDirty(Version, [input], [out_hdr]):
        gen_header(input, out_hdr)

