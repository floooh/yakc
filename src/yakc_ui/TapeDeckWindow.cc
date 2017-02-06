//------------------------------------------------------------------------------
//  TapeDeckWindow.cc
//------------------------------------------------------------------------------
#include "TapeDeckWindow.h"
#include "IMUI/IMUI.h"
#include <cmath>

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
TapeDeckWindow::Setup(yakc& emu) {
    this->setName("GERACORD 2000");
    this->angle = 0.0f;
}

//------------------------------------------------------------------------------
bool
TapeDeckWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(320, 200), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {

        if (this->playing) {
            this->angle += 0.025f;
            this->counter = angle;
        }

        // cassette tape window
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 s(152, 48);
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
        float da = 2*3.141526 / 6;
        float a = angle;
        float r0 = 15.5f, r1 = 10.0f;
        for (int i=0; i < 6; i++, a+=da) {
            float s = sin(a);
            float c = cos(a);
            draw_list->AddLine(ImVec2(l_mid.x+s*r0, l_mid.y+c*r0), ImVec2(l_mid.x+s*r1, l_mid.y+c*r1), black, 1);
            draw_list->AddLine(ImVec2(r_mid.x+s*r0, r_mid.y+c*r0), ImVec2(r_mid.x+s*r1, r_mid.y+c*r1), black, 1);
        }

        // draw tape counter
        p = ImGui::GetCursorScreenPos();
        ImVec2 ts = ImGui::CalcTextSize("0");
        ImGui::SetCursorScreenPos(ImVec2(p_mid.x-1.8f*ts.x, p_mid.y-0.5f*ts.y));
        ImGui::Text("%03d", this->counter);
        ImGui::SetCursorScreenPos(p);

        // draw control buttons
        ImVec2 btn_s(32, 32);
        {
            //
            p = ImGui::GetCursorScreenPos();
            if (ImGui::Button("##back", btn_s)) {

            }
        }
        ImGui::SameLine();
        p = ImGui::GetCursorScreenPos();
        if (ImGui::Button("##ffwd", btn_s)) {

        }
        ImGui::SameLine();
        {
            // play button
            p = ImGui::GetCursorScreenPos();
            if (ImGui::Button("##play", btn_s)) {
                this->playing = true;
            }
            ImVec2 p0(p.x+7, p.y+6), p1(p.x+27, p.y+16), p2(p.x+7, p.y+26);
            draw_list->AddTriangleFilled(p0, p1, p2, darker_gray);
        }
        ImGui::SameLine();
        {
            // the stop button
            p = ImGui::GetCursorScreenPos();
            if (ImGui::Button("##stop", btn_s)) {
                this->playing = false;
            }
            draw_list->AddRectFilled(ImVec2(p.x+7, p.y+7), ImVec2(p.x+25, p.y+25), darker_gray, 2);
        }
    }
    ImGui::End();
    return this->Visible;
}


} // namespace YAKC
