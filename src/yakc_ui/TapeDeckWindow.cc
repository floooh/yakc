//------------------------------------------------------------------------------
//  TapeDeckWindow.cc
//------------------------------------------------------------------------------
#include "TapeDeckWindow.h"
#include "IMUI/IMUI.h"
#include <cmath>

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
TapeDeckWindow::TapeDeckWindow(FileLoader* loader) {
    this->fileLoader = loader;
}

//------------------------------------------------------------------------------
void
TapeDeckWindow::Setup(yakc& emu) {
    this->setName("Tape Deck");
    this->angle = 0.0f;
}

//------------------------------------------------------------------------------
bool
TapeDeckWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(180, 136), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders|ImGuiWindowFlags_NoResize)) {

        if (tape.is_playing()) {
            this->angle += 0.025f;
        }

        // cassette tape window
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 s(164, 48);
        const ImVec2 p_mid(p.x + s.x/2, p.y + s.y/2);
        ImGui::InvisibleButton("canvas", s);
        ImVec2 l_mid(p_mid.x-36, p_mid.y), r_mid(p_mid.x+36, p_mid.y);
        float ln_x0 = l_mid.x + 14, ln_x1 = r_mid.x - 14;
        float ln_y0 = l_mid.y - 14, ln_y1 = r_mid.y + 12;
        const ImU32 black = 0xFF222222;
        const ImU32 light_gray = 0xFFCCCCCC;
        const ImU32 dark_gray  = 0xFFAAAAAA;
        const ImU32 darker_gray = 0xFF666666;
        draw_list->AddRectFilled(p, ImVec2(p.x + s.x, p.y + s.y), light_gray);
        draw_list->AddRect(p, ImVec2(p.x + s.x, p.y + s.y), black);
        draw_list->AddCircleFilled(l_mid, 16, dark_gray, 36);
        draw_list->AddCircleFilled(r_mid, 16, dark_gray, 36);
        draw_list->AddCircle(l_mid, 16, black, 36);
        draw_list->AddCircle(r_mid, 16, black, 36);
        draw_list->AddLine(ImVec2(ln_x0, ln_y0), ImVec2(ln_x1, ln_y0), black, 1);
        draw_list->AddLine(ImVec2(ln_x0, ln_y1), ImVec2(ln_x1, ln_y1), black, 1);
        float da = 2*3.141526f / 6;
        float a = angle;
        float rd0 = 15.5f, rd1 = 10.0f;
        for (int i=0; i < 6; i++, a+=da) {
            float s = sin(a);
            float c = cos(a);
            draw_list->AddLine(ImVec2(l_mid.x+s*rd0, l_mid.y+c*rd0), ImVec2(l_mid.x+s*rd1, l_mid.y+c*rd1), black, 1);
            draw_list->AddLine(ImVec2(r_mid.x+s*rd0, r_mid.y+c*rd0), ImVec2(r_mid.x+s*rd1, r_mid.y+c*rd1), black, 1);
        }

        // draw control buttons
        ImVec2 btn_s(24, 24);
        ImVec2 t0(0, 0), t1(9, 8), t2(0, 18);
        ImVec2 r0(0, 0), r1(14, 14);
        ImVec2 p0, p1, p2;

        // play button
        p = ImGui::GetCursorScreenPos();
        if (ImGui::Button("##play", btn_s)) {
            tape.play();
        }
        p.x += 8; p.y += 3;
        p0.x=p.x+t0.x; p0.y=p.y+t0.y;
        p1.x=p.x+t1.x; p1.y=p.y+t1.y; p2.x=p.x+t2.x; p2.y=p.y+t2.y;
        draw_list->AddTriangleFilled(p0, p1, p2, darker_gray);

        // stop button
        ImGui::SameLine();
        p = ImGui::GetCursorScreenPos();
        if (ImGui::Button("##stop", btn_s)) {
            tape.stop_rewind();
        }
        p.x += 5; p.y += 5;
        draw_list->AddRectFilled(ImVec2(p.x+r0.x, p.y+r0.y), ImVec2(p.x+r1.x, p.y+r1.y), darker_gray);

        // draw tape counter
        p0 = ImGui::GetCursorScreenPos();
        ImGui::SameLine();
        p1 = ImGui::GetCursorScreenPos();
        ImGui::SetWindowFontScale(2.0f);
        ImGui::SetCursorScreenPos(ImVec2(p1.x+32, p1.y-4));
        ImGui::Text("%03d", tape.counter());
        ImGui::SetWindowFontScale(1.0f);
        ImGui::SetCursorScreenPos(p0);

        // insert tape button
        const char* tape_name = tape.tape_name();
        if (!tape_name[0]) {
            tape_name = "insert tape";
        }
        if (ImGui::Button(tape_name, ImVec2(164,0))) {
            ImGui::OpenPopup("select");
        }
        if (ImGui::BeginPopup("select")) {
            for (const auto& item : this->fileLoader->Items) {
                if (!filetype_quickloadable(item.Type) && (int(item.Compat) & int(emu.model)))
                {
                    if (ImGui::MenuItem(item.Name.AsCStr())) {
                        this->fileLoader->LoadTape(item);
                    }
                }
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
