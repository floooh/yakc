//------------------------------------------------------------------------------
//  FileLoader.cc
//------------------------------------------------------------------------------
#include "FileLoader.h"
#include "Core/String/StringBuilder.h"

using namespace Oryol;

namespace YAKC {

// KCC file format header block
#pragma pack(push,1)
struct kcc_header {
    uint8_t name[10];
    uint8_t reserved[6];
    uint8_t num_addr;
    uint8_t load_addr_l;    // NOTE: odd offset!
    uint8_t load_addr_h;
    uint8_t end_addr_l;
    uint8_t end_addr_h;
    uint8_t exec_addr_l;
    uint8_t exec_addr_h;
    uint8_t pad[128 - 23];  // pad to 128 bytes
};

// KC TAP file format header block
struct tap_header {
    uint8_t sig[16];              // "\xC3KC-TAPE by AF. ";
    uint8_t type;                 // 00: KCTAP_Z9001, 01: KCTAP_KC85, else: KCTAB_SYS
    kcc_header kcc;             // from here on identical with KCC
};

// KC Z80 file format header block
struct z80_header {
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
    uint16_t dump_size;
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
#pragma pack(pop)

FileLoader* FileLoader::pointer = nullptr;

//------------------------------------------------------------------------------
void
FileLoader::Setup(yakc& emu_) {
    o_assert(nullptr == pointer);
    this->emu = &emu_;
    pointer = this;
    this->Items.Add("Pengo", "pengo.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Pengo", "pengo4.kcc", filetype::kcc, system::kc85_4);
    this->Items.Add("Cave", "cave.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Labyrinth", "labyrinth.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("House", "house.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("House", "house4.tap", filetype::kc_tap, system::kc85_4);
    this->Items.Add("Jungle", "jungle.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Jungle", "jungle4.tap", filetype::kc_tap, system::kc85_4);
    this->Items.Add("Pacman", "pacman.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Breakout", "breakout.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Mad Breakin", "breakin.853", filetype::kcc, system::kc85_3);
    this->Items.Add("Boulderdash", "boulder3.tap", filetype::kc_tap, system::kc85_3);
    this->Items.Add("Boulderdash", "boulder4.tap", filetype::kc_tap, system::kc85_4);
    this->Items.Add("Digger", "digger3.tap", filetype::kc_tap, system::kc85_3);
    this->Items.Add("Digger", "digger4.tap", filetype::kc_tap, system::kc85_4);
    this->Items.Add("Tetris", "tetris.kcc", filetype::kcc, system::kc85_4);
    this->Items.Add("Ladder", "ladder-3.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Enterprise", "enterpri.tap", filetype::kc_tap, system::any_kc85);
    this->Items.Add("Chess", "chess.kcc", filetype::kcc, system::any_kc85);
    this->Items.Add("Testbild", "testbild.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Demo1", "demo1.kcc", filetype::kcc, system::kc85_4);
    this->Items.Add("Demo2", "demo2.kcc", filetype::kcc, system::kc85_4);
    this->Items.Add("Demo3", "demo3.kcc", filetype::kcc, system::kc85_4);
    this->Items.Add("Tiny-Basic 3.01", "tinybasic-3.01.z80", filetype::kc_z80, system::z1013_01);
    this->Items.Add("KC-Basic", "kc_basic.z80", filetype::kc_z80, system::any_z1013);
    this->Items.Add("Z1013 Forth", "z1013_forth.z80", filetype::kc_z80, system::any_z1013);
    this->Items.Add("Boulderdash", "boulderdash_1_0.z80", filetype::kc_z80, system(int(system::z1013_16)|int(system::z1013_64)));
    this->Items.Add("Demolation", "demolation.z80", filetype::kc_z80, system::any_z1013);
    this->Items.Add("Cosmic Ball", "cosmic_ball.z80", filetype::kc_z80, system::z1013_01);
    this->Items.Add("Galactica", "galactica.z80", filetype::kc_z80, system::any_z1013);
    this->Items.Add("Mazogs", "mazog_deutsch.z80", filetype::kc_z80, system::any_z1013);
    this->Items.Add("Monitor ZM30 (start with 'ZM')", "zm30.kcc", filetype::kcc, system::any_z9001);
    this->Items.Add("Forth 83 (start with 'F83')", "F83_COM.TAP", filetype::kc_tap, system::any_z9001);
    this->Items.Add("Arkanoid", "arkanoid.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Ghosts'n'Goblins", "ghosts_n_goblins.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Gryzor", "gryzor.sna", filetype::cpc_sna, system::cpc6128, true);
    this->Items.Add("Dragon Ninja", "dragon_ninja.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Head over Heels", "head_over_heels.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Boulderdash", "boulder_dash.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Bomb Jack", "bomb_jack.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Chase HQ", "chase_hq.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Cybernoid", "cybernoid.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Fruity Frank", "fruity_frank.sna", filetype::cpc_sna, system(int(system::kccompact)|int(system::cpc6128)), true);
    this->Items.Add("Ikari Warriors", "ikari_warriors.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("1943", "1943.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Exolon", "exolon.z80", filetype::zx_z80, system::zxspectrum48k, true);
    this->Items.Add("Cyclone", "cyclone.z80", filetype::zx_z80, system::zxspectrum48k, true);
    this->Items.Add("Boulderdash", "boulderdash_zx.z80", filetype::zx_z80, system::zxspectrum48k, true);
    this->Items.Add("Bomb Jack", "bombjack_zx.z80", filetype::zx_z80, system::zxspectrum48k, true);
    this->Items.Add("Hello World!", "atom_hello.txt", filetype::text, system::acorn_atom, true);
    this->Items.Add("Text Mode Test", "atom_alnum_test.txt", filetype::text, system::acorn_atom, true);
    this->Items.Add("Graphics Mode Test", "atom_graphics_test.txt", filetype::text, system::acorn_atom, true);
}

//------------------------------------------------------------------------------
void
FileLoader::Discard() {
    o_assert(this == pointer);
    pointer = nullptr;
}

//------------------------------------------------------------------------------
void
FileLoader::Load(const Item& item) {
    this->load(item, false);
}

//------------------------------------------------------------------------------
void
FileLoader::LoadAndStart(const Item& item) {
    this->load(item, true);
}

//------------------------------------------------------------------------------
bool
FileLoader::Copy() {
    if (Ready == this->State) {
        copy(this->emu, this->Info, this->FileData);
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool
FileLoader::Start() {
    if (Ready == this->State) {
        copy(this->emu, this->Info, this->FileData);
        start(this->emu, this->Info, this->FileData);
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
Buffer&&
FileLoader::ObtainTextBuffer() {
    this->State = Waiting;
    return std::move(this->FileData);
}

//------------------------------------------------------------------------------
void
FileLoader::load(const Item& item, bool autostart) {
    StringBuilder strBuilder;
    strBuilder.Format(128, "kcc:%s", item.Filename.AsCStr());
    this->Url = strBuilder.GetString();
    this->State = Loading;
    IO::Load(strBuilder.GetString(),
        // load succeeded
        [this, item, autostart](IO::LoadResult ioResult) {
            this->FileData = std::move(ioResult.Data);
            this->Info = parseHeader(this->FileData, item);
            this->State = Ready;
            String path = ioResult.Url.Path();
            if (this->emu->is_system(system::any_kc85)) {
                auto fp = this->emu->filesystem.open(path.AsCStr(), filesystem::mode::write);
                if (fp) {
                    this->emu->filesystem.write(fp, this->FileData.Data(), this->FileData.Size());
                    this->emu->filesystem.close(fp);
                    this->emu->quickload(path.AsCStr(), this->Info.Type, autostart);
                }
            }
            else if (autostart) {
                copy(this->emu, this->Info, this->FileData);
                start(this->emu, this->Info, this->FileData);
            }
        },
        // load failed
        [this](const URL& url, IOStatus::Code ioStatus) {
            this->State = Failed;
            this->FailedStatus = ioStatus;
        });
}

//------------------------------------------------------------------------------
FileLoader::FileInfo
FileLoader::parseHeader(const Buffer& data, const Item& item) {
    FileInfo info;
    info.Filename = item.Filename;
    info.EnableJoystick = item.EnableJoystick;
    if (filetype::none == item.Type) {
        // guess the file type
        StringBuilder strb(item.Filename);
        if (strb.Contains(".TXT") || strb.Contains(".txt")) {
            info.Type = filetype::text;
        }
        else if (strb.Contains(".TAP") || strb.Contains(".tap")) {
            if (this->emu->is_system(system::any_zx)) {
                info.Type = filetype::zx_tap;
            }
            else if (this->emu->is_system(system::acorn_atom)) {
                info.Type = filetype::atom_tap;
            }
            else {
                info.Type = filetype::kc_tap;
            }
        }
        else if (strb.Contains(".KCC") || strb.Contains(".kcc")) {
            info.Type = filetype::kcc;
        }
        else if (strb.Contains(".Z80") || strb.Contains(".z80")) {
            if (this->emu->is_system(system::any_zx)) {
                info.Type = filetype::zx_z80;
            }
            else {
                info.Type = filetype::kc_z80;
            }
        }
        else if (strb.Contains(".SNA") || strb.Contains(".sna")) {
            info.Type = filetype::cpc_sna;
        }
        else if (strb.Contains(".BIN") || strb.Contains(".bin")) {
            info.Type = filetype::raw;
        }
    }
    else {
        info.Type = item.Type;
    }
    if (data.Empty()) {
        return info;
    }
    const uint8_t* start = data.Data();
    const uint8_t* ptr = start;
    if ((filetype::kc_tap == info.Type) || (filetype::kcc == info.Type)) {
        if (filetype::kc_tap == info.Type) {
            ptr = (const uint8_t*)&(((tap_header*)ptr)->kcc);
        }
        const kcc_header* hdr = (const kcc_header*) ptr;
        info.Name = String((const char*)hdr->name, 0, 16);
        info.StartAddr = (hdr->load_addr_h<<8 | hdr->load_addr_l) & 0xFFFF;
        info.EndAddr = (hdr->end_addr_h<<8 | hdr->end_addr_l) & 0xFFFF;
        info.ExecAddr = (hdr->exec_addr_h<<8 | hdr->exec_addr_l) & 0xFFFF;
        info.HasExecAddr = hdr->num_addr > 2;
        info.PayloadOffset = int((ptr + sizeof(kcc_header)) - start);
        if ((info.EndAddr-info.StartAddr) > data.Size()-128) {
            info.FileSizeError = true;
        }
    }
    else if (filetype::kc_z80 == info.Type) {
        const z80_header* hdr = (const z80_header*) ptr;
        info.Name = String((const char*)hdr->name, 0, 16);
        info.StartAddr = (hdr->load_addr_h<<8 | hdr->load_addr_l) & 0xFFFF;
        info.EndAddr = (hdr->end_addr_h<<8 | hdr->end_addr_l) & 0xFFFF;
        info.ExecAddr = (hdr->exec_addr_h<<8 | hdr->exec_addr_l) & 0xFFFF;
        info.HasExecAddr = true;
        info.PayloadOffset = sizeof(z80_header);
        if ((info.EndAddr-info.StartAddr) > data.Size()-32) {
            info.FileSizeError = true;
        }
    }
    else if (filetype::raw == info.Type) {
        info.Name = item.Name;
        info.StartAddr = item.OptStartAddr;
        info.EndAddr = item.OptStartAddr + data.Size();
        info.ExecAddr = item.OptExecAddr;
        info.HasExecAddr = item.OptExecAddr != 0;
        info.PayloadOffset = 0;
    }
    else if (filetype::zx_z80 == info.Type) {
        // http://www.worldofspectrum.org/faq/reference/z80format.htm
        const zxz80_header* hdr = (const zxz80_header*) ptr;
        const zxz80ext_header* ext_hdr = nullptr;
        uint16_t pc = (hdr->PC_h<<8 | hdr->PC_l) & 0xFFFF;
        if (0 == pc) {
            // Z80 version 2 or 3
            ext_hdr = (const zxz80ext_header*) (ptr + sizeof(zxz80_header));
            pc = (ext_hdr->PC_h<<8 | ext_hdr->PC_l) & 0xFFFF;
            if (ext_hdr->hw_mode < 3) {
                info.RequiredSystem = system::zxspectrum48k;
            }
            else {
                info.RequiredSystem = system::zxspectrum128k;
            }
        }
        else {
            info.RequiredSystem = system::zxspectrum48k;
        }
        info.StartAddr = 0x4000;
        info.EndAddr = 0x10000;
        info.ExecAddr = pc;
        info.HasExecAddr = true;
        info.PayloadOffset = sizeof(zxz80_header);
        if (ext_hdr) {
            info.PayloadOffset += 2 + ((ext_hdr->len_h<<8 | ext_hdr->len_l) & 0xFFFF);
        }
    }
    else if (filetype::cpc_sna == info.Type) {
        // http://cpctech.cpc-live.com/docs/snapshot.html
        const sna_header* hdr = (const sna_header*) ptr;
        info.StartAddr = 0x0000;
        info.EndAddr = 0x10000;
        info.ExecAddr = (hdr->PC_h<<8)|hdr->PC_l;
        info.HasExecAddr = true;
        info.PayloadOffset = 0x100;
        info.RequiredSystem = system::any_cpc;  // FIXME
    }
    else if (filetype::atom_tap == info.Type) {
        const atomtap_header* hdr = (const atomtap_header*) ptr;
        info.Name = String((const char*)hdr->name, 0, 16);        
        info.StartAddr = hdr->load_addr;
        info.EndAddr = hdr->load_addr + hdr->length;
        info.ExecAddr = hdr->exec_addr;
        info.HasExecAddr = true;
        info.PayloadOffset = 0x16;
        info.RequiredSystem = system::acorn_atom;
    }
    return info;
}

//------------------------------------------------------------------------------
void
FileLoader::load_kctap(yakc* emu, const FileInfo& info, const Buffer& data) {
    o_assert_dbg(emu);
    // KC_TAP payload is 128 byte blocks, each with a single header byte
    const uint8_t* payload = data.Data() + info.PayloadOffset;
    uint16_t addr = info.StartAddr;
    const uint8_t* ptr = payload;
    while (addr < info.EndAddr) {
        // skip block lead byte
        ptr++;
        // copy 128 bytes
        for (int i = 0; (i < 128) && (addr < info.EndAddr); i++) {
            emu->board.z80.mem.w8(addr++, *ptr++);
        }
    }
}

//------------------------------------------------------------------------------
void
FileLoader::load_zxz80(yakc* emu, const FileInfo& info, const Buffer& data) {
    // http://www.worldofspectrum.org/faq/reference/z80format.htm
    o_assert_dbg(emu);
    if (emu->is_system(system::any_zx)) {
        const uint8_t* payload = data.Data() + info.PayloadOffset;
        const zxz80_header* hdr = (const zxz80_header*) data.Data();
        bool is_version1 = ((hdr->PC_h != 0) || (hdr->PC_l != 0));
        bool v1_compr = 0 != (hdr->flags0 & (1<<5));
        const uint8_t* ptr = payload;
        const uint8_t* end_ptr = data.Data() + data.Size();
        while (ptr < end_ptr) {
            const uint8_t* src_ptr = ptr;
            int page_index = 0;
            uint32_t src_len, dst_len;
            if (is_version1) {
                src_len = data.Size() - sizeof(zxz80_header);
                dst_len = 48 * 1024;
            }
            else {
                src_ptr += sizeof(zxz80page_header);
                const zxz80page_header* phdr = (const zxz80page_header*) ptr;
                src_len = (phdr->len_h<<8 | phdr->len_l) & 0xFFFF;
                dst_len = 0x4000;
                page_index = phdr->page_nr - 3;
                if (emu->is_system(system::zxspectrum48k) && (page_index == 5)) {
                    page_index = 0;
                }
            }
            uint8_t* dst_ptr = emu->zx.board->ram[page_index];
            const uint8_t* dst_end_ptr = dst_ptr + dst_len;
            if ((page_index >= 0) && (page_index < 8)) {
                if (0xFFFF == src_len) {
                    // uncompressed
                    o_assert2(false, "FIXME: uncompressed!");
                }
                else {
                    // compressed
                    const uint8_t* src_end_ptr = src_ptr + src_len;
                    bool v1_done = false;
                    while ((src_ptr < src_end_ptr) && !v1_done) {
                        uint8_t val = src_ptr[0];
                        // check for version 1 end marker
                        if (v1_compr && (0==val) && (0xED==src_ptr[1]) && (0xED==src_ptr[2]) && (0==src_ptr[3])) {
                            v1_done = true;
                            src_ptr += 4;
                        }
                        else if (0xED == val) {
                            if (0xED == src_ptr[1]) {
                                uint8_t count = src_ptr[2];
                                o_assert(0 != count);
                                uint8_t data = src_ptr[3];
                                src_ptr += 4;
                                for (int i = 0; i < count; i++) {
                                    o_assert(dst_ptr < dst_end_ptr);
                                    *dst_ptr++ = data;
                                }
                            }
                            else {
                                // single ED
                                o_assert(dst_ptr < dst_end_ptr);
                                *dst_ptr++ = val;
                                src_ptr++;
                            }
                        }
                        else {
                            // any value
                            o_assert(dst_ptr < dst_end_ptr);
                            *dst_ptr++ = val;
                            src_ptr++;
                        }
                    }
                    o_assert(dst_ptr == dst_end_ptr);
                    o_assert(src_ptr == src_end_ptr);
                }
            }
            if (0xFFFF == src_len) {
                ptr += sizeof(zxz80page_header) + 0x4000;
            }
            else {
                ptr += sizeof(zxz80page_header) + src_len;
            }
        }
    }
}

//------------------------------------------------------------------------------
void
FileLoader::load_sna(yakc* emu, const FileInfo& info, const Buffer& data) {
    // http://cpctech.cpc-live.com/docs/snapshot.html
    o_assert_dbg(emu);
    if (emu->is_system(system::any_cpc)) {
        const sna_header* hdr = (const sna_header*) data.Data();
        const uint8_t* payload = data.Data() + info.PayloadOffset;
        if (hdr->dump_size == 64) {
            o_assert(sizeof(emu->board.ram) >= 0x10000);
            memcpy(emu->board.ram[0], payload, 0x10000);
        }
        else {
            o_assert(sizeof(emu->board.ram) >= 0x20000);
            memcpy(emu->board.ram[0], payload, 0x20000);
        }
    }
}

//------------------------------------------------------------------------------
void
FileLoader::copy(yakc* emu, const FileInfo& info, const Buffer& data) {
    if (!info.FileSizeError) {
        enum State newState = FileLoader::Waiting;
        if (filetype::zx_z80 == info.Type) {
            FileLoader::load_zxz80(emu, info, data);
        }
        else if (filetype::cpc_sna == info.Type) {
            FileLoader::load_sna(emu, info, data);
        }
        else if (filetype::text == info.Type) {
            // do nothing for text files, this will be checked
            // and caught from the outside
            newState = FileLoader::TextReady;
        }
        else {
            // all others: filetype payload is simply a continuous block of data
            const uint8_t* payload = data.Data() + info.PayloadOffset;
            if (emu->cpu_type() == cpu_model::z80) {
                emu->board.z80.mem.write(info.StartAddr, payload, info.EndAddr-info.StartAddr);
            }
            else {
                emu->board.mos6502.mem.write(info.StartAddr, payload, info.EndAddr-info.StartAddr);
            }
        }
        FileLoader::pointer->State = newState;
    }
}

//------------------------------------------------------------------------------
void
FileLoader::start(yakc* emu, const FileInfo& info, const Buffer& data) {
    emu->enable_joystick(info.EnableJoystick);
    if (info.HasExecAddr) {
        if (filetype::zx_z80 == info.Type) {
            const zxz80_header* hdr = (const zxz80_header*) data.Data();
            const zxz80ext_header* ext_hdr = nullptr;
            if ((hdr->PC_h == 0) && (hdr->PC_l == 0)) {
                ext_hdr = (const zxz80ext_header*) (data.Data() + sizeof(zxz80_header));
            }
            z80& cpu = emu->board.z80;
            cpu.A = hdr->A; cpu.F = hdr->F;
            cpu.B = hdr->B; cpu.C = hdr->C;
            cpu.D = hdr->D; cpu.E = hdr->E;
            cpu.H = hdr->H; cpu.L = hdr->L;
            cpu.IXH = hdr->IX_h; cpu.IXL = hdr->IX_l;
            cpu.IYH = hdr->IY_h; cpu.IYL = hdr->IY_l;
            cpu.AF_ = (hdr->A_<<8 | hdr->F_) & 0xFFFF;
            cpu.BC_ = (hdr->B_<<8 | hdr->C_) & 0xFFFF;
            cpu.DE_ = (hdr->D_<<8 | hdr->E_) & 0xFFFF;
            cpu.HL_ = (hdr->H_<<8 | hdr->L_) & 0xFFFF;
            cpu.SP = (hdr->SP_h<<8 | hdr->SP_l) & 0xFFFF;
            cpu.I = hdr->I;
            cpu.R = (hdr->R & 0x7F) | ((hdr->flags0 & 1)<<7);
            cpu.IFF2 = hdr->IFF2;
            cpu.int_enable = hdr->EI != 0;
            if (hdr->flags1 != 0xFF) {
                cpu.IM = (hdr->flags1 & 3);
            }
            else {
                cpu.IM = 1;
            }
            if (ext_hdr) {
                cpu.PC = (ext_hdr->PC_h<<8 | ext_hdr->PC_l) & 0xFFFF;
                cpu.out(&emu->zx, 0xFFFD, ext_hdr->out_fffd);
                cpu.out(&emu->zx, 0x7FFD, ext_hdr->out_7ffd);
            }
            else {
                cpu.PC = (hdr->PC_h<<8 | hdr->PC_l) & 0xFFFF;
            }
            emu->zx.border_color = zx::palette[(hdr->flags0>>1) & 7] & 0xFFD7D7D7;
        }
        else if (filetype::cpc_sna == info.Type) {
            const sna_header* hdr = (const sna_header*) data.Data();
            // CPU state
            auto& cpu = emu->board.z80;
            cpu.F = hdr->F; cpu.A = hdr->A;
            cpu.C = hdr->C; cpu.B = hdr->B;
            cpu.E = hdr->E; cpu.D = hdr->D;
            cpu.L = hdr->L; cpu.H = hdr->H;
            cpu.R = hdr->R; cpu.I = hdr->I;
            cpu.IFF1 = (hdr->IFF1 & 1) != 0;
            cpu.IFF2 = (hdr->IFF2 & 1) != 0;
            cpu.IX = (hdr->IX_h<<8 | hdr->IX_l) & 0xFFFF;
            cpu.IY = (hdr->IY_h<<8 | hdr->IY_l) & 0xFFFF;
            cpu.SP = (hdr->SP_h<<8 | hdr->SP_l) & 0xFFFF;
            cpu.PC = (hdr->PC_h<<8 | hdr->PC_l) & 0xFFFF;
            cpu.IM = hdr->IM;
            cpu.AF_ = (hdr->A_<<8 | hdr->F_) & 0xFFFF;
            cpu.BC_ = (hdr->B_<<8 | hdr->C_) & 0xFFFF;
            cpu.DE_ = (hdr->D_<<8 | hdr->E_) & 0xFFFF;
            cpu.HL_ = (hdr->H_<<8 | hdr->L_) & 0xFFFF;
            // gate array state
            auto& cpc = emu->cpc;
            for (int i = 0; i < 17; i++) {
                cpc.cpu_out(0x7FFF, i);
                cpc.cpu_out(0x7FFF, (hdr->pens[i] & 0x1F) | 0x40);
            }
            cpc.cpu_out(0x7FFF, hdr->selected_pen & 0x1F);
            cpc.cpu_out(0x7FFF, (hdr->gate_array_config & 0x3F) | 0x80);
            cpc.cpu_out(0x7FFF, (hdr->ram_config & 0x3F) | 0xC0);
            for (int i = 0; i < 18; i++) {
                emu->board.mc6845.select(i);
                emu->board.mc6845.write(hdr->crtc_regs[i]);
            }
            emu->board.mc6845.select(hdr->crtc_selected);
            // FIXME: rom_config
            emu->board.i8255.output[i8255::PORT_A] = hdr->ppi_a;
            emu->board.i8255.output[i8255::PORT_B] = hdr->ppi_b;
            emu->board.i8255.output[i8255::PORT_C] = hdr->ppi_c;
            cpc.psg_selected = hdr->psg_selected;
            for (int i = 0; i < 16; i++) {
                emu->board.ay8910.regs[i] = hdr->psg_regs[i];
            }
        }
        else if (emu->cpu_type() == cpu_model::z80) {
            z80& cpu = emu->board.z80;
            cpu.A = 0x00;
            cpu.F = 0x10;
            cpu.BC = cpu.BC_ = 0x0000;
            cpu.DE = cpu.DE_ = 0x0000;
            cpu.HL = cpu.HL_ = 0x0000;
            cpu.AF_ = 0x0000;
            if (emu->is_system(system::any_kc85)) {

                // initialize registers
                cpu.SP = 0x01C2;

                // delete ASCII video memory
                for (uint16_t addr = 0xb200; addr < 0xb700; addr++) {
                    cpu.mem.w8(addr, 0);
                }
                cpu.mem.w8(0xb7a0, 0);
                if (emu->model == system::kc85_3) {
                    cpu.out(&emu->kc85, 0x89, 0x9f);
                    cpu.mem.w16(cpu.SP, 0xf15c);
                }
                else if (emu->model == system::kc85_4) {
                    cpu.out(&emu->kc85, 0x89, 0xFF);
                    cpu.mem.w16(cpu.SP, 0xf17e);
                }
            }

            // start address
            cpu.PC = info.ExecAddr;
        }
    }
}

//------------------------------------------------------------------------------
//  emscripten-specific file loader function (can be hooked up to
//  a HTML drag'n'drop or file-dialog event handler
//
extern "C" {

void emsc_pass_data(const char* name, const uint8_t* data, int size) {
    Log::Info("External data received: %s, %p, %d\n", name, data, size);
    if (FileLoader::pointer && data && (size > 0)) {
        FileLoader::Item item(name, name, filetype::none, system::any);
        FileLoader* loader = FileLoader::pointer;
        loader->FileData.Clear();
        loader->FileData.Add(data, size);
        loader->Info = loader->parseHeader(loader->FileData, item);
        loader->State = FileLoader::Ready;
        loader->ExtFileReady = true;
    }
}

} // extern "C"

} // namespace YAKC

