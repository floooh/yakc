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
    this->Items.Add("Pengo (KC85/3)", "pengo.kcc", kc85_model::kc85_3);
    this->Items.Add("Pengo (KC85/4)", "pengo4.kcc", kc85_model::kc85_4);
    this->Items.Add("Cave (KC85/3)", "cave.kcc", kc85_model::kc85_3);
    this->Items.Add("Labyrinth (KC85/3)", "labyrinth.kcc", kc85_model::kc85_3);
    this->Items.Add("House (KC85/3)", "house.kcc", kc85_model::kc85_3);
    this->Items.Add("House (KC85/4)", "house4.tap", kc85_model::kc85_4);
    this->Items.Add("Jungle (KC85/3)", "jungle.kcc", kc85_model::kc85_3);
    this->Items.Add("Jungle (KC85/4)", "jungle4.tap", kc85_model::kc85_4);
    this->Items.Add("Pacman (KC85/3)", "pacman.kcc", kc85_model::kc85_3);
    this->Items.Add("Breakout (KC85/3)", "breakout.kcc", kc85_model::kc85_3);
    this->Items.Add("Boulderdash (KC85/3)", "boulder3.tap", kc85_model::kc85_3);
    this->Items.Add("Boulderdash (KC85/4)", "boulder4.tap", kc85_model::kc85_4);
    this->Items.Add("Digger (KC85/3)", "digger-3.kcc", kc85_model::kc85_3);
    this->Items.Add("Digger (KC85/4)", "digger4.tap", kc85_model::kc85_4);
    this->Items.Add("Ladder (KC85/3)", "ladder-3.kcc", kc85_model::kc85_3);
    this->Items.Add("Chess (KC85/3+KC85/4)", "chess.kcc", kc85_model::any);
    this->Items.Add("Testbild (KC85/3)", "testbild.kcc", kc85_model::kc85_3);
    this->Items.Add("Demo1 (KC85/4)", "demo1.kcc", kc85_model::kc85_4);
    this->Items.Add("Demo2 (KC85/4)", "demo2.kcc", kc85_model::kc85_4);
    this->Items.Add("Demo3 (KC85/4)", "demo3.kcc", kc85_model::kc85_4);
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
        [this, kc, autostart](const Ptr<Stream>& data) {
            this->kccData = data;
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
FileLoader::parseHeader(const Ptr<Stream>& data) {
    FileInfo info;
    if (data->Open(OpenMode::ReadOnly)) {
        const ubyte* start = data->MapRead(nullptr);
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
        info.PayloadOffset = (ptr + sizeof(kcc_header)) - start;
        if ((info.EndAddr-info.StartAddr) > data->Size()-128) {
            info.FileSizeError = true;
        }
        data->UnmapRead();
        data->Close();
    }
    return info;
}

//------------------------------------------------------------------------------
void
FileLoader::copy(kc85* kc, const FileInfo& info, const Ptr<Stream>& data) {
    if (!info.FileSizeError) {
        if (data->Open(OpenMode::ReadOnly)) {
            const ubyte* end;
            const ubyte* payload = data->MapRead(&end) + info.PayloadOffset;
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
            data->UnmapRead();
            data->Close();
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
    if (info.Name == "DIGGER/3COM") {
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
        // reset volume
        kc->cpu.out(0x89, 0x9f);
        kc->cpu.state.PC = info.ExecAddr;
    }
}
