#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::filesystem
    
    A simple memory filesystem for transferring files between 
    emulated systems and the outside world.
*/
#include "yakc/core/core.h"

namespace YAKC {

class filesystem {
public:
    /// open/close modes
    enum class mode {
        none,
        read,
        write,
    };
    /// file handle typedef
    typedef int file;
    /// the invalid file handle (must evaluate to false!)
    static const int invalid_file = 0;

    /// clear everything in the filesystem
    void reset();
    /// open a file
    file open(const char* name, mode m);
    /// read data from file
    int read(file fp, void* ptr, int num_bytes);
    /// write data to file
    int write(file fp, const void* ptr, int num_bytes);
    /// set current file position, only in read mode!
    bool set_pos(file fp, int pos);
    /// get current file position
    int get_pos(file fp) const;
    /// get size of opened file
    int size(file fp) const;
    /// check for end-of-file reached (pos == size)
    bool eof(file fp) const;
    /// skip N bytes (only in read mode)
    void skip(file fp, int num_bytes);
    /// peek a uint8_t at relative position module file size, fail hard if size==0
    uint8_t peek_u8(file fp, int rel_pos);
    /// close a file
    void close(file fp);
    /// delete a file
    void rm(const char* name);
    /// test if a file exists
    bool exists(const char* name);
    /// find a file entry by name
    file find(const char* name);

    static const int max_num_blocks = 256;
    struct block {
        static const int shift = 12;            // 1<<12 = 4 kbyte
        static const int size = (1<<shift);
        static const int mask = size - 1;
        bool free = true;
    };
    block blocks[max_num_blocks];

    static const int max_num_files = 16;
    struct file_item {
        static const int max_name_size = 128;
        char name[max_name_size] = { };
        mode open_mode = mode::none;
        bool valid = false;
        int pos = 0;
        int size = 0;
        int b_index = -1;   // last write block index
        static const int max_blocks = 32;
        static const int max_size = max_blocks * block::size;
        uint8_t blocks[max_blocks] = { };
    };
    /// note the first entry is never used (it's the 'invalid file')
    file_item files[max_num_files];

    uint8_t store[max_num_blocks][block::size];

    /// convert a position to a block index
    static int block_index(int pos);
    /// convert a byte size to a block index (size + block_size-1) / block_size
    static int num_blocks(int size);
    /// allocate a new block, return -1 if no free blocks
    int alloc_block();
    /// free a block
    void free_block(int i);
};

/// KCC file format header block
struct kcc_header {
    uint8_t name[16];
    uint8_t num_addr;
    uint8_t load_addr_l;    // NOTE: odd offset!
    uint8_t load_addr_h;
    uint8_t end_addr_l;
    uint8_t end_addr_h;
    uint8_t exec_addr_l;
    uint8_t exec_addr_h;
    uint8_t pad[128 - 23];  // pad to 128 bytes
};

/// KC TAP file format header block
struct kctap_header {
    uint8_t sig[16];              // "\xC3KC-TAPE by AF. ";
    uint8_t type;                 // 00: KCTAP_Z9001, 01: KCTAP_KC85, else: KCTAB_SYS
    kcc_header kcc;             // from here on identical with KCC
};

// KC Z80 file format header block
struct kcz80_header {
    uint8_t load_addr_l;
    uint8_t load_addr_h;
    uint8_t end_addr_l;
    uint8_t end_addr_h;
    uint8_t exec_addr_l;
    uint8_t exec_addr_h;
    uint8_t free[6];
    uint8_t typ;
    uint8_t d3[3];        // d3 d3 d3
    uint8_t name[16];
};

// ZX Z80 file format header (http://www.worldofspectrum.org/faq/reference/z80format.htm)
struct zxz80_header {
    uint8_t A, F;
    uint8_t C, B;
    uint8_t L, H;
    uint8_t PC_l, PC_h;
    uint8_t SP_l, SP_h;
    uint8_t I, R;
    uint8_t flags0;
    uint8_t E, D;
    uint8_t C_, B_;
    uint8_t E_, D_;
    uint8_t L_, H_;
    uint8_t A_, F_;
    uint8_t IY_l, IY_h;
    uint8_t IX_l, IX_h;
    uint8_t EI;
    uint8_t IFF2;
    uint8_t flags1;
};
static_assert(sizeof(zxz80_header) == 30, "zxz80_header size mismatch");

struct zxz80ext_header {
    uint8_t len_l;
    uint8_t len_h;
    uint8_t PC_l, PC_h;
    uint8_t hw_mode;
    uint8_t out_7ffd;
    uint8_t rom1;
    uint8_t flags;
    uint8_t out_fffd;
    uint8_t audio[16];
    uint8_t tlow_l;
    uint8_t tlow_h;
    uint8_t spectator_flags;
    uint8_t mgt_rom_paged;
    uint8_t multiface_rom_paged;
    uint8_t rom_0000_1fff;
    uint8_t rom_2000_3fff;
    uint8_t joy_mapping[10];
    uint8_t kbd_mapping[10];
    uint8_t mgt_type;
    uint8_t disciple_button_state;
    uint8_t disciple_flags;
    uint8_t out_1ffd;
};

struct zxz80page_header {
    uint8_t len_l;
    uint8_t len_h;
    uint8_t page_nr;
};

// CPC SNA file format header
// http://cpctech.cpc-live.com/docs/snapshot.html
struct sna_header {
    uint8_t magic[8];     // must be "MV - SNA"
    uint8_t pad0[8];
    uint8_t version;
    uint8_t F, A, C, B, E, D, L, H, R, I;
    uint8_t IFF1, IFF2;
    uint8_t IX_l, IX_h;
    uint8_t IY_l, IY_h;
    uint8_t SP_l, SP_h;
    uint8_t PC_l, PC_h;
    uint8_t IM;
    uint8_t F_, A_, C_, B_, E_, D_, L_, H_;
    uint8_t selected_pen;
    uint8_t pens[17];             // palette + border colors
    uint8_t gate_array_config;
    uint8_t ram_config;
    uint8_t crtc_selected;
    uint8_t crtc_regs[18];
    uint8_t rom_config;
    uint8_t ppi_a;
    uint8_t ppi_b;
    uint8_t ppi_c;
    uint8_t ppi_control;
    uint8_t psg_selected;
    uint8_t psg_regs[16];
    uint8_t dump_size_l;
    uint8_t dump_size_h;
    uint8_t pad1[0x93];
};
static_assert(sizeof(sna_header) == 256, "SNA header size");

// Atom TAP / ATM header (https://github.com/hoglet67/Atomulator/blob/master/docs/atommmc2.txt )
struct atomtap_header {
    uint8_t name[16];
    uint16_t load_addr;
    uint16_t exec_addr;
    uint16_t length;
};
static_assert(sizeof(atomtap_header) == 16 + 3*2, "atomtap header size");

} // namespace YAKC



