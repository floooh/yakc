//------------------------------------------------------------------------------
//  LoadWindow.cc
//------------------------------------------------------------------------------
#include "LoadWindow.h"
#include "IMUI/IMUI.h"
#include "Core/String/StringBuilder.h"
#include <string.h>

OryolClassImpl(LoadWIndow);

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
LoadWindow::~LoadWindow() {
    if (this->ioQueue.IsStarted()) {
        this->ioQueue.Stop();
    }
}

//------------------------------------------------------------------------------
void
LoadWindow::Setup(kc85& kc) {
    this->setName("Load File");
    this->files.Add("pengo.kcc");
    this->files.Add("cave.kcc");
    this->files.Add("labyrinth.kcc");
    this->files.Add("house.kcc");
    this->files.Add("jungle.kcc");
    this->ioQueue.Start();
}

//------------------------------------------------------------------------------
bool
LoadWindow::Draw(kc85& kc) {
    ImGui::SetNextWindowSize(ImVec2(512, 256), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {

        // item list
        static int selected = 0;
        ImGui::BeginChild("kcclist", ImVec2(150,0), true);
        for (int i = 0; i < this->files.Size(); i++) {
            if (ImGui::Selectable(this->files[i].AsCStr(), selected == i)) {
                selected = i;

                // start downloading file...
                StringBuilder strBuilder;
                strBuilder.Format(128, "kcc:%s", this->files[i].AsCStr());
                this->url = strBuilder.GetString();
                this->state = Loading;
                this->ioQueue.Add(strBuilder.GetString(),
                    // load succeeded
                    [this](const Ptr<Stream>& data) {
                        this->kccData = data;
                        this->state = Ready;
                    },
                    // load failed
                    [this](const URL& url, IOStatus::Code ioStatus) {
                        this->state = Failed;
                        this->failedStatus = ioStatus;
                    });
            }
        }
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::BeginChild("status", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()));    // leave room for 1 row below
        if (Waiting == this->state) {
            ImGui::Text("Select a file to load...");
        }
        else if (Loading == this->state) {
            ImGui::Text("Loading:\n\n%s", this->url.AsCStr());
        }
        else if (Failed == this->state) {
            ImGui::Text("Failed to load:\n\n%s\n\nReason: %d (%s)", this->url.AsCStr(), this->failedStatus, IOStatus::ToString(this->failedStatus));
        }
        else if (Ready == this->state) {
            fileInfo info = parseHeader(this->kccData);
            ImGui::Text("Name: %s", info.name.AsCStr());
            ImGui::Text("Start Address: 0x%04X", info.startAddr);
            ImGui::Text("End Address:   0x%04X", info.endAddr);
            if (info.hasExecAddr) {
                ImGui::Text("Exec Address:  0x%04X", info.execAddr);
            }
            else {
                ImGui::Text("No executable address in KCC file.");
            }
            if (info.fileSizeError) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "File Size Error!");
            }
            if (ImGui::Button("Load")) {
                copy(kc, info, this->kccData);
                this->Visible = false;
            }
            ImGui::SameLine();
            if (info.hasExecAddr) {
                if (ImGui::Button("Load & Start")) {
                    copy(kc, info, this->kccData);
                    start(kc, info);
                    this->Visible = false;
                }
            }
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::End();
    return this->Visible;
}

//------------------------------------------------------------------------------
LoadWindow::fileInfo
LoadWindow::parseHeader(const Ptr<Stream>& data) {
    fileInfo info;
    if (data->Open(OpenMode::ReadOnly)) {
        const kcc_header* hdr = (const kcc_header*) data->MapRead(nullptr);
        info.name = String((const char*)hdr->name, 0, 16);
        info.startAddr = hdr->load_addr_h<<8 | hdr->load_addr_l;
        info.endAddr = hdr->end_addr_h<<8 | hdr->end_addr_l;
        info.execAddr = hdr->exec_addr_h<<8 | hdr->exec_addr_l;
        info.hasExecAddr = hdr->num_addr > 2;
        if ((info.endAddr-info.startAddr) > data->Size()-128) {
            info.fileSizeError = true;
        }
        data->UnmapRead();
        data->Close();
    }
    return info;
}

//------------------------------------------------------------------------------
void
LoadWindow::copy(kc85& kc, const fileInfo& info, const Ptr<Stream>& data) {
    if (!info.fileSizeError) {
        if (data->Open(OpenMode::ReadOnly)) {
            const ubyte* end;
            const ubyte* kcc = data->MapRead(&end);
            kcc += 128; // skip header
            kc.cpu.mem.write(info.startAddr, kcc, info.endAddr-info.startAddr);
            data->UnmapRead();
            data->Close();
        }
    }
}

//------------------------------------------------------------------------------
void
LoadWindow::start(kc85& kc, const fileInfo& info) {
    if (info.hasExecAddr) {
        // reset volume
        kc.cpu.out(0x89, 0x9f);
        kc.cpu.state.PC = info.execAddr;

        // FIXME: patch JUNGLE until I have time to do a proper
        // 'restoration', see Alexander Lang's KC emu here:
        // http://lanale.de/kc85_emu/KC85_Emu.html
        if (info.name == "JUNGLE     ") {
            // patch start level 1 into memory
            auto& mem = kc.cpu.mem;
            mem.w8(0x36b7, 1);
            mem.w8(0x3697, 1);
            for (int i = 0; i < 5; i++) {
                mem.w8(0x1770 + i, mem.r8(0x36b6 + i));
            }
        }
    }
}
