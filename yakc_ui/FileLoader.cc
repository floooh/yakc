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
    this->Items.Add("Cave", "cave.kcc", kc85_model::kc85_3);
    this->Items.Add("Labyrinth", "labyrinth.kcc", kc85_model::kc85_3);
    this->Items.Add("House", "house.kcc", kc85_model::kc85_3);
    this->Items.Add("Jungle", "jungle.kcc", kc85_model::kc85_3);
    this->Items.Add("Pacman", "pacman.kcc", kc85_model::kc85_3);
    this->Items.Add("Breakout", "breakout.kcc", kc85_model::kc85_3);
// some don't properly work yet
//    this->Items.Add("Boulderdash", "bould-3.kcc");
//    this->Items.Add("Digger", "digger-3.kcc");
    this->Items.Add("Ladder", "ladder-3.kcc", kc85_model::kc85_3);
    this->Items.Add("Chess", "chess.kcc", kc85_model::any);
    this->Items.Add("Testbild", "testbild.kcc", kc85_model::any);
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
        const kcc_header* hdr = (const kcc_header*) data->MapRead(nullptr);
        info.Name = String((const char*)hdr->name, 0, 16);
        info.StartAddr = hdr->load_addr_h<<8 | hdr->load_addr_l;
        info.EndAddr = hdr->end_addr_h<<8 | hdr->end_addr_l;
        info.ExecAddr = hdr->exec_addr_h<<8 | hdr->exec_addr_l;
        info.HasExecAddr = hdr->num_addr > 2;
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
            const ubyte* kcc = data->MapRead(&end);
            kcc += 128; // skip header
            kc->cpu.mem.write(info.StartAddr, kcc, info.EndAddr-info.StartAddr);
            data->UnmapRead();
            data->Close();
        }
    }
}

//------------------------------------------------------------------------------
void
FileLoader::start(kc85* kc, const FileInfo& info) {
    if (info.HasExecAddr) {
        // reset volume
        kc->cpu.out(0x89, 0x9f);
        kc->cpu.state.PC = info.ExecAddr;

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
    }
}
