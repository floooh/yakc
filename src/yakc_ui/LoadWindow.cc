//------------------------------------------------------------------------------
//  LoadWindow.cc
//------------------------------------------------------------------------------
#include "LoadWindow.h"
#include "IMUI/IMUI.h"
#include "Core/String/StringBuilder.h"
#include "Util.h"
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

    ImGui::SetNextWindowSize(ImVec2(384, 220), ImGuiSetCond_Appearing);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        auto& ldr = *this->loader;
        if (!ldr.ExtFileReady && ldr.State != FileLoader::Ready) {
            #if ORYOL_EMSCRIPTEN
            ImGui::Text("Drag and drop a file into the emulator!");
            #else
            static char urlBuf[256] = "";
            ImGui::Text("http://localhost:8000/"); ImGui::SameLine();
            if (ImGui::InputText("##url", urlBuf, sizeof(urlBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
                FileLoader::Item item("", urlBuf, filetype::none, system::any);
                ldr.Load(item);
            }
            #endif
        }
        else {
            // these must match the FileLoader::FileType enum
            static const char* typeNames[] = {
                "RAW",
                "KCC",
                "KC TAP",
                "KC Z80",
                "ZX TAP",
                "ZX Z80",
                "CPC SNA",
                "CPC TAP",
                "CPC BIN",
                "ATOM TAP",
                "C64 TAP",
                "TEXT",
            };
            static_assert(int(sizeof(typeNames)/sizeof(const char*)) == int(filetype::num), "FileType mismatch");
            int curFileType = (int) ldr.Info.Type;
            if (ImGui::Combo("File Type", &curFileType, typeNames, int(filetype::num))) {
                // reparse loaded data
                FileLoader::Item item("", ldr.Info.Filename.AsCStr(), (filetype)curFileType, system::any);
                ldr.Info = ldr.parseHeader(ldr.FileData, item);
            }
            ImGui::Text("Filename: %s", ldr.Info.Filename.AsCStr());
            ImGui::Text("Name:     %s", ldr.Info.Name.AsCStr());
            ImGui::Checkbox("Enable Joystick", &ldr.Info.EnableJoystick);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            const char* compatMsg = " ";
            if (!emu.is_system(ldr.Info.RequiredSystem)) {
                switch (ldr.Info.RequiredSystem) {
                    case system::zxspectrum48k:
                        compatMsg = "Please reboot to ZX Spectrum 48K";
                        break;
                    case system::zxspectrum128k:
                        compatMsg = "Please reboot to ZX Spectrum 128K";
                        break;
                    case system::cpc464:
                        compatMsg = "Please reboot to Amstrad CPC464";
                        break;
                    case system::cpc6128:
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
                ldr.Copy();
                this->Visible = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Load & Start")) {
                ldr.Start();
                this->Visible = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset")) {
                ldr.State = FileLoader::Waiting;
            }
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC

