#-------------------------------------------------------------------------------
#   roms.py
#   Dump ROMs into C arrays.
#-------------------------------------------------------------------------------

Version = 1
roms = ['basic_c0.853', 'caos31.853'] 

import os.path
import genutil

#-------------------------------------------------------------------------------
def get_rom_path(rom_filename, file_path) :
    '''
    Returns absolute path to ROM file, given ROM name and 
    another full file path in the same directory.
    '''
    return '{}/{}'.format(os.path.dirname(file_path), rom_filename)

#-------------------------------------------------------------------------------
def get_rom_cname(rom_filename) :
    '''
    Returns name of C array for a ROM file.
    '''
    return 'rom_{}'.format(os.path.splitext(rom_filename)[0])

#-------------------------------------------------------------------------------
def gen_header(f, out_hdr, roms) :
    f.write('#pragma once\n')
    f.write('// #version:{}\n'.format(Version))
    f.write('// machine generated, do not edit!\n')
    f.write('namespace yakc {\n')
    for rom in roms :
        rom_path = get_rom_path(rom, out_hdr)
        if os.path.isfile(rom_path) :
            rom_name = get_rom_cname(rom)
            rom_size = os.path.getsize(rom_path)
            f.write('extern unsigned char {}[{}];\n'.format(rom_name, rom_size))
        else :
            genutil.fmtError("ROM file not found: '{}'".format(rom_path))
    f.write('} // namespace yakc\n')

#-------------------------------------------------------------------------------
def gen_source(f, out_src, roms) :
    f.write('// #version:{}\n'.format(Version))
    f.write('// machine generated, do not edit!\n')
    f.write('#include "{}.h"\n'.format(os.path.splitext(os.path.basename(out_src))[0]))
    f.write('namespace yakc {\n')
    for rom in roms :
        rom_path = get_rom_path(rom, out_src)
        if os.path.isfile(rom_path) :
            with open(rom_path, 'rb') as src_file:
                rom_data = src_file.read()
                rom_name = get_rom_cname(rom)
                rom_size = os.path.getsize(rom_path)
                f.write('unsigned char {}[{}] = {{\n'.format(rom_name, rom_size))               
                num = 0
                for byte in rom_data :
                    f.write(hex(ord(byte)) + ', ')
                    num += 1
                    if 0 == num%16:
                        f.write('\n')
                f.write('\n};\n')
        else :
            genutil.fmtError("ROM file not found: '{}'".format(rom_path))
    f.write('} // namespace yakc\n')

#-------------------------------------------------------------------------------
def generate(input, out_src, out_hdr) :
    genutil.setErrorLocation(input, 6)
    inputs = [input]
    outputs = [out_src, out_hdr]
    if genutil.isDirty(Version, inputs, outputs) :
        with open(out_hdr, 'w') as f :
            gen_header(f, out_hdr, roms)
        with open(out_src, 'w') as f :
            gen_source(f, out_src, roms)
        
