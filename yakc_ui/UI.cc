//------------------------------------------------------------------------------
//  UI.cc
//------------------------------------------------------------------------------
#include "UI.h"
#include "yakc_app/Util.h"
#include "MemoryWindow.h"
#include "DebugWindow.h"
#include "PIOWindow.h"
#include "CTCWindow.h"
#include "ModuleWindow.h"
#include "KeyboardWindow.h"
#include "Time/Clock.h"
#include "Input/Input.h"
#include "Core/String/StringBuilder.h"
#include "yakc_core/roms.h"

using namespace Oryol;
using namespace yakc;

const ImVec4 UI::ColorText = ImColor(255, 255, 255).Value;
const ImVec4 UI::ColorDetail = ImColor(164, 17, 6).Value;
const ImVec4 UI::ColorDetailBright = ImColor(230, 17, 6).Value;
const ImVec4 UI::ColorDetailDark = ImColor(94, 17, 6).Value;
const ImVec4 UI::ColorBackground = ImColor(32, 32, 32).Value;
const ImVec4 UI::ColorBackgroundLight = ImColor(96, 96, 96).Value;

//------------------------------------------------------------------------------
void
UI::Setup(kc85& kc) {
    IMUI::Setup();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Alpha = 1.0f;
    style.WindowFillAlphaDefault = 1.0f;
    style.WindowTitleAlign = ImGuiAlign_Center;
    style.TouchExtraPadding = ImVec2(5.0f, 5.0f);
    style.AntiAliasedLines = false;
    style.AntiAliasedShapes = false;

    /*
    style.Colors[ImGuiCol_Text] = ColorText;
    style.Colors[ImGuiCol_Border] = ColorDetail;
    style.Colors[ImGuiCol_TitleBg] = ColorDetail;
    style.Colors[ImGuiCol_FrameBg] = ColorBackgroundLight;
    style.Colors[ImGuiCol_FrameBgHovered] = ColorDetail;
    style.Colors[ImGuiCol_FrameBgActive] = ColorDetail;
    style.Colors[ImGuiCol_WindowBg] = ColorBackground;
    style.Colors[ImGuiCol_ChildWindowBg] = ColorBackground;
    style.Colors[ImGuiCol_TitleBgActive] = ColorDetail;
    style.Colors[ImGuiCol_MenuBarBg] = ColorDetail;
    style.Colors[ImGuiCol_CheckMark] = ColorDetailBright;
    style.Colors[ImGuiCol_SliderGrab] = ColorDetail;
    style.Colors[ImGuiCol_SliderGrabActive] = ColorDetail;
    style.Colors[ImGuiCol_Button] = ColorDetail;
    style.Colors[ImGuiCol_ButtonHovered] = ColorDetailBright;
    style.Colors[ImGuiCol_ButtonActive] = ColorDetailDark;
    style.Colors[ImGuiCol_ScrollbarBg] = ColorBackgroundLight;
    style.Colors[ImGuiCol_ScrollbarGrab] = ColorDetail;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ColorDetailBright;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ColorDetailBright;
    */

    this->curTime = Clock::Now();
}

//------------------------------------------------------------------------------
void
UI::Discard() {
    IMUI::Discard();
}

//------------------------------------------------------------------------------
void
UI::OpenWindow(kc85& kc, const Ptr<WindowBase>& win) {
    win->Setup(kc);
    this->windows.Add(win);
}

//------------------------------------------------------------------------------
void
UI::OnFrame(kc85& kc) {
    IMUI::NewFrame(Clock::LapTime(this->curTime));

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(kc.model() == kc85::kc_model::kc85_3 ? "KC85/3":"KC85/4")) {
            if (ImGui::MenuItem("Power Cycle")) {
                kc.switchoff();
                kc.switchon(kc.model(), kc.caos_rom(), kc.caos_rom_size());
            }
            if (ImGui::MenuItem("Reset")) {
                kc.reset();
            }
            if (ImGui::BeginMenu("KC 85/3")) {
                if (ImGui::MenuItem("CAOS 3.1")) {
                    kc.switchoff();
                    kc.switchon(kc85::kc_model::kc85_3, rom_caos31, sizeof(rom_caos31));
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("KC85/4 (TODO)")) {
                // FIXME
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows")) {
            if (ImGui::MenuItem("Keyboard")) {
                this->OpenWindow(kc, KeyboardWindow::Create());
            }
            if (ImGui::MenuItem("Modules")) {
                this->OpenWindow(kc, ModuleWindow::Create());
            }
            if (ImGui::MenuItem("Memory")) {
                this->OpenWindow(kc, MemoryWindow::Create());
            }
            if (ImGui::MenuItem("Debugger")) {
                this->OpenWindow(kc, DebugWindow::Create());
            }
            if (ImGui::MenuItem("Z80 PIO")) {
                this->OpenWindow(kc, PIOWindow::Create());
            }
            if (ImGui::MenuItem("Z80 CTC")) {
                this->OpenWindow(kc, CTCWindow::Create());
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
