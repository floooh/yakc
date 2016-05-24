//------------------------------------------------------------------------------
//  FileLoader.cc
//------------------------------------------------------------------------------
#include "FileLoader.h"
#include "Core/String/StringBuilder.h"

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
FileLoader::Setup(emu& emu) {
    this->Items.Add("Pengo", "pengo.kcc", device::kc85_3);
    this->Items.Add("Pengo", "pengo4.kcc", device::kc85_4);
    this->Items.Add("Cave", "cave.kcc", device::kc85_3);
    this->Items.Add("Labyrinth", "labyrinth.kcc", device::kc85_3);
    this->Items.Add("House", "house.kcc", device::kc85_3);
    this->Items.Add("House", "house4.tap", device::kc85_4);
    this->Items.Add("Jungle", "jungle.kcc", device::kc85_3);
    this->Items.Add("Jungle", "jungle4.tap", device::kc85_4);
    this->Items.Add("Pacman", "pacman.kcc", device::kc85_3);
    this->Items.Add("Breakout", "breakout.kcc", device::kc85_3);
    this->Items.Add("Mad Breakin", "breakin.853", device::kc85_3);
    this->Items.Add("Boulderdash", "boulder3.tap", device::kc85_3);
    this->Items.Add("Boulderdash", "boulder4.tap", device::kc85_4);
    this->Items.Add("Digger", "digger3.tap", device::kc85_3);
    this->Items.Add("Digger", "digger4.tap", device::kc85_4);
    this->Items.Add("Tetris", "tetris.kcc", device::kc85_4);
    this->Items.Add("Ladder", "ladder-3.kcc", device::kc85_3);
    this->Items.Add("Enterprise", "enterpri.tap", device::any_kc85);
    this->Items.Add("Chess", "chess.kcc", device::any_kc85);
    this->Items.Add("Testbild", "testbild.kcc", device::kc85_3);
    this->Items.Add("Demo1", "demo1.kcc", device::kc85_4);
    this->Items.Add("Demo2", "demo2.kcc", device::kc85_4);
    this->Items.Add("Demo3", "demo3.kcc", device::kc85_4);
}

//------------------------------------------------------------------------------
void
FileLoader::Discard() {
    // empty
}

//------------------------------------------------------------------------------
void
FileLoader::Load(emu& emu, const Item& item) {
    this->load(&emu, item, false);
}

//------------------------------------------------------------------------------
void
FileLoader::LoadAndStart(emu& emu, const Item& item) {
    this->load(&emu, item, true);
}

//------------------------------------------------------------------------------
bool
FileLoader::Copy(emu& emu) {
    if (Ready == this->State) {
        copy(&emu, this->Info, this->kccData);
        patch(&emu, this->Info);
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool
FileLoader::Start(emu& emu) {
    if (Ready == this->State) {
        copy(&emu, this->Info, this->kccData);
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
FileLoader::load(emu* emu, const Item& item, bool autostart) {
    StringBuilder strBuilder;
    strBuilder.Format(128, "kcc:%s", item.Filename.AsCStr());
    this->Url = strBuilder.GetString();
    this->State = Loading;
    IO::Load(strBuilder.GetString(),
        // load succeeded
        [this, emu, autostart](IO::LoadResult ioResult) {
            this->kccData = std::move(ioResult.Data);
            this->Info = parseHeader(this->kccData);
            this->State = Ready;
            if (autostart) {
                copy(emu, this->Info, this->kccData);
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
FileLoader::parseHeader(const Buffer& data) {
    FileInfo info;
    const ubyte* start = data.Data();
    const ubyte* ptr = start;

    // first check whether this is a KCC or TAP file
    const char* tap_header_string = "\xC3KC-TAPE by AF. ";
    if (0 == memcmp(ptr, tap_header_string, strlen(tap_header_string))) {
        // it's a TAP!
        info.Type = FileType::TAP;
        ptr = (const ubyte*)&(((tap_header*)ptr)->kcc);
    }
    else {
        info.Type = FileType::KCC;
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
    return info;
}

//------------------------------------------------------------------------------
void
FileLoader::copy(emu* emu, const FileInfo& info, const Buffer& data) {
    if (!info.FileSizeError) {
        const ubyte* payload = data.Data() + info.PayloadOffset;
        if (FileType::KCC == info.Type) {
            // KCC payload is simply a continuous block of data
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
FileLoader::patch(emu* emu, const FileInfo& info) {
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
FileLoader::start(emu* emu, const FileInfo& info) {
    if (info.HasExecAddr) {

        // initialize registers
        z80& cpu = emu->board.cpu;
        cpu.A = 0x00;
        cpu.F = 0x10;
        cpu.BC = cpu.BC_ = 0x0000;
        cpu.DE = cpu.DE_ = 0x0000;
        cpu.HL = cpu.HL_ = 0x0000;
        cpu.AF_ = 0x0000;
        cpu.SP = 0x01C2;

        // delete ASCII video memory
        for (uword addr = 0xb200; addr < 0xb700; addr++) {
            cpu.mem.w8(addr, 0);
        }
        cpu.mem.w8(0xb7a0, 0);
        if (emu->model == device::kc85_3) {
            cpu.out(0x89, 0x9f);
            cpu.mem.w16(cpu.SP, 0xf15c);
        }
        else if (emu->model == device::kc85_4) {
            cpu.out(0x89, 0xFF);
            cpu.mem.w16(cpu.SP, 0xf17e);
        }

        // start address
        cpu.PC = info.ExecAddr;
    }
}
