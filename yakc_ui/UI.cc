//------------------------------------------------------------------------------
//  UI.cc
//------------------------------------------------------------------------------
#include "UI.h"
#include "yakc_app/Util.h"
#include "CpuWindow.h"
#include "MemoryWindow.h"
#include "Core/Containers/StaticArray.h"
#include "IMUI/IMUI.h"
#include "Time/Clock.h"
#include "Input/Input.h"
#include "Core/String/StringBuilder.h"

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
UI::Setup(kc85& kc) {
    IMUI::Setup();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Alpha = 1.0f;
    style.WindowFillAlphaDefault = 1.0f;
    style.WindowTitleAlign = ImGuiAlign_Center;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);

    this->curTime = Clock::Now();
}

//------------------------------------------------------------------------------
void
UI::Discard() {
    IMUI::Discard();
}

//------------------------------------------------------------------------------
void
UI::OpenWindow(const kc85& kc, const Ptr<WindowBase>& win) {
    win->Setup(kc);
    this->windows.Add(win);
}

//------------------------------------------------------------------------------
void
UI::OnFrame(kc85& kc) {
    IMUI::NewFrame(Clock::LapTime(this->curTime));

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(kc.model() == kc85::kc_model::kc85_3 ? "KC85/3":"KC85/4")) {
            if (ImGui::MenuItem("Power Cycle...")) {
                kc.switchoff();
                kc.switchon(kc.model());
            }
            if (ImGui::MenuItem("Reset...")) {
                kc.reset();
            }
            if (ImGui::MenuItem("Reboot to KC85/3... (TODO)")) {
                // FIXME
            }
            if (ImGui::MenuItem("Reboot to KC85/4... (TODO)")) {
                // FIXME
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debugger")) {
            if (ImGui::MenuItem("CPU State...")) {
                this->OpenWindow(kc, CpuWindow::Create());
            }
            StringBuilder strBuilder;
            for (int i = 0; i < memory::num_banks; i++) {
                static const char* bank_names[memory::num_banks] = {
                    "RAM0", "RAM1", "IRM", "ROM"
                };
                if (kc.cpu.mem.get_bank_ptr(i)) {
                    strBuilder.Format(32, "Memory %s...", bank_names[i]);
                    if (ImGui::MenuItem(strBuilder.GetString().AsCStr())) {
                        auto win = MemoryWindow::Create();
                        win->MemoryBankIndex = i;
                        this->OpenWindow(kc, win);
                    }
                }
            }
            if (ImGui::MenuItem("Disassembler... (TODO)")) {
                // FIXME
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // draw open windows
    for (auto& win : this->windows) {
        win->Draw(kc);
    }
    ImGui::Render();

    // delete closed windows
    for (int i = this->windows.Size() - 1; i >= 0; i--) {
        if (!this->windows[i]->Visible) {
            this->windows.Erase(i);
        }
    }
}
