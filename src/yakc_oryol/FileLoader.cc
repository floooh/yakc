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
    ubyte name[10];
    ubyte reserved[6];
    ubyte num_addr;
    ubyte load_addr_l;    // NOTE: odd offset!
    ubyte load_addr_h;
    ubyte end_addr_l;
    ubyte end_addr_h;
    ubyte exec_addr_l;
    ubyte exec_addr_h;
    ubyte pad[128 - 23];  // pad to 128 bytes
};

// KC TAP file format header block
struct tap_header {
    ubyte sig[16];              // "\xC3KC-TAPE by AF. ";
    ubyte type;                 // 00: KCTAP_Z9001, 01: KCTAP_KC85, else: KCTAB_SYS
    kcc_header kcc;             // from here on identical with KCC
};

// KC Z80 file format header block
struct z80_header {
    ubyte load_addr_l;
    ubyte load_addr_h;
    ubyte end_addr_l;
    ubyte end_addr_h;
    ubyte exec_addr_l;
    ubyte exec_addr_h;
    ubyte free[6];
    ubyte typ;
    ubyte d3[3];        // d3 d3 d3
    ubyte name[16];
};

// ZX Z80 file format header (http://www.worldofspectrum.org/faq/reference/z80format.htm)
struct zxz80_header {
    ubyte A, F;
    ubyte C, B;
    ubyte L, H;
    ubyte PC_l, PC_h;
    ubyte SP_l, SP_h;
    ubyte I, R;
    ubyte flags0;
    ubyte E, D;
    ubyte C_, B_;
    ubyte E_, D_;
    ubyte L_, H_;
    ubyte A_, F_;
    ubyte IY_l, IY_h;
    ubyte IX_l, IX_h;
    ubyte EI;
    ubyte IFF2;
    ubyte flags1;
};
static_assert(sizeof(zxz80_header) == 30, "zxz80_header size mismatch");

struct zxz80ext_header {
    ubyte len_l;
    ubyte len_h;
    ubyte PC_l, PC_h;
    ubyte hw_mode;
    ubyte out_7ffd;
    ubyte rom1;
    ubyte flags;
    ubyte out_fffd;
    ubyte audio[16];
    ubyte tlow_l;
    ubyte tlow_h;
    ubyte spectator_flags;
    ubyte mgt_rom_paged;
    ubyte multiface_rom_paged;
    ubyte rom_0000_1fff;
    ubyte rom_2000_3fff;
    ubyte joy_mapping[10];
    ubyte kbd_mapping[10];
    ubyte mgt_type;
    ubyte disciple_button_state;
    ubyte disciple_flags;
    ubyte out_1ffd;
};

struct zxz80page_header {
    ubyte len_l;
    ubyte len_h;
    ubyte page_nr;
};

// CPC SNA file format header
// http://cpctech.cpc-live.com/docs/snapshot.html
struct sna_header {
    ubyte magic[8];     // must be "MV - SNA"
    ubyte pad0[8];
    ubyte version;
    ubyte F, A, C, B, E, D, L, H, R, I;
    ubyte IFF1, IFF2;
    ubyte IX_l, IX_h;
    ubyte IY_l, IY_h;
    ubyte SP_l, SP_h;
    ubyte PC_l, PC_h;
    ubyte IM;
    ubyte F_, A_, C_, B_, E_, D_, L_, H_;
    ubyte selected_pen;
    ubyte pens[17];             // palette + border colors
    ubyte gate_array_config;
    ubyte ram_config;
    ubyte crtc_selected;
    ubyte crtc_regs[18];
    ubyte rom_config;
    ubyte ppi_a;
    ubyte ppi_b;
    ubyte ppi_c;
    ubyte ppi_control;
    ubyte psg_selected;
    ubyte psg_regs[16];
    uword dump_size;
    ubyte pad1[0x93];
};
static_assert(sizeof(sna_header) == 256, "SNA header size");
#pragma pack(pop)

FileLoader* FileLoader::pointer = nullptr;

