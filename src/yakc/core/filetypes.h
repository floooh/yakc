#pragma once
//------------------------------------------------------------------------------
/** 
    @file yakc/core/filetypes.h
    @brief various emulator file format structures
*/
#include "yakc/core/core.h"

/// file type enum
enum class filetype {
    raw,
    kcc,
    kc_tap,
    kc_z80,
    zx_tap,
    zx_z80,
    cpc_sna,
    cpc_tap,
    cpc_bin,    // raw bin file with 128 byte AMSDOS header (http://www.cpcwiki.eu/index.php/AMSDOS_Header)
    atom_tap,
    text,

    num,
    none,
};

/// get filetype enum from string
inline filetype filetype_from_string(const char* str) {
    if (strcmp(str, "raw")==0) return filetype::raw;
    if (strcmp(str, "kcc")==0) return filetype::kcc;
    if (strcmp(str, "kc_tap")==0) return filetype::kc_tap;
    if (strcmp(str, "kc_z80")==0) return filetype::kc_z80;
    if (strcmp(str, "zx_tap")==0) return filetype::zx_tap;
    if (strcmp(str, "zx_z80")==0) return filetype::zx_z80;
    if (strcmp(str, "cpc_sna")==0) return filetype::cpc_sna;
    if (strcmp(str, "cpc_tap")==0) return filetype::cpc_tap;
    if (strcmp(str, "cpc_bin")==0) return filetype::cpc_bin;
    if (strcmp(str, "atom_tap")==0) return filetype::atom_tap;
    if (strcmp(str, "text")==0) return filetype::text;
    return filetype::none;
}

/// return true for quickloadable filetypes
inline bool filetype_quickloadable(filetype t) {
    switch (t) {
        case filetype::cpc_tap:
        case filetype::atom_tap:
            return false;
        default:
            return true;
    }
}

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

// CPC TAP file format header
// http://www.cpcwiki.eu/index.php/Format:TAP_tape_image_file_format
struct cpctap_header {
    uint8_t block_len[2];
    uint8_t block_flags;
    uint8_t name[16];
    uint8_t block_nr;
    uint8_t last_block;
    uint8_t filetype;
    uint8_t block_length_l;
    uint8_t block_length_h;
    uint8_t location_l;
    uint8_t location_h;
    uint8_t first_block;
    uint8_t total_length_l;
    uint8_t total_length_h;
    uint8_t entry_addr_l;
    uint8_t entry_addr_h;
};
static_assert(sizeof(cpctap_header) == 28 + 3, "CPC TAP header size");

// CPC AMSDOS header (for bin files)
// http://www.cpcwiki.eu/index.php/AMSDOS_Header
struct cpcbin_header {
    uint8_t user_number;
    uint8_t file_name[8];
    uint8_t file_ext[3];
    uint8_t pad_0[6];
    uint8_t type;
    uint8_t pad_1[2];
    uint8_t load_addr_l;
    uint8_t load_addr_h;
    uint8_t pad_2;
    uint8_t length_l;
    uint8_t length_h;
    uint8_t start_addr_l;
    uint8_t start_addr_h;
    uint8_t pad_4[4];
    uint8_t pad_5[0x60];
};
static_assert(sizeof(cpcbin_header) == 128, "CPC BIN header size");

// Atom TAP / ATM header (https://github.com/hoglet67/Atomulator/blob/master/docs/atommmc2.txt )
struct atomtap_header {
    uint8_t name[16];
    uint16_t load_addr;
    uint16_t exec_addr;
    uint16_t length;
};
static_assert(sizeof(atomtap_header) == 16 + 3*2, "atomtap header size");
