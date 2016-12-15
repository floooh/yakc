//------------------------------------------------------------------------------
//  FileLoader.cc
//------------------------------------------------------------------------------
#include "FileLoader.h"
#include "Core/String/StringBuilder.h"

using namespace Oryol;

namespace YAKC {

/// KCC file format header block
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
#pragma pack(pop)

/// TAP file format header block
#pragma pack(push,1)
struct tap_header {
    ubyte sig[16];              // "\xC3KC-TAPE by AF. ";
    ubyte type;                 // 00: KCTAP_Z9001, 01: KCTAP_KC85, else: KCTAB_SYS
    kcc_header kcc;             // from here on identical with KCC
};
#pragma pack(pop)

/// Z80 file format header block
#pragma pack(push,1)
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
#pragma pack(pop)

FileLoader* FileLoader::ptr = nullptr;

//------------------------------------------------------------------------------
void
FileLoader::Setup(yakc& emu_) {
    o_assert(nullptr == ptr);
    this->emu = &emu_;
    ptr = this;
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
}

//------------------------------------------------------------------------------
void
FileLoader::Discard() {
    o_assert(this == ptr);
    ptr = nullptr;
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
        start(this->emu, this->Info);
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
                start(this->emu, this->Info);
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
        const kcc_header* kcc_hdr = (const kcc_header*) ptr;
        info.Name = String((const char*)kcc_hdr->name, 0, 16);
        info.StartAddr = kcc_hdr->load_addr_h<<8 | kcc_hdr->load_addr_l;
        info.EndAddr = kcc_hdr->end_addr_h<<8 | kcc_hdr->end_addr_l;
        info.ExecAddr = kcc_hdr->exec_addr_h<<8 | kcc_hdr->exec_addr_l;
        info.HasExecAddr = kcc_hdr->num_addr > 2;
        info.PayloadOffset = int((ptr + sizeof(kcc_header)) - start);
        if ((info.EndAddr-info.StartAddr) > data.Size()-128) {
            info.FileSizeError = true;
        }
    }
    else if (FileType::KC_Z80 == info.Type) {
        const z80_header* z80_hdr = (const z80_header*) ptr;
        info.Name = String((const char*)z80_hdr->name, 0, 16);
        info.StartAddr = z80_hdr->load_addr_h<<8 | z80_hdr->load_addr_l;
        info.EndAddr = z80_hdr->end_addr_h<<8 | z80_hdr->end_addr_l;
        info.ExecAddr = z80_hdr->exec_addr_h<<8 | z80_hdr->exec_addr_l;
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
    return info;
}

//------------------------------------------------------------------------------
void
FileLoader::copy(yakc* emu, const FileInfo& info, const Buffer& data) {
    if (!info.FileSizeError) {
        const ubyte* payload = data.Data() + info.PayloadOffset;
        if (FileType::KC_TAP == info.Type) {
            // KC_TAP payload is 128 byte blocks, each with a single header byte
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
        else {
            // KCC, Z80 and BIN file type payload is simply a continuous block of data
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
FileLoader::start(yakc* emu, const FileInfo& info) {
    if (info.HasExecAddr) {

        if (FileType::ZX_Z80 == info.Type) {
            // FIXME
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
    if (FileLoader::ptr && data && (size > 0)) {
        FileLoader::Item item(name, name, FileLoader::FileType::None, device::none);
        FileLoader* loader = FileLoader::ptr;
        loader->FileData.Clear();
        loader->FileData.Add(data, size);
        loader->Info = loader->parseHeader(loader->FileData, item);
        loader->State = FileLoader::Ready;
        loader->ExtFileReady = true;
    }
}

} // extern "C"

} // namespace YAKC