//------------------------------------------------------------------------------
void
FileLoader::Setup(yakc& emu_) {
    o_assert(nullptr == pointer);
    this->emu = &emu_;
    pointer = this;
    this->Items.Add("Pengo", "pengo.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Pengo", "pengo4.kcc", FileType::KCC, device::kc85_4);
    this->Items.Add("Cave", "cave.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Labyrinth", "labyrinth.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("House", "house.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("House", "house4.tap", FileType::KC_TAP, device::kc85_4);
    this->Items.Add("Jungle", "jungle.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Jungle", "jungle4.tap", FileType::KC_TAP, device::kc85_4);
    this->Items.Add("Pacman", "pacman.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Breakout", "breakout.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Mad Breakin", "breakin.853", FileType::KCC, device::kc85_3);
    this->Items.Add("Boulderdash", "boulder3.tap", FileType::KC_TAP, device::kc85_3);
    this->Items.Add("Boulderdash", "boulder4.tap", FileType::KC_TAP, device::kc85_4);
    this->Items.Add("Digger", "digger3.tap", FileType::KC_TAP, device::kc85_3);
    this->Items.Add("Digger", "digger4.tap", FileType::KC_TAP, device::kc85_4);
    this->Items.Add("Tetris", "tetris.kcc", FileType::KCC, device::kc85_4);
    this->Items.Add("Ladder", "ladder-3.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Enterprise", "enterpri.tap", FileType::KC_TAP, device::any_kc85);
    this->Items.Add("Chess", "chess.kcc", FileType::KCC, device::any_kc85);
    this->Items.Add("Testbild", "testbild.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Demo1", "demo1.kcc", FileType::KCC, device::kc85_4);
    this->Items.Add("Demo2", "demo2.kcc", FileType::KCC, device::kc85_4);
    this->Items.Add("Demo3", "demo3.kcc", FileType::KCC, device::kc85_4);
    this->Items.Add("Tiny-Basic 3.01", "tinybasic-3.01.z80", FileType::KC_Z80, device::z1013_01);
    this->Items.Add("KC-Basic", "kc_basic.z80", FileType::KC_Z80, device::any_z1013);
    this->Items.Add("Z1013 Forth", "z1013_forth.z80", FileType::KC_Z80, device::any_z1013);
    this->Items.Add("Boulderdash", "boulderdash_1_0.z80", FileType::KC_Z80, device(int(device::z1013_16)|int(device::z1013_64)));
    this->Items.Add("Demolation", "demolation.z80", FileType::KC_Z80, device::any_z1013);
    this->Items.Add("Cosmic Ball", "cosmic_ball.z80", FileType::KC_Z80, device::z1013_01);
    this->Items.Add("Galactica", "galactica.z80", FileType::KC_Z80, device::any_z1013);
    this->Items.Add("Mazogs", "mazog_deutsch.z80", FileType::KC_Z80, device::any_z1013);
    this->Items.Add("Monitor ZM30 (start with 'ZM')", "zm30.kcc", FileType::KCC, device::any_z9001);
    this->Items.Add("Forth 83 (start with 'F83')", "F83_COM.TAP", FileType::KC_TAP, device::any_z9001);
    this->Items.Add("Arkanoid", "arkanoid.sna", FileType::CPC_SNA, device::any_cpc, true);
    this->Items.Add("Ghosts'n'Goblins", "ghosts_n_goblins.sna", FileType::CPC_SNA, device::any_cpc, true);
    this->Items.Add("Gryzor", "gryzor.sna", FileType::CPC_SNA, device::cpc6128, true);
    this->Items.Add("Dragon Ninja", "dragon_ninja.sna", FileType::CPC_SNA, device::any_cpc, true);
    this->Items.Add("Head over Heels", "head_over_heels.sna", FileType::CPC_SNA, device::any_cpc, true);
    this->Items.Add("Boulderdash", "boulder_dash.sna", FileType::CPC_SNA, device::any_cpc, true);
    this->Items.Add("Bomb Jack", "bomb_jack.sna", FileType::CPC_SNA, device::any_cpc, true);
    this->Items.Add("Chase HQ", "chase_hq.sna", FileType::CPC_SNA, device::any_cpc, true);
    this->Items.Add("Cybernoid", "cybernoid.sna", FileType::CPC_SNA, device::any_cpc, true);
    this->Items.Add("Fruity Frank", "fruity_frank.sna", FileType::CPC_SNA, device(int(device::kccompact)|int(device::cpc6128)), true);
    this->Items.Add("Ikari Warriors", "ikari_warriors.sna", FileType::CPC_SNA, device::any_cpc, true);
    this->Items.Add("1943", "1943.sna", FileType::CPC_SNA, device::any_cpc, true);
    this->Items.Add("Exolon", "exolon.z80", FileType::ZX_Z80, device::zxspectrum48k, true);
    this->Items.Add("Cyclone", "cyclone.z80", FileType::ZX_Z80, device::zxspectrum48k, true);
    this->Items.Add("Boulderdash", "boulderdash_zx.z80", FileType::ZX_Z80, device::zxspectrum48k, true);
    this->Items.Add("Bomb Jack", "bombjack_zx.z80", FileType::ZX_Z80, device::zxspectrum48k, true);
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
        patch(this->emu, this->Info);
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
        patch(this->emu, this->Info);
        start(this->emu, this->Info, this->FileData);
        return true;
    }
    else {
        return false;
    }
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
            if (autostart) {
                copy(this->emu, this->Info, this->FileData);
                patch(this->emu, this->Info);
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
    if (FileType::None == item.Type) {
        // guess the file type
        StringBuilder strb(item.Filename);
        if (strb.Contains(".TAP") || strb.Contains(".tap")) {
            if (this->emu->is_device(device::any_zx)) {
                info.Type = FileLoader::FileType::ZX_TAP;
            }
            else {
                info.Type = FileLoader::FileType::KC_TAP;
            }
        }
        else if (strb.Contains(".KCC") || strb.Contains(".kcc")) {
            info.Type = FileLoader::FileType::KCC;
        }
        else if (strb.Contains(".Z80") || strb.Contains(".z80")) {
            if (this->emu->is_device(device::any_zx)) {
                info.Type = FileLoader::FileType::ZX_Z80;
            }
            else {
                info.Type = FileLoader::FileType::KC_Z80;
            }
        }
        else if (strb.Contains(".SNA") || strb.Contains(".sna")) {
            info.Type = FileLoader::FileType::CPC_SNA;
        }
        else if (strb.Contains(".BIN") || strb.Contains(".bin")) {
            info.Type = FileLoader::FileType::RAW;
        }
    }
    else {
        info.Type = item.Type;
    }
    if (data.Empty()) {
        return info;
    }
    const ubyte* start = data.Data();
    const ubyte* ptr = start;
    if ((FileType::KC_TAP == info.Type) || (FileType::KCC == info.Type)) {
        if (FileType::KC_TAP == info.Type) {
            ptr = (const ubyte*)&(((tap_header*)ptr)->kcc);
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
    else if (FileType::KC_Z80 == info.Type) {
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
    else if (FileType::RAW == info.Type) {
        info.Name = item.Name;
        info.StartAddr = item.OptStartAddr;
        info.EndAddr = item.OptStartAddr + data.Size();
        info.ExecAddr = item.OptExecAddr;
        info.HasExecAddr = item.OptExecAddr != 0;
        info.PayloadOffset = 0;
    }
    else if (FileType::ZX_Z80 == info.Type) {
        // http://www.worldofspectrum.org/faq/reference/z80format.htm
        const zxz80_header* hdr = (const zxz80_header*) ptr;
        const zxz80ext_header* ext_hdr = nullptr;
        uword pc = (hdr->PC_h<<8 | hdr->PC_l) & 0xFFFF;
        if (0 == pc) {
            // Z80 version 2 or 3
            ext_hdr = (const zxz80ext_header*) (ptr + sizeof(zxz80_header));
            pc = (ext_hdr->PC_h<<8 | ext_hdr->PC_l) & 0xFFFF;
            if (ext_hdr->hw_mode < 3) {
                info.RequiredSystem = device::zxspectrum48k;
            }
            else {
                info.RequiredSystem = device::zxspectrum128k;
            }
        }
        else {
            info.RequiredSystem = device::zxspectrum48k;
        }
        info.StartAddr = 0x4000;
        info.EndAddr = 0x10000;
        info.ExecAddr = pc;
        info.HasExecAddr = true;
        info.PayloadOffset = sizeof(zxz80_header);
        if (ext_hdr) {
            info.PayloadOffset += 2 + (ext_hdr->len_h<<8 | ext_hdr->len_l) & 0xFFFF;
        }
    }
    else if (FileType::CPC_SNA == info.Type) {
        // http://cpctech.cpc-live.com/docs/snapshot.html
        const sna_header* hdr = (const sna_header*) ptr;
        info.StartAddr = 0x0000;
        info.EndAddr = 0x10000;
        info.ExecAddr = (hdr->PC_h<<8)|hdr->PC_l;
        info.HasExecAddr = true;
        info.PayloadOffset = 0x100;
        info.RequiredSystem = device::any_cpc;  // FIXME
    }
    return info;
}

//------------------------------------------------------------------------------
void
FileLoader::load_kctap(yakc* emu, const FileInfo& info, const Buffer& data) {
    o_assert_dbg(emu);
    // KC_TAP payload is 128 byte blocks, each with a single header byte
    const ubyte* payload = data.Data() + info.PayloadOffset;
    uword addr = info.StartAddr;
    const ubyte* ptr = payload;
    while (addr < info.EndAddr) {
        // skip block lead byte
        ptr++;
        // copy 128 bytes
        for (int i = 0; (i < 128) && (addr < info.EndAddr); i++) {
            emu->board.cpu.mem.w8(addr++, *ptr++);
        }
    }
}

//------------------------------------------------------------------------------
void
FileLoader::load_zxz80(yakc* emu, const FileInfo& info, const Buffer& data) {
    // http://www.worldofspectrum.org/faq/reference/z80format.htm
    o_assert_dbg(emu);
    if (emu->is_device(device::any_zx)) {
        const ubyte* payload = data.Data() + info.PayloadOffset;
        const zxz80_header* hdr = (const zxz80_header*) data.Data();
        bool is_version1 = ((hdr->PC_h != 0) || (hdr->PC_l != 0));
        bool v1_compr = 0 != (hdr->flags0 & (1<<5));
        const ubyte* ptr = payload;
        const ubyte* end_ptr = data.Data() + data.Size();
        while (ptr < end_ptr) {
            const ubyte* src_ptr = ptr;
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
                if (emu->is_device(device::zxspectrum48k) && (page_index == 5)) {
                    page_index = 0;
                }
            }
            ubyte* dst_ptr = emu->zx.board->ram[page_index];
            const ubyte* dst_end_ptr = dst_ptr + dst_len;
            if ((page_index >= 0) && (page_index < 8)) {
                if (0xFFFF == src_len) {
                    // uncompressed
                    o_assert2(false, "FIXME: uncompressed!");
                }
                else {
                    // compressed
                    const ubyte* src_end_ptr = src_ptr + src_len;
                    bool v1_done = false;
                    while ((src_ptr < src_end_ptr) && !v1_done) {
                        ubyte val = src_ptr[0];
                        // check for version 1 end marker
                        if (v1_compr && (0==val) && (0xED==src_ptr[1]) && (0xED==src_ptr[2]) && (0==src_ptr[3])) {
                            v1_done = true;
                            src_ptr += 4;
                        }
                        else if (0xED == val) {
                            if (0xED == src_ptr[1]) {
                                ubyte count = src_ptr[2];
                                o_assert(0 != count);
                                ubyte data = src_ptr[3];
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
    if (emu->is_device(device::any_cpc)) {
        const sna_header* hdr = (const sna_header*) data.Data();
        const ubyte* payload = data.Data() + info.PayloadOffset;
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
        if (FileType::KC_TAP == info.Type) {
            FileLoader::load_kctap(emu, info, data);
        }
        else if (FileType::ZX_Z80 == info.Type) {
            FileLoader::load_zxz80(emu, info, data);
        }
        else if (FileType::CPC_SNA == info.Type) {
            FileLoader::load_sna(emu, info, data);
        }
        else {
            // KCC, Z80 and BIN file type payload is simply a continuous block of data
            const ubyte* payload = data.Data() + info.PayloadOffset;            
            emu->board.cpu.mem.write(info.StartAddr, payload, info.EndAddr-info.StartAddr);
        }
    }
}

//------------------------------------------------------------------------------
void
FileLoader::patch(yakc* emu, const FileInfo& info) {
    auto& mem = emu->board.cpu.mem;

    // FIXME: patch JUNGLE until I have time to do a proper
    // 'restoration', see Alexander Lang's KC emu here:
    // http://lanale.de/kc85_emu/KC85_Emu.html
    if (info.Name == "JUNGLE     ") {
        // patch start level 1 into memory
        mem.w8(0x36b7, 1);
        mem.w8(0x3697, 1);
        for (int i = 0; i < 5; i++) {
            mem.w8(0x1770 + i, mem.r8(0x36b6 + i));
        }
    }
    // FIXME: patch Digger (see http://lanale.de/kc85_emu/KC85_Emu.html)
    if (info.Name == "DIGGER  COM\x01") {
        mem.w16(0x09AA, 0x0160);    // time for delay-loop 0160 instead of 0260
        mem.w8(0x3d3a, 0xB5);   // OR L instead of OR (HL)
    }
    if (info.Name == "DIGGERJ") {
        mem.w16(0x09AA, 0x0260);
        mem.w8(0x3d3a, 0xB5);   // OR L instead of OR (HL)
    }
}

//------------------------------------------------------------------------------
void
FileLoader::start(yakc* emu, const FileInfo& info, const Buffer& data) {
    emu->enable_joystick(info.EnableJoystick);
    if (info.HasExecAddr) {
        if (FileType::ZX_Z80 == info.Type) {
            const zxz80_header* hdr = (const zxz80_header*) data.Data();
            const zxz80ext_header* ext_hdr = nullptr;
            if ((hdr->PC_h == 0) && (hdr->PC_l == 0)) {
                ext_hdr = (const zxz80ext_header*) (data.Data() + sizeof(zxz80_header));
            }
            z80& cpu = emu->board.cpu;
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
            cpu.enable_interrupt = hdr->EI != 0;
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
        else if (FileType::CPC_SNA == info.Type) {
            const sna_header* hdr = (const sna_header*) data.Data();
            // CPU state
            auto& cpu = emu->board.cpu;
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
        else {
            z80& cpu = emu->board.cpu;
            cpu.A = 0x00;
            cpu.F = 0x10;
            cpu.BC = cpu.BC_ = 0x0000;
            cpu.DE = cpu.DE_ = 0x0000;
            cpu.HL = cpu.HL_ = 0x0000;
            cpu.AF_ = 0x0000;
            if (emu->is_device(device::any_kc85)) {

                // initialize registers
                cpu.SP = 0x01C2;

                // delete ASCII video memory
                for (uword addr = 0xb200; addr < 0xb700; addr++) {
                    cpu.mem.w8(addr, 0);
                }
                cpu.mem.w8(0xb7a0, 0);
                if (emu->model == device::kc85_3) {
                    cpu.out(&emu->kc85, 0x89, 0x9f);
                    cpu.mem.w16(cpu.SP, 0xf15c);
                }
                else if (emu->model == device::kc85_4) {
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
        FileLoader::Item item(name, name, FileLoader::FileType::None, device::any);
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

