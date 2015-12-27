//------------------------------------------------------------------------------
//  FileLoader.cc
//------------------------------------------------------------------------------
#include "FileLoader.h"
#include "Core/String/StringBuilder.h"

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
FileLoader::Setup(kc85& kc) {
    this->Items.Add("Pengo", "pengo.kcc", kc85_model::kc85_3);
    this->Items.Add("Pengo", "pengo4.kcc", kc85_model::kc85_4);
    this->Items.Add("Cave", "cave.kcc", kc85_model::kc85_3);
    this->Items.Add("Labyrinth", "labyrinth.kcc", kc85_model::kc85_3);
    this->Items.Add("House", "house.kcc", kc85_model::kc85_3);
    this->Items.Add("House", "house4.tap", kc85_model::kc85_4);
    this->Items.Add("Jungle", "jungle.kcc", kc85_model::kc85_3);
    this->Items.Add("Jungle", "jungle4.tap", kc85_model::kc85_4);
    this->Items.Add("Pacman", "pacman.kcc", kc85_model::kc85_3);
    this->Items.Add("Breakout", "breakout.kcc", kc85_model::kc85_3);
    this->Items.Add("Mad Breakin", "breakin.853", kc85_model::kc85_3);
    this->Items.Add("Boulderdash", "boulder3.tap", kc85_model::kc85_3);
    this->Items.Add("Boulderdash", "boulder4.tap", kc85_model::kc85_4);
    this->Items.Add("Digger", "digger3.tap", kc85_model::kc85_3);
    this->Items.Add("Digger", "digger4.tap", kc85_model::kc85_4);
    this->Items.Add("Tetris", "tetris.kcc", kc85_model::kc85_4);
    this->Items.Add("Ladder", "ladder-3.kcc", kc85_model::kc85_3);
    this->Items.Add("Enterprise", "enterpri.tap", kc85_model::any);
    this->Items.Add("Chess", "chess.kcc", kc85_model::any);
    this->Items.Add("Testbild", "testbild.kcc", kc85_model::kc85_3);
    this->Items.Add("Demo1", "demo1.kcc", kc85_model::kc85_4);
    this->Items.Add("Demo2", "demo2.kcc", kc85_model::kc85_4);
    this->Items.Add("Demo3", "demo3.kcc", kc85_model::kc85_4);
    this->ioQueue.Start();
}

//------------------------------------------------------------------------------
void
FileLoader::Discard() {
    this->ioQueue.Stop();
}

//------------------------------------------------------------------------------
void
FileLoader::Load(kc85& kc, const Item& item) {
    this->load(&kc, item, false);
}

//------------------------------------------------------------------------------
void
FileLoader::LoadAndStart(kc85& kc, const Item& item) {
    this->load(&kc, item, true);
}

//------------------------------------------------------------------------------
bool
FileLoader::Copy(kc85& kc) {
    if (Ready == this->State) {
        copy(&kc, this->Info, this->kccData);
        patch(&kc, this->Info);
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool
FileLoader::Start(kc85& kc) {
    if (Ready == this->State) {
        copy(&kc, this->Info, this->kccData);
        patch(&kc, this->Info);
        start(&kc, this->Info);
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
FileLoader::load(kc85* kc, const Item& item, bool autostart) {
    StringBuilder strBuilder;
    strBuilder.Format(128, "kcc:%s", item.Filename.AsCStr());
    this->Url = strBuilder.GetString();
    this->State = Loading;
    this->ioQueue.Add(strBuilder.GetString(),
        // load succeeded
        [this, kc, autostart](IOQueue::Result ioResult) {
            this->kccData = std::move(ioResult.Data);
            this->Info = parseHeader(this->kccData);
            this->State = Ready;
            if (autostart) {
                copy(kc, this->Info, this->kccData);
                patch(kc, this->Info);
                start(kc, this->Info);
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
FileLoader::copy(kc85* kc, const FileInfo& info, const Buffer& data) {
    if (!info.FileSizeError) {
        const ubyte* payload = data.Data() + info.PayloadOffset;
        if (FileType::KCC == info.Type) {
            // KCC payload is simply a continuous block of data
            kc->cpu.mem.write(info.StartAddr, payload, info.EndAddr-info.StartAddr);
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
                    kc->cpu.mem.w8(addr++, *ptr++);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void
FileLoader::patch(kc85* kc, const FileInfo& info) {
    // FIXME: patch JUNGLE until I have time to do a proper
    // 'restoration', see Alexander Lang's KC emu here:
    // http://lanale.de/kc85_emu/KC85_Emu.html
    if (info.Name == "JUNGLE     ") {
        // patch start level 1 into memory
        auto& mem = kc->cpu.mem;
        mem.w8(0x36b7, 1);
        mem.w8(0x3697, 1);
        for (int i = 0; i < 5; i++) {
            mem.w8(0x1770 + i, mem.r8(0x36b6 + i));
        }
    }
    // FIXME: patch Digger (see http://lanale.de/kc85_emu/KC85_Emu.html)
    if (info.Name == "DIGGER  COM\x01") {
        auto& mem = kc->cpu.mem;
        mem.w16(0x09AA, 0x0160);    // time for delay-loop 0160 instead of 0260
        mem.w8(0x3d3a, 0xB5);   // OR L instead of OR (HL)
    }
    if (info.Name == "DIGGERJ") {
        auto& mem = kc->cpu.mem;
        mem.w16(0x09AA, 0x0260);
        mem.w8(0x3d3a, 0xB5);   // OR L instead of OR (HL)
    }
}

//------------------------------------------------------------------------------
void
FileLoader::start(kc85* kc, const FileInfo& info) {
    if (info.HasExecAddr) {

        // initialize registers
        kc->cpu.A = 0x00;
        kc->cpu.F = 0x10;
        kc->cpu.BC = kc->cpu.BC_ = 0x0000;
        kc->cpu.DE = kc->cpu.DE_ = 0x0000;
        kc->cpu.HL = kc->cpu.HL_ = 0x0000;
        kc->cpu.AF_ = 0x0000;
        kc->cpu.SP = 0x01C2;

        // delete ASCII video memory
        for (uword addr = 0xb200; addr < 0xb700; addr++) {
            kc->cpu.mem.w8(addr, 0);
        }
        kc->cpu.mem.w8(0xb7a0, 0);
        if (kc->cur_model == kc85_model::kc85_3) {
            kc->cpu.out(0x89, 0x9f);
            kc->cpu.mem.w16(kc->cpu.SP, 0xf15c);
        }
        else if (kc->cur_model == kc85_model::kc85_4) {
            kc->cpu.out(0x89, 0xFF);
            kc->cpu.mem.w16(kc->cpu.SP, 0xf17e);
        }

        // start address
        kc->cpu.PC = info.ExecAddr;
    }
}
