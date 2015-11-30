//------------------------------------------------------------------------------
//  UI.cc
//------------------------------------------------------------------------------
#include "UI.h"
#include "yakc_app/Util.h"
#include "MemoryWindow.h"
#include "MemoryMapWindow.h"
#include "DebugWindow.h"
#include "DisasmWindow.h"
#include "PIOWindow.h"
#include "CTCWindow.h"
#include "ModuleWindow.h"
#include "KeyboardWindow.h"
#include "LoadWindow.h"
#include "Time/Clock.h"
#include "Input/Input.h"
#include "Core/String/StringBuilder.h"
#include "yakc_roms/roms.h"

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

    this->fileLoader.Setup(kc);
    this->curTime = Clock::Now();
}

//------------------------------------------------------------------------------
void
UI::Discard() {
    this->fileLoader.Discard();
    this->windows.Clear();
    IMUI::Discard();
}

//------------------------------------------------------------------------------
void
UI::Toggle() {
    this->uiEnabled = !this->uiEnabled;
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
    if (this->uiEnabled) {
        if (ImGui::BeginMainMenuBar()) {
            const char* model;
            switch (kc.model()) {
                case kc85_model::kc85_2: model = "KC85/2"; break;
                case kc85_model::kc85_3: model = "KC85/3"; break;
                case kc85_model::kc85_4: model = "KC85/4"; break;
                default: model="??"; break;
            }
            if (ImGui::BeginMenu(model)) {
                if (ImGui::MenuItem("Load File...")) {
                    auto loadWindow = LoadWindow::Create();
                    loadWindow->SetFileLoader(&this->fileLoader);
                    this->OpenWindow(kc, loadWindow);
                }
                if (ImGui::MenuItem("Power Cycle")) {
                    kc.poweroff();
                    kc.poweron(kc.model(), kc.caos());
                }
                if (ImGui::MenuItem("Reset")) {
                    kc.reset();
                }
                if (ImGui::BeginMenu("Boot to KC85/2")) {
                    if (ImGui::MenuItem("CAOS 2.1")) {
                        kc.poweroff();
                        kc.poweron(kc85_model::kc85_2, kc85_caos::caos_2_1);
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Boot to KC85/3")) {
                    if (ImGui::MenuItem("CAOS 3.1")) {
                        kc.poweroff();
                        kc.poweron(kc85_model::kc85_3, kc85_caos::caos_3_1);
                    }
                    if (ImGui::MenuItem("CAOS 3.4")) {
                        kc.poweroff();
                        kc.poweron(kc85_model::kc85_3, kc85_caos::caos_3_4);
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Boot to KC85/4")) {
                    if (ImGui::MenuItem("CAOS 4.1")) {
                        kc.poweroff();
                        kc.poweron(kc85_model::kc85_4, kc85_caos::caos_4_1);
                    }
                    if (ImGui::MenuItem("CAOS 4.2")) {
                        kc.poweroff();
                        kc.poweron(kc85_model::kc85_4, kc85_caos::caos_4_2);
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Games")) {
                for (const auto& item : this->fileLoader.Items) {
                    if (int(item.Compat) & int(kc.model())) {
                        if (ImGui::MenuItem(item.Name.AsCStr())) {
                            this->fileLoader.LoadAndStart(kc, item);
                        }
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Hardware")) {
                if (ImGui::MenuItem("Keyboard")) {
                    this->OpenWindow(kc, KeyboardWindow::Create());
                }
                if (ImGui::MenuItem("Expansion Slots")) {
                    this->OpenWindow(kc, ModuleWindow::Create());
                }
                if (ImGui::MenuItem("Memory Map")) {
                    this->OpenWindow(kc, MemoryMapWindow::Create());
                }
                if (ImGui::MenuItem("Z80 PIO")) {
                    this->OpenWindow(kc, PIOWindow::Create());
                }
                if (ImGui::MenuItem("Z80 CTC")) {
                    this->OpenWindow(kc, CTCWindow::Create());
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Debugging")) {
                if (ImGui::MenuItem("Debugger")) {
                    this->OpenWindow(kc, DebugWindow::Create());
                }
                if (ImGui::MenuItem("Disassembler")) {
                    this->OpenWindow(kc, DisasmWindow::Create());
                }
                if (ImGui::MenuItem("Memory Editor")) {
                    this->OpenWindow(kc, MemoryWindow::Create());
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Settings")) {
                if (ImGui::MenuItem("CRT Effect", nullptr, this->Settings.crtEffect)) {
                    this->Settings.crtEffect = !this->Settings.crtEffect;
                }
                if (ImGui::MenuItem("Color TV", nullptr, this->Settings.colorTV)) {
                    this->Settings.colorTV = !this->Settings.colorTV;
                }
                ImGui::SliderFloat("CRT Warp", &this->Settings.crtWarp, 0.0f, 1.0f/16.0f);
                ImGui::SliderInt("CPU Speed", &this->Settings.cpuSpeed, 1, 8, "%.0fx");
                if (ImGui::MenuItem("Reset To Defaults")) {
                    this->Settings = settings();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // draw open windows
        for (auto& win : this->windows) {
            win->Draw(kc);
        }
    }
    else {
        // if UI is disabled, draw a simple overlay with help on how to toggle UI
        if (helpOpen) {
            ImGui::SetNextWindowPosCenter();
            if (ImGui::Begin("Help", &this->helpOpen, ImVec2(0,0), 0.75f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize))
            {
                ImGui::Text("Press TAB or double-tap to toggle UI!");
                ImGui::Dummy(ImVec2(96,0)); ImGui::SameLine();
                if (ImGui::Button("Got it!")) {
                    this->helpOpen = false;
                }
            }
            ImGui::End();
        }
    }
    ImGui::Render();

    // delete closed windows
    for (int i = this->windows.Size() - 1; i >= 0; i--) {
        if (!this->windows[i]->Visible) {
            this->windows.Erase(i);
        }
    }
}
