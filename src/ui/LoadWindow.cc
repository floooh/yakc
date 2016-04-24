//------------------------------------------------------------------------------
//  LoadWindow.cc
//------------------------------------------------------------------------------
#include "LoadWindow.h"
#include "IMUI/IMUI.h"
#include "Core/String/StringBuilder.h"
#include <string.h>

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
LoadWindow::SetFileLoader(FileLoader* fileLoader) {
    this->loader = fileLoader;
}

//------------------------------------------------------------------------------
void
LoadWindow::Setup(kc85& kc) {
    YAKC_ASSERT(this->loader);
    this->setName("Load File");
}

//------------------------------------------------------------------------------
bool
LoadWindow::Draw(kc85& kc) {
    YAKC_ASSERT(this->loader);

    ImGui::SetNextWindowSize(ImVec2(512, 256), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {

        // item list
        static int selected = 0;
        int curr = 0;
        ImGui::BeginChild("kcclist", ImVec2(250,0), true);
        for (const auto& item : this->loader->Items) {
            if (int(item.Compat) & int(kc.model())) {
                if (ImGui::Selectable(item.Name.AsCStr(), selected == curr)) {
                    selected = curr;
                    this->loader->Load(kc, item);
                }
                curr++;
            }
        }
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::BeginChild("status", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()));    // leave room for 1 row below
        if (FileLoader::Waiting == this->loader->State) {
            ImGui::Text("Select a file to load...");
        }
        else if (FileLoader::Loading == this->loader->State) {
            ImGui::Text("Loading:\n\n%s", this->loader->Url.AsCStr());
        }
        else if (FileLoader::Failed == this->loader->State) {
            ImGui::Text("Failed to load:\n\n%s\n\nReason: %d (%s)",
                this->loader->Url.AsCStr(),
                this->loader->FailedStatus,
                IOStatus::ToString(this->loader->FailedStatus));
        }
        else if (FileLoader::Ready == this->loader->State) {
            ImGui::Text("Name: %s", this->loader->Info.Name.AsCStr());
            ImGui::Text("Type: %s", (this->loader->Info.Type == FileLoader::FileType::KCC) ? "KCC":"TAP");
            ImGui::Text("Start Address: 0x%04X", this->loader->Info.StartAddr);
            ImGui::Text("End Address:   0x%04X", this->loader->Info.EndAddr);
            if (this->loader->Info.HasExecAddr) {
                ImGui::Text("Exec Address:  0x%04X", this->loader->Info.ExecAddr);
            }
            else {
                ImGui::Text("No executable address in file.");
            }
            if (this->loader->Info.FileSizeError) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "File Size Error!");
            }
            if (ImGui::Button("Load")) {
                this->loader->Copy(kc);
                this->Visible = false;
            }
            ImGui::SameLine();
            if (this->loader->Info.HasExecAddr) {
                if (ImGui::Button("Load & Start")) {
                    this->loader->Start(kc);
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

