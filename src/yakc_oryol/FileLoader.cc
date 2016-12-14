//------------------------------------------------------------------------------
//  FileLoader.cc
//------------------------------------------------------------------------------
#include "FileLoader.h"
#include "Core/String/StringBuilder.h"

using namespace Oryol;

namespace YAKC {

FileLoader* FileLoader::ptr = nullptr;

//------------------------------------------------------------------------------
void
FileLoader::Setup(yakc& emu) {
    o_assert(nullptr == ptr);
    ptr = this;
    this->Items.Add("Pengo", "pengo.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Pengo", "pengo4.kcc", FileType::KCC, device::kc85_4);
    this->Items.Add("Cave", "cave.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Labyrinth", "labyrinth.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("House", "house.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("House", "house4.tap", FileType::TAP, device::kc85_4);
    this->Items.Add("Jungle", "jungle.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Jungle", "jungle4.tap", FileType::TAP, device::kc85_4);
    this->Items.Add("Pacman", "pacman.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Breakout", "breakout.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Mad Breakin", "breakin.853", FileType::KCC, device::kc85_3);
    this->Items.Add("Boulderdash", "boulder3.tap", FileType::TAP, device::kc85_3);
    this->Items.Add("Boulderdash", "boulder4.tap", FileType::TAP, device::kc85_4);
    this->Items.Add("Digger", "digger3.tap", FileType::TAP, device::kc85_3);
    this->Items.Add("Digger", "digger4.tap", FileType::TAP, device::kc85_4);
    this->Items.Add("Tetris", "tetris.kcc", FileType::KCC, device::kc85_4);
    this->Items.Add("Ladder", "ladder-3.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Enterprise", "enterpri.tap", FileType::TAP, device::any_kc85);
    this->Items.Add("Chess", "chess.kcc", FileType::KCC, device::any_kc85);
    this->Items.Add("Testbild", "testbild.kcc", FileType::KCC, device::kc85_3);
    this->Items.Add("Demo1", "demo1.kcc", FileType::KCC, device::kc85_4);
    this->Items.Add("Demo2", "demo2.kcc", FileType::KCC, device::kc85_4);
    this->Items.Add("Demo3", "demo3.kcc", FileType::KCC, device::kc85_4);
    this->Items.Add("Tiny-Basic 3.01", "tinybasic-3.01.z80", FileType::Z80, device::z1013_01);
    this->Items.Add("KC-Basic", "kc_basic.z80", FileType::Z80, device::any_z1013);
    this->Items.Add("Z1013 Forth", "z1013_forth.z80", FileType::Z80, device::any_z1013);
    this->Items.Add("Boulderdash", "boulderdash_1_0.z80", FileType::Z80, device(int(device::z1013_16)|int(device::z1013_64)));
    this->Items.Add("Demolation", "demolation.z80", FileType::Z80, device::any_z1013);
    this->Items.Add("Cosmic Ball", "cosmic_ball.z80", FileType::Z80, device::z1013_01);
    this->Items.Add("Galactica", "galactica.z80", FileType::Z80, device::any_z1013);
    this->Items.Add("Mazogs", "mazog_deutsch.z80", FileType::Z80, device::any_z1013);
    this->Items.Add("Monitor ZM30 (start with 'ZM')", "zm30.kcc", FileType::KCC, device::any_z9001);
    this->Items.Add("Forth 83 (start with 'F83')", "F83_COM.TAP", FileType::TAP, device::any_z9001);
}

//------------------------------------------------------------------------------
void
FileLoader::Discard() {
    o_assert(this == ptr);
    ptr = nullptr;
}

//------------------------------------------------------------------------------
void
FileLoader::Load(yakc& emu, const Item& item) {
    this->load(&emu, item, false);
}

//------------------------------------------------------------------------------
void
FileLoader::LoadAndStart(yakc& emu, const Item& item) {
    this->load(&emu, item, true);
}

//------------------------------------------------------------------------------
bool
FileLoader::Copy(yakc& emu) {
    if (Ready == this->State) {
        copy(&emu, this->Info, this->FileData);
        patch(&emu, this->Info);
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool
FileLoader::Start(yakc& emu) {
    if (Ready == this->State) {
        copy(&emu, this->Info, this->FileData);
        patch(&emu, this->Info);
        start(&emu, this->Info);
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
FileLoader::load(yakc* emu, const Item& item, bool autostart) {
    StringBuilder strBuilder;
    strBuilder.Format(128, "kcc:%s", item.Filename.AsCStr());
    this->Url = strBuilder.GetString();
    this->State = Loading;
    IO::Load(strBuilder.GetString(),
        // load succeeded
        [this, emu, item, autostart](IO::LoadResult ioResult) {
            this->FileData = std::move(ioResult.Data);
            this->Info = parseHeader(this->FileData, item);
            this->State = Ready;
            if (autostart) {
                copy(emu, this->Info, this->FileData);
                patch(emu, this->Info);
                start(emu, this->Info);
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
    const ubyte* start = data.Data();
    const ubyte* ptr = start;
    info.Type = item.Type;
    if ((FileType::TAP == item.Type) || (FileType::KCC == item.Type)) {
        if (FileType::TAP == item.Type) {
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
    else if (FileType::Z80 == item.Type) {
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
    else if (FileType::BIN == item.Type) {
        info.Name = item.Name;
        info.StartAddr = item.StartAddr;
        info.EndAddr = item.StartAddr + data.Size();
        info.ExecAddr = item.ExecAddr;
        info.HasExecAddr = item.ExecAddr != 0;
        info.PayloadOffset = 0;
    }
    return info;
}

//------------------------------------------------------------------------------
void
FileLoader::copy(yakc* emu, const FileInfo& info, const Buffer& data) {
    if (!info.FileSizeError) {
        const ubyte* payload = data.Data() + info.PayloadOffset;
        if (FileType::TAP != info.Type) {
            // KCC, Z80 and BIN file type payload is simply a continuous block of data
            emu->board.cpu.mem.write(info.StartAddr, payload, info.EndAddr-info.StartAddr);
        }
        else {
            // TAP payload is 128 byte blocks, each with a single header byte
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

//------------------------------------------------------------------------------
//  emscripten-specific file loader function (can be hooked up to
//  a HTML drag'n'drop or file-dialog event handler
//
extern "C" {

void emsc_pass_data(const char* name, const uint8_t* data, int size) {
    Log::Info("External data received: %s, %p, %d\n", name, data, size);
    if (FileLoader::ptr && data && (size > 0)) {
        FileLoader::FileType type = FileLoader::FileType::UNKNOWN;
        StringBuilder strb(name);
        if (strb.Contains(".TAP") || strb.Contains(".tap")) {
            type = FileLoader::FileType::TAP;
            Log::Info("%s is a TAP file\n", name);
        }
        else if (strb.Contains(".KCC") || strb.Contains(".kcc")) {
            type = FileLoader::FileType::KCC;
            Log::Info("%s is a KCC file\n", name);
        }
        else if (strb.Contains(".Z80") || strb.Contains(".z80")) {
            type = FileLoader::FileType::Z80;
            Log::Info("%s is a Z80 file\n", name);
        }
        else if (strb.Contains(".BIN") || strb.Contains(".bin")) {
            type = FileLoader::FileType::BIN;
            Log::Info("%s is a BIN file\n", name);            
        }

        for (int i = 0; i < 128; i++) {
            Log::Info("%02x ", data[i]);
        }
        Log::Info("\n");

        FileLoader::Item item(name, name, type, device::none);
        FileLoader* loader = FileLoader::ptr;
        loader->FileData.Clear();
        loader->FileData.Add(data, size);
        loader->Info = FileLoader::parseHeader(loader->FileData, item);
        loader->State = FileLoader::Ready;
        loader->ExtFileReady = true;
    }
}

} // extern "C"

} // namespace YAKC

