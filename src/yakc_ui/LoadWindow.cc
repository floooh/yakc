//------------------------------------------------------------------------------
//  LoadWindow.cc
//------------------------------------------------------------------------------
#include "LoadWindow.h"
#include "IMUI/IMUI.h"
#include "Core/String/StringBuilder.h"
#include <string.h>

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
LoadWindow::LoadWindow(FileLoader* fileLoader):
    loader(fileLoader) { }

//------------------------------------------------------------------------------
void
LoadWindow::Setup(yakc& emu) {
    YAKC_ASSERT(this->loader);
    this->setName("Load File");
}

//------------------------------------------------------------------------------
bool
LoadWindow::Draw(yakc& emu) {
    YAKC_ASSERT(this->loader);

    ImGui::SetNextWindowSize(ImVec2(384, 200), ImGuiSetCond_Appearing);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        auto& ldr = *this->loader;
        if (!ldr.ExtFileReady && ldr.State != FileLoader::Ready) {
            #if ORYOL_EMSCRIPTEN
            ImGui::Text("Drag and drop a file into the emulator!");
            #else
            static char urlBuf[256] = "";
            ImGui::Text("http://localhost:8000/"); ImGui::SameLine();
            if (ImGui::InputText("##url", urlBuf, sizeof(urlBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
                FileLoader::Item item("", urlBuf, FileLoader::FileType::None, device::any);
                ldr.Load(item);
            }
            #endif
        }
        else {
            static const char* typeNames[] = {
                "RAW",
                "KCC",
                "KC TAP",
                "KC Z80",
                "ZX TAP",
                "ZX Z80",
                "CPC SNA"
            };
            int curFileType = (int) ldr.Info.Type;
            if (ImGui::Combo("File Type", &curFileType, typeNames, int(FileLoader::FileType::Num))) {
                // reparse loaded data
                FileLoader::Item item("", ldr.Info.Filename.AsCStr(), (FileLoader::FileType)curFileType, device::any);
                ldr.Info = ldr.parseHeader(ldr.FileData, item);
            }
            ImGui::Text("Filename: %s", ldr.Info.Filename.AsCStr());
            ImGui::Text("Name:     %s", ldr.Info.Name.AsCStr());
            this->startAddr.Configure16("Start Address", ldr.Info.StartAddr);
            if (this->startAddr.Draw()) {
                ldr.Info.StartAddr = this->startAddr.Get16();
            }
            this->endAddr.Configure16("End Address", ldr.Info.EndAddr);
            if (this->endAddr.Draw()) {
                ldr.Info.EndAddr = this->endAddr.Get16();
            }
            this->execAddr.Configure16("Exec Address", ldr.Info.ExecAddr);
            const bool execAddrInvalid = ((ldr.Info.ExecAddr < ldr.Info.StartAddr) || (ldr.Info.ExecAddr > ldr.Info.EndAddr));
            if (execAddrInvalid) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            }
            if (this->execAddr.Draw()) {
                ldr.Info.ExecAddr = this->execAddr.Get16();
            }
            if (execAddrInvalid) {
                ImGui::PopStyleColor();
            }
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            const char* compatMsg = " ";
            if (!emu.is_device(ldr.Info.RequiredSystem)) {
                switch (ldr.Info.RequiredSystem) {
                    case device::zxspectrum48k:
                        compatMsg = "Please reboot to ZX Spectrum 48K";
                        break;
                    case device::zxspectrum128k:
                        compatMsg = "Please reboot to ZX Spectrum 128K";
                        break;
                    case device::cpc464:
                        compatMsg = "Please reboot to Amstrad CPC464";
                        break;
                    case device::cpc6128:
                        compatMsg = "Please reboot to Amstrad CPC6128";
                        break;
                    default:
                        compatMsg = "File will not work on this system";
                        break;
                }
            }
            ImGui::Text("%s", compatMsg);
            ImGui::PopStyleColor();
            if (ImGui::Button("Load")) {
                this->loader->Copy();
                this->Visible = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Load & Start")) {
                this->loader->Start();
                this->Visible = false;
            }
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC

